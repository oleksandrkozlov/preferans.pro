// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (c) 2025 Oleksandr Kozlov

#pragma once

#include "common/common.hpp"
#include "game_data.hpp"
#include "proto/pref.pb.h"
#include "serialization.hpp"
#include "server.hpp"
#include "transport.hpp"

#include <boost/asio.hpp>
#include <range/v3/all.hpp>

#include <cassert>
#include <coroutine>
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace pref {

using PlayerTurnData = std::tuple<Player::Id, GameStage, std::string, bool, int, CardsNames>;

[[nodiscard]] inline auto players() -> decltype(auto)
{
    return ctx().players | rv::values;
}

[[nodiscard]] inline auto findDeclarerId()
{
    const auto players = pref::players();
    return pref::find_if(
        players,
        [](const std::string_view bid) { return not std::empty(bid) and bid != PREF_PASS; },
        &Player::bid,
        &Player::id);
}

[[nodiscard]] inline auto getDeclarer() -> Player&
{
    const auto declarerId = findDeclarerId();
    assert(declarerId and ctx().players.contains(*declarerId) and "declarer exists");
    return ctx().players.at(*declarerId);
}

[[nodiscard]] inline auto playersIdents() -> PlayersIdents
{
    return players()
        | rv::transform([](const Player& player) { return PlayerIdent{player.id, player.name}; })
        | rng::to_vector;
}

inline auto sendToAll(std::string payload) -> task<>
{
    const auto channels = players() //
        | rv::transform([](const Player& player) { return player.conn.ch; })
        | rng::to_vector;
    co_await sendToMany(channels, std::move(payload));
}

inline auto forwardToOne(const Player::IdView playerId, const Message& msg) -> task<>
{
    return sendToOne(ctx().player(playerId).conn.ch, msg.SerializeAsString());
}

inline auto forwardToAll(const Message& msg) -> task<>
{
    return sendToAll(msg.SerializeAsString());
}

inline auto sendToAllExcept(std::string payload, const Player::IdView excludedId) -> task<>
{
    const auto channels = pref::players()
        | rv::filter(notEqualTo(excludedId), &Player::id)
        | rv::transform([](const Player& player) { return player.conn.ch; })
        | rng::to_vector;
    co_await sendToMany(channels, std::move(payload));
}

inline auto forwardToAllExcept(const Message& msg, const Player::IdView excludedId) -> task<>
{
    return sendToAllExcept(msg.SerializeAsString(), excludedId);
}

inline auto sendLoginResponse(
    const ChannelPtr& ch, std::string error, const Player::IdView playerId = {}, std::string authToken = {}) -> task<>
{
    co_await sendToOne(
        ch, makeLoginResponse(ctx().stage, playerId, std::move(authToken), playersIdents(), std::move(error)));
}

inline auto sendAuthResponse(const ChannelPtr& ch, std::string error, const Player::NameView playerName = {}) -> task<>
{
    co_await sendToOne(ch, makeAuthResponse(ctx().stage, playerName, playersIdents(), std::move(error)));
}

inline auto sendPlayerJoined(const PlayerSession& session) -> task<>
{
    return sendToAllExcept(makePlayerJoined(session.playerName, session.playerId), session.playerId);
}

inline auto sendPlayerLeft(Player::Id playerId) -> task<>
{
    return sendToAll(makePlayerLeft(std::move(playerId)));
}

inline auto sendReadyCheckToOne(const ChannelPtr& ch, const Player::IdView playerId, const ReadyCheckState state)
    -> task<>
{
    co_await sendToOne(ch, makeReadyCheck(playerId, state));
}

inline auto sendForehand() -> task<>
{
    return sendToAll(makeForehand(ctx().forehandId));
}

inline auto sendTableOrder() -> task<>
{
    if (std::size(ctx().tableOrder) != NumberOfPlayers) { co_return; }
    co_await sendToAll(makeTableOrder(ctx().tableOrder));
}

inline auto sendTableOrderToOne(const ChannelPtr& ch) -> task<>
{
    if (std::size(ctx().tableOrder) != NumberOfPlayers) { co_return; }
    co_await sendToOne(ch, makeTableOrder(ctx().tableOrder));
}

inline auto sendDealCardsExcept(const Player::IdView playerId, const Hand& hand) -> task<>
{
    return sendToAllExcept(makeDealCards(playerId, hand | rng::to_vector), playerId);
}

inline auto sendDealCardsFor(const ChannelPtr& ch, const Player::IdView playerId, const Hand& hand) -> task<>
{
    co_await sendToOne(ch, makeDealCards(playerId, hand | rng::to_vector));
}

inline auto sendPlayerTurn(const PlayerTurnData& playerTurn) -> task<>
{
    const auto& [playerId, stage, minBid, canHalfWhist, passRound, talon] = playerTurn;
    return sendToAll(makePlayerTurn(playerId, stage, minBid, canHalfWhist, passRound, talon));
}

inline auto sendBiddingToOne(const ChannelPtr& ch, const Player::IdView playerId, const std::string_view bid) -> task<>
{
    return sendToOne(ch, makeBidding(playerId, bid));
}

inline auto sendBidding(const Player::IdView playerId, const std::string_view bid) -> task<>
{
    return sendToAllExcept(makeBidding(playerId, bid), playerId);
}

inline auto sendWhistingToOne(const ChannelPtr& ch, const Player::IdView playerId, const std::string_view choice)
    -> task<>
{
    return sendToOne(ch, makeWhisting(playerId, choice));
}

inline auto sendHowToPlayToOne(const ChannelPtr& ch, const Player::IdView playerId, const std::string_view choice)
    -> task<>
{
    return sendToOne(ch, makeHowToPlay(playerId, choice));
}

inline auto sendWhisting(const Player::IdView playerId, const std::string_view choice) -> task<>
{
    return sendToAll(makeWhisting(playerId, choice));
}

inline auto sendHowToPlay(const Player::IdView playerId, const std::string_view choice) -> task<>
{
    return sendToAll(makeHowToPlay(playerId, choice));
}

inline auto sendOpenWhistPlayToOne(
    const ChannelPtr& ch, const Player::IdView activeWhisterId, const Player::IdView passiveWhisterId) -> task<>
{
    return sendToOne(ch, makeOpenWhistPlay(activeWhisterId, passiveWhisterId));
}

inline auto sendOpenWhistPlay(const Player::IdView activeWhisterId, const Player::IdView passiveWhisterId) -> task<>
{
    return sendToAll(makeOpenWhistPlay(activeWhisterId, passiveWhisterId));
}

inline auto sendOpenTalonToOne(const ChannelPtr& ch) -> task<>
{
    return sendToOne(ch, makeOpenTalon(ctx().talon.current));
}

inline auto sendOpenTalon() -> task<>
{
    assert(ctx().talon.open < std::size(ctx().talon.cards));
    ctx().talon.current = ctx().talon.cards[ctx().talon.open];
    return sendToAll(makeOpenTalon(ctx().talon.current));
}

// TODO: combine sendMiserCardsToOne() and sendMiserCards()
inline auto sendMiserCardsToOne(const ChannelPtr& ch) -> task<>
{
    const auto& declarer = getDeclarer();
    const auto& discardedCards = ctx().talon.discardedCards;
    auto played = declarer.playedCards
        | rv::remove_if([&](const CardNameView card) { return rng::contains(discardedCards, card); })
        | rng::to_vector;
    auto remaining = rv::concat(declarer.hand, std::empty(discardedCards) ? ctx().talon.cards : discardedCards)
        | rv::remove_if([&](const CardNameView card) { return rng::contains(played, card); })
        | rng::to_vector;
    return sendToOne(ch, makeMiserCards(std::move(remaining), std::move(played)));
}

inline auto sendMiserCards() -> task<>
{
    const auto& declarer = getDeclarer();
    const auto& discardedCards = ctx().talon.discardedCards;
    auto played = declarer.playedCards
        | rv::remove_if([&](const CardNameView card) { return rng::contains(discardedCards, card); })
        | rng::to_vector;
    auto remaining = rv::concat(declarer.hand, std::empty(discardedCards) ? ctx().talon.cards : discardedCards)
        | rv::remove_if([&](const CardNameView card) { return rng::contains(played, card); })
        | rng::to_vector;
    return sendToAll(makeMiserCards(std::move(remaining), std::move(played)));
}

inline auto sendGameState(const ChannelPtr& ch) -> task<>
{
    const auto playersTakenTricks = players()
        | rv::transform([](const Player& player) { return std::pair{player.id, player.tricksTaken}; })
        | rng::to_vector;

    const auto cardsLeft = players()
        | rv::transform([](const Player& player) {
                               return std::pair{player.id, static_cast<int>(std::ssize(player.hand))};
                           })
        | rng::to_vector;
    co_await sendToOne(ch, makeGameState(ctx().lastTrick, playersTakenTricks, cardsLeft));
}

inline auto sendPlayedCards(const ChannelPtr& ch) -> task<>
{
    for (const auto& card : ctx().trick) { co_await sendToOne(ch, makePlayCard(card.playerId, card.name)); }
}

inline auto sendTrickFinished() -> task<>
{
    const auto playersTakenTricks = players()
        | rv::transform([](const Player& player) { return std::pair{player.id, player.tricksTaken}; })
        | rng::to_vector;
    return sendToAll(makeTrickFinished(playersTakenTricks));
}

inline auto sendDealFinished(
    const std::map<PlayerId, std::int32_t>& lastDealMmr,
    const ScoreSheet& lastDealScoreSheet,
    const bool isGameOver) -> task<>
{
    return sendToAll(makeDealFinished(ctx().scoreSheet, lastDealMmr, lastDealScoreSheet, isGameOver));
}

inline auto sendPingPong(const Message& msg, const ChannelPtr& ch) -> task<>
{
    co_await sendToOne(ch, msg.SerializeAsString());
}

inline auto sendUserGames(const Player& player) -> task<>
{
    co_await sendToOne(player.conn.ch, makeUserGames(ctx().gameData, player.id));
}

inline auto sendUserGames() -> task<>
{
    for (const auto& player : players()) { co_await sendUserGames(player); }
}

[[nodiscard]] inline auto totalMmrByPlayerId(const Player::IdView playerId) -> std::int32_t
{
    return userByPlayerId(ctx().gameData, playerId)
        .transform([](const User& user) { return rng::accumulate(user.games(), 0, std::plus{}, &UserGame::mmr); })
        .value_or(0);
}

inline auto sendLadderToOne(const ChannelPtr& ch) -> task<>
{
    auto ladder = std::map<PlayerId, std::int32_t>{};
    for (const auto& player : players()) { ladder.emplace(player.id, totalMmrByPlayerId(player.id)); }
    co_await sendToOne(ch, makeLadder(ladder));
}

inline auto sendLadder() -> task<>
{
    auto ladder = std::map<PlayerId, std::int32_t>{};
    for (const auto& player : players()) { ladder.emplace(player.id, totalMmrByPlayerId(player.id)); }
    co_await sendToAll(makeLadder(ladder));
}

} // namespace pref
