// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (c) 2025 Oleksandr Kozlov

#include "auth.hpp"
#include "common/common.hpp"
#include "common/logger.hpp"
#include "common/time.hpp"
#include "game_data.hpp"
#include "proto/pref.pb.h"

#include <docopt/docopt.h>
#include <range/v3/all.hpp>

#include <charconv>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace pref {
namespace {

constexpr std::string_view Usage = R"(
Preferans CLI

Usage:
  pref-cli <path> add --user <name> <password>
  pref-cli <path> show --users
  pref-cli <path> show --user <id>
  pref-cli <path> show --games <id>
  pref-cli <path> show --game <game>
  pref-cli <path> show --tokens <id>
  pref-cli <path> remove --user <id>
  pref-cli <path> remove --games <id>
  pref-cli <path> remove --game <id> <game>
  pref-cli <path> remove --tokens <id> [--token=<token>]
  pref-cli (-h | --help)
)";

constexpr auto LogOnNone
    = [](const PlayerIdView playerId) { return OnNone([playerId] { PREF_W("{} not found", PREF_V(playerId)); }); };

constexpr auto sum = []<typename Rng>(Rng&& rng) { return rng::fold_left(std::forward<Rng>(rng), 0, std::plus{}); };

auto listUsers(const GameData& data) -> void
{
    auto nameWidth = std::size_t{};
    for (const auto& user : data.users()) { nameWidth = std::max(nameWidth, user.player_name().size()); }
    rng::for_each(data.users(), [nameWidth](const auto& user) {
        auto mmr = sum(user.games() | rv::transform([](auto&& game) { return game.mmr(); }));
        std::println("{} | {:<{}} | {:+d}", user.player_id(), user.player_name(), nameWidth, mmr);
    });
}

auto showUser(const GameData& data, const PlayerIdView playerId) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([](const User& user) {
        std::println("Name:     {}", user.player_name());
        std::println("ID:       {}", user.player_id());
        std::println("Password: {}", user.password());
        std::println("Tokens:   {}", user.auth_tokens_size());
        std::println("Games:    {}", user.games_size());
    });
}

auto showTokens(const GameData& data, const PlayerIdView playerId) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([](const User& user) {
        rng::for_each(user.auth_tokens(), [](const std::string_view token) { std::println("{}", token); });
    });
}

auto showGames(const GameData& data, const PlayerIdView playerId) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([](const User& user) {
        rng::for_each(user.games(), [](const UserGame& game) {
            std::println(
                "| #{:<2} | {} {} | {} | {} | {:>+4} | {}/{}/{}",
                game.id(),
                formatDate(game.timestamp()),
                formatTime(game.timestamp()),
                formatDuration(game.duration()),
                game.game_type() == GameType::NORMAL ? "Normal" : "Ranked",
                game.mmr(),
                game.pool(),
                game.dump(),
                game.whists());
        });
    });
}

auto showGame(const GameData& data, const std::int32_t gameId) -> void
{
    constexpr auto RedSuitAnsi = "\x1b[38;2;255;85;85m";
    constexpr auto AnsiReset = "\x1b[0m";
    const auto userNamesById = data.users() | rv::transform([](const User& user) {
        return std::pair{user.player_id(), user.player_name()};
    }) | rng::to<std::map<std::string, std::string>>;
    const auto cardCell = [RedSuitAnsi, AnsiReset](const std::string_view card) -> std::string {
        const auto rank = cardRank(card);
        const auto suit = cardSuit(card);
        const auto shortRank = [&]() -> std::string_view {
            if (rank == PREF_ACE) { return "A"; }
            if (rank == PREF_KING) { return "K"; }
            if (rank == PREF_QUEEN) { return "Q"; }
            if (rank == PREF_JACK) { return "J"; }
            if (rank == PREF_TEN) { return "10"; }
            if (rank == PREF_NINE) { return "9"; }
            if (rank == PREF_EIGHT) { return "8"; }
            if (rank == PREF_SEVEN) { return "7"; }
            return rank;
        }();
        const auto suitSign = [&]() -> std::string {
            if (suit == PREF_SPADES) { return std::string{SpadeSign}; }
            if (suit == PREF_CLUBS) { return std::string{ClubSign}; }
            if (suit == PREF_HEARTS) { return fmt::format("{}{}{}", RedSuitAnsi, HeartSign, AnsiReset); }
            if (suit == PREF_DIAMONDS) { return fmt::format("{}{}{}", RedSuitAnsi, DiamondSign, AnsiReset); }
            return std::string{suit};
        }();
        return shortRank == PREF_TEN ? fmt::format("{}{}", shortRank, suitSign)
                                     : fmt::format(" {}{}", shortRank, suitSign);
    };
    const auto suitValue = [](const std::string_view suit) -> int {
        static const auto map
            = std::map<std::string_view, int>{{PREF_SPADES, 1}, {PREF_DIAMONDS, 2}, {PREF_CLUBS, 3}, {PREF_HEARTS, 4}};
        return map.at(suit);
    };
    const auto cardLess = [&](const std::string_view lhs, const std::string_view rhs) {
        const auto lhsSuit = suitValue(cardSuit(lhs));
        const auto rhsSuit = suitValue(cardSuit(rhs));
        const auto lhsRank = rankValue(cardRank(lhs));
        const auto rhsRank = rankValue(cardRank(rhs));
        return std::tie(lhsSuit, lhsRank) < std::tie(rhsSuit, rhsRank);
    };
    const auto gameIt = rng::find(data.games(), gameId, &Game::id);
    if (gameIt == rng::end(data.games())) {
        PREF_W("{} not found", PREF_V(gameId));
        return;
    }
    std::println("Game #{} | deals: {}", gameIt->id(), gameIt->deals_size());
    for (const auto& deal : gameIt->deals()) {
        std::println("  Deal #{}", deal.id());
        const auto visibleLen = [](const std::string_view text) -> std::size_t {
            auto len = std::size_t{};
            for (auto i = 0uz; i < std::size(text);) {
                if (text[i] == '\x1b') {
                    ++i;
                    if (i < std::size(text) and text[i] == '[') {
                        while (i < std::size(text) and text[i] != 'm') { ++i; }
                        if (i < std::size(text)) { ++i; }
                    }
                    continue;
                }
                ++len;
                ++i;
            }
            return len;
        };
        const auto padRight = [&](const std::string& text, const std::size_t width) {
            const auto len = visibleLen(text);
            return fmt::format("{}{}", text, std::string(width > len ? width - len : 0, ' '));
        };
        auto talonSorted = deal.talon() | rng::to_vector;
        rng::sort(talonSorted, cardLess);
        const auto talonCells = talonSorted | rv::transform(cardCell) | rng::to_vector;
        const auto talonText = fmt::format("{}", fmt::join(talonCells, " "));

        auto hands
            = deal.hands() | rv::transform([](const auto& entry) { return std::pair{entry.first, entry.second}; })
            | rng::to_vector;
        rng::sort(hands, std::less{}, &decltype(hands)::value_type::first);
        auto maxNameLen = std::size_t{5};
        auto maxCardsLen = visibleLen(talonText);
        auto maxChoiceLen = std::size_t{0};
        auto rows = std::vector<std::tuple<std::string, std::string, std::string, std::int32_t>>{};
        for (const auto& [playerId, cards] : hands) {
            const auto nameIt = userNamesById.find(playerId);
            const auto& playerName = nameIt != std::end(userNamesById) ? nameIt->second : playerId;
            maxNameLen = std::max(maxNameLen, std::size(playerName));
            const auto decisionIt = deal.decisions().find(playerId);
            auto choice = decisionIt != std::end(deal.decisions()) ? decisionIt->second : std::string{};
            if (choice.empty() and playerId == deal.declarer_id()) { choice = deal.contract(); }
            const auto tricksIt = deal.tricks().find(playerId);
            const auto tricks = tricksIt != std::end(deal.tricks()) ? tricksIt->second : 0;
            auto cardsSorted = cards.cards() | rng::to_vector;
            rng::sort(cardsSorted, cardLess);
            const auto cardsCells = cardsSorted | rv::transform(cardCell) | rng::to_vector;
            auto cardsText = fmt::format("{}", fmt::join(cardsCells, " "));
            maxCardsLen = std::max(maxCardsLen, visibleLen(cardsText));
            maxChoiceLen = std::max(maxChoiceLen, std::size(choice));
            rows.emplace_back(std::string{playerName}, std::move(cardsText), std::move(choice), tricks);
        }
        std::println(
            "    {} | {}",
            fmt::format("{:<{}}", "Talon", maxNameLen),
            padRight(talonText, maxCardsLen));
        for (const auto& [name, cardsText, choice, tricks] : rows) {
            std::println(
                "    {} | {} | {} | {}",
                fmt::format("{:<{}}", name, maxNameLen),
                padRight(cardsText, maxCardsLen),
                fmt::format("{:<{}}", choice, maxChoiceLen),
                tricks);
        }
    }
}

auto removeTokens(GameData& data, const PlayerIdView playerId, const std::optional<std::string>& authToken) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([&authToken, playerId](User& user) {
        authToken | OnNone([&user, playerId] {
            PREF_I("Removed {} tokens for {}", user.auth_tokens_size(), PREF_V(playerId));
            user.clear_auth_tokens();
        }) | OnValue([&user, playerId](const std::string& token) {
            auto& tokens = *user.mutable_auth_tokens();
            const auto it = rng::remove(tokens, token);
            if (it == rng::end(tokens)) {
                PREF_W("{} not found for {}", PREF_V(token), PREF_V(playerId));
                return;
            }
            tokens.erase(it, rng::end(tokens));
            PREF_I("Removed {} from {}", PREF_V(token), PREF_V(playerId));
        });
    });
}

auto addUser(GameData& data, const PlayerName& name, const std::string& password) -> void
{
    auto& newUser = *data.add_users();
    newUser.set_player_id(generateUuid());
    newUser.set_player_name(name);
    newUser.set_password(hashPassword(password));
    newUser.set_version(1);
    PREF_I("Added profileId: {}", newUser.player_id());
}

auto removeUser(GameData& data, const PlayerNameView playerId) -> void
{
    auto& users = *data.mutable_users();
    const auto it = rng::remove(users, playerId, &User::player_id);
    if (it == rng::end(users)) {
        PREF_W("{} not found", PREF_V(playerId));
        return;
    }
    users.erase(it, rng::end(users));
    PREF_I("Removed {}", PREF_V(playerId));
}

auto removeGames(GameData& data, const PlayerNameView playerId) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([](User& user) {
        PREF_I("Removed {} games", user.games_size());
        user.clear_games();
    });
}

auto removeGame(GameData& data, const PlayerNameView playerId, const std::int32_t game) -> void
{
    userByPlayerId(data, playerId) | LogOnNone(playerId) | OnValue([playerId, game](User& user) {
        auto& games = *user.mutable_games();
        const auto it = rng::remove(games, game, &UserGame::id);
        if (it == rng::end(games)) {
            PREF_W("{} for {} not found", PREF_V(game), PREF_V(playerId));
            return;
        }
        games.erase(it, rng::end(games));
        PREF_I("Removed {} for {}", PREF_V(game), PREF_V(playerId));
    });
}

} // namespace
} // namespace pref

auto main(int argc, char** argv) -> int
{
    try {
        const auto args = docopt::docopt(std::string{pref::Usage}, {std::next(argv), std::next(argv, argc)});
        const auto path = args.at("<path>").asString();
        auto data = pref::loadGameData(path);
        const auto parseInt32 = [](const std::string& value) -> std::optional<std::int32_t> {
            auto number = std::int32_t{};
            const auto first = std::data(value);
            const auto last = std::next(first, std::ssize(value));
            if (std::from_chars(first, last, number).ec != std::errc{}) { return std::nullopt; }
            return number;
        };
        if (args.at("show").asBool()) {
            if (args.at("--users").asBool()) {
                pref::listUsers(data);
            } else if (args.at("--user").asBool()) {
                pref::showUser(data, args.at("<id>").asString());
            } else if (args.at("--tokens").asBool()) {
                pref::showTokens(data, args.at("<id>").asString());
            } else if (args.at("--games").asBool()) {
                pref::showGames(data, args.at("<id>").asString());
            } else if (args.at("--game").asBool()) {
                const auto game = args.at("<game>").asString();
                if (const auto gameId = parseInt32(game)) {
                    pref::showGame(data, *gameId);
                } else {
                    PREF_W("Invalid {}", PREF_V(game));
                    return 1;
                }
            }
        } else if (args.at("remove").asBool()) {
            if (args.at("--tokens").asBool()) {
                const auto tokenOpt
                    = args.at("--token").isString() ? std::optional{args.at("--token").asString()} : std::nullopt;
                pref::removeTokens(data, args.at("<id>").asString(), tokenOpt);
            } else if (args.at("--user").asBool()) {
                pref::removeUser(data, args.at("<id>").asString());
            } else if (args.at("--games").asBool()) {
                pref::removeGames(data, args.at("<id>").asString());
            } else if (args.at("--game").asBool()) {
                const auto game = args.at("<game>").asString();
                if (const auto num = parseInt32(game)) {
                    pref::removeGame(data, args.at("<id>").asString(), *num);
                } else {
                    PREF_W("Invalid {}", PREF_V(game));
                    return 1;
                }
            }
            pref::storeGameData(path, data);
        } else if (args.at("add").asBool()) {
            if (args.at("--user").asBool()) {
                pref::addUser(data, args.at("<name>").asString(), args.at("<password>").asString());
            }
            pref::storeGameData(path, data);
        }
        return 1;
    } catch (...) {
        return 0;
    }
}
