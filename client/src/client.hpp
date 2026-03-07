// SPDX-License-Identifier: AGPL-3.0-only
// Copyright (c) 2025 Oleksandr Kozlov

#pragma once

#include "common/common.hpp"
#include "proto/pref.pb.h"

#include <emscripten/em_types.h>
#include <emscripten/html5.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

namespace pref {

inline constexpr auto VirtualW = 2560.f;
inline constexpr auto VirtualH = 1440.f;

inline constexpr auto OriginalCardHeight = 726.f;
inline constexpr auto OriginalCardWidth = 500.f;
inline constexpr auto CardAspectRatio = OriginalCardWidth / OriginalCardHeight;
inline constexpr auto CardHeight = VirtualH / 5.f;
inline constexpr auto CardWidth = CardHeight * CardAspectRatio;
inline constexpr auto CardOverlapX = CardWidth * 0.51f;
inline constexpr auto CardOverlapY = CardHeight * 0.24f;
inline constexpr auto CardBorderMargin = VirtualW / 30.f;
inline constexpr auto MyCardBorderMarginY = CardHeight / 10.8f;
inline constexpr auto CardInnerMargin = VirtualW / 100.f;
inline constexpr auto FontSpacing = 1.f;
inline constexpr auto BorderMargin = VirtualW / 52.f;
inline constexpr auto ScrollBarWidth = static_cast<int>(VirtualW / 106.f);
inline constexpr auto MenuX = VirtualW - CardBorderMargin - CardWidth - CardInnerMargin;

inline constexpr auto SettingsIcon = "";
inline constexpr auto ScoreSheetIcon = "";
inline constexpr auto EnterFullScreenIcon = "";
inline constexpr auto ExitFullScreenIcon = "";
inline constexpr auto SpeechBubbleIcon = "";
inline constexpr auto OverallScoreboardIcon = "";
inline constexpr auto LadderIcon = "";
inline constexpr auto StatIcon = "";
inline constexpr auto LogoutIcon = "";
inline constexpr auto HandshakeIcon = "";
inline constexpr auto MicOnIcon = "";
inline constexpr auto MicOffIcon = "";
inline constexpr auto VoiceOnIcon = "";
inline constexpr auto VoiceOffIcon = "";
inline constexpr auto KeyboardIcon = "";
inline constexpr int LeftArrowIcon = 0xF112;
inline constexpr int RightArrowIcon = 0xF064;
inline constexpr int DownArrowIcon = 0xF3BE;

inline constexpr auto PREF_SEVEN_OF_SPADES_CARD = 0x1F0A7;
inline constexpr auto PREF_SEVEN_OF_CLUBS_CARD = 0x1F0D7;
inline constexpr auto PREF_SEVEN_OF_DIAMONDS_CARD = 0x1F0C7;
inline constexpr auto PREF_SEVEN_OF_HEARTS_CARD = 0x1F0B7;
inline constexpr auto PREF_EIGHT_OF_SPADES_CARD = 0x1F0A8;
inline constexpr auto PREF_EIGHT_OF_CLUBS_CARD = 0x1F0D8;
inline constexpr auto PREF_EIGHT_OF_DIAMONDS_CARD = 0x01F0C8;
inline constexpr auto PREF_EIGHT_OF_HEARTS_CARD = 0x1F0B8;
inline constexpr auto PREF_NINE_OF_SPADES_CARD = 0x1F0A9;
inline constexpr auto PREF_NINE_OF_CLUBS_CARD = 0x1F0D9;
inline constexpr auto PREF_NINE_OF_DIAMONDS_CARD = 0x1F0C9;
inline constexpr auto PREF_NINE_OF_HEARTS_CARD = 0x1F0B9;
inline constexpr auto PREF_TEN_OF_SPADES_CARD = 0x1F0AA;
inline constexpr auto PREF_TEN_OF_CLUBS_CARD = 0x1F0DA;
inline constexpr auto PREF_TEN_OF_DIAMONDS_CARD = 0x1F0CA;
inline constexpr auto PREF_TEN_OF_HEARTS_CARD = 0x1F0BA;
inline constexpr auto PREF_JACK_OF_SPADES_CARD = 0x1F0AB;
inline constexpr auto PREF_JACK_OF_CLUBS_CARD = 0x1F0DB;
inline constexpr auto PREF_JACK_OF_DIAMONDS_CARD = 0x01F0CB;
inline constexpr auto PREF_JACK_OF_HEARTS_CARD = 0x1F0BB;
inline constexpr auto PREF_QUEEN_OF_SPADES_CARD = 0x1F0AD;
inline constexpr auto PREF_QUEEN_OF_CLUBS_CARD = 0x1F0DD;
inline constexpr auto PREF_QUEEN_OF_DIAMONDS_CARD = 0x1F0CD;
inline constexpr auto PREF_QUEEN_OF_HEARTS_CARD = 0x1F0BD;
inline constexpr auto PREF_KING_OF_SPADES_CARD = 0x1F0AE;
inline constexpr auto PREF_KING_OF_CLUBS_CARD = 0x1F0DE;
inline constexpr auto PREF_KING_OF_DIAMONDS_CARD = 0x1F0CE;
inline constexpr auto PREF_KING_OF_HEARTS_CARD = 0x1F0BE;
inline constexpr auto PREF_ACE_OF_SPADES_CARD = 0x1F0A1;
inline constexpr auto PREF_ACE_OF_CLUBS_CARD = 0x1F0D1;
inline constexpr auto PREF_ACE_OF_DIAMONDS_CARD = 0x1F0C1;
inline constexpr auto PREF_ACE_OF_HEARTS_CARD = 0x1F0B1;

#define PREF_SEVEN_OF_SPADES PREF_SEVEN PREF_OF_ PREF_SPADES
#define PREF_SEVEN_OF_CLUBS PREF_SEVEN PREF_OF_ PREF_CLUBS
#define PREF_SEVEN_OF_DIAMONDS PREF_SEVEN PREF_OF_ PREF_DIAMONDS
#define PREF_SEVEN_OF_HEARTS PREF_SEVEN PREF_OF_ PREF_HEARTS
#define PREF_EIGHT_OF_SPADES PREF_EIGHT PREF_OF_ PREF_SPADES
#define PREF_EIGHT_OF_CLUBS PREF_EIGHT PREF_OF_ PREF_CLUBS
#define PREF_EIGHT_OF_DIAMONDS PREF_EIGHT PREF_OF_ PREF_DIAMONDS
#define PREF_EIGHT_OF_HEARTS PREF_EIGHT PREF_OF_ PREF_HEARTS
#define PREF_NINE_OF_SPADES PREF_NINE PREF_OF_ PREF_SPADES
#define PREF_NINE_OF_CLUBS PREF_NINE PREF_OF_ PREF_CLUBS
#define PREF_NINE_OF_DIAMONDS PREF_NINE PREF_OF_ PREF_DIAMONDS
#define PREF_NINE_OF_HEARTS PREF_NINE PREF_OF_ PREF_HEARTS
#define PREF_TEN_OF_SPADES PREF_TEN PREF_OF_ PREF_SPADES
#define PREF_TEN_OF_DIAMONDS PREF_TEN PREF_OF_ PREF_DIAMONDS
#define PREF_TEN_OF_HEARTS PREF_TEN PREF_OF_ PREF_HEARTS
#define PREF_TEN_OF_CLUBS PREF_TEN PREF_OF_ PREF_CLUBS
#define PREF_JACK_OF_SPADES PREF_JACK PREF_OF_ PREF_SPADES
#define PREF_JACK_OF_CLUBS PREF_JACK PREF_OF_ PREF_CLUBS
#define PREF_JACK_OF_DIAMONDS PREF_JACK PREF_OF_ PREF_DIAMONDS
#define PREF_JACK_OF_HEARTS PREF_JACK PREF_OF_ PREF_HEARTS
#define PREF_QUEEN_OF_SPADES PREF_QUEEN PREF_OF_ PREF_SPADES
#define PREF_QUEEN_OF_CLUBS PREF_QUEEN PREF_OF_ PREF_CLUBS
#define PREF_QUEEN_OF_DIAMONDS PREF_QUEEN PREF_OF_ PREF_DIAMONDS
#define PREF_QUEEN_OF_HEARTS PREF_QUEEN PREF_OF_ PREF_HEARTS
#define PREF_KING_OF_SPADES PREF_KING PREF_OF_ PREF_SPADES
#define PREF_KING_OF_CLUBS PREF_KING PREF_OF_ PREF_CLUBS
#define PREF_KING_OF_DIAMONDS PREF_KING PREF_OF_ PREF_DIAMONDS
#define PREF_KING_OF_HEARTS PREF_KING PREF_OF_ PREF_HEARTS
#define PREF_ACE_OF_SPADES PREF_ACE PREF_OF_ PREF_SPADES
#define PREF_ACE_OF_CLUBS PREF_ACE PREF_OF_ PREF_CLUBS
#define PREF_ACE_OF_DIAMONDS PREF_ACE PREF_OF_ PREF_DIAMONDS
#define PREF_ACE_OF_HEARTS PREF_ACE PREF_OF_ PREF_HEARTS

#define PREF_OK_CLICK 1
#define PREF_NO_CLICK -1

// clang-format off
#define PREF_CARDS \
    PREF_X(PREF_SEVEN_OF_SPADES) PREF_X(PREF_SEVEN_OF_CLUBS) PREF_X(PREF_SEVEN_OF_DIAMONDS) PREF_X(PREF_SEVEN_OF_HEARTS) \
    PREF_X(PREF_EIGHT_OF_SPADES) PREF_X(PREF_EIGHT_OF_CLUBS) PREF_X(PREF_EIGHT_OF_DIAMONDS) PREF_X(PREF_EIGHT_OF_HEARTS) \
    PREF_X(PREF_NINE_OF_SPADES) PREF_X(PREF_NINE_OF_CLUBS) PREF_X(PREF_NINE_OF_DIAMONDS) PREF_X(PREF_NINE_OF_HEARTS) \
    PREF_X(PREF_TEN_OF_SPADES) PREF_X(PREF_TEN_OF_CLUBS) PREF_X(PREF_TEN_OF_DIAMONDS) PREF_X(PREF_TEN_OF_HEARTS) \
    PREF_X(PREF_JACK_OF_SPADES) PREF_X(PREF_JACK_OF_CLUBS) PREF_X(PREF_JACK_OF_DIAMONDS) PREF_X(PREF_JACK_OF_HEARTS) \
    PREF_X(PREF_QUEEN_OF_SPADES) PREF_X(PREF_QUEEN_OF_CLUBS) PREF_X(PREF_QUEEN_OF_DIAMONDS) PREF_X(PREF_QUEEN_OF_HEARTS) \
    PREF_X(PREF_KING_OF_SPADES) PREF_X(PREF_KING_OF_CLUBS) PREF_X(PREF_KING_OF_DIAMONDS) PREF_X(PREF_KING_OF_HEARTS) \
    PREF_X(PREF_ACE_OF_SPADES) PREF_X(PREF_ACE_OF_CLUBS) PREF_X(PREF_ACE_OF_DIAMONDS) PREF_X(PREF_ACE_OF_HEARTS)
// clang-format on

#define PREF_NUMBER_01 "❶"
#define PREF_NUMBER_02 "❷"
#define PREF_NUMBER_03 "❸"
#define PREF_NUMBER_04 "❹"
#define PREF_NUMBER_05 "❺"
#define PREF_NUMBER_06 "❻"
#define PREF_NUMBER_07 "❼"
#define PREF_NUMBER_08 "❽"
#define PREF_NUMBER_09 "❾"
#define PREF_NUMBER_10 "❿"

#define PREF_MOUSE_FACE "\xF0\x9F\x90\xAD"
#define PREF_COW_FACE "\xF0\x9F\x90\xAE"
#define PREF_CAT_FACE "\xF0\x9F\x90\xB1"
#define PREF_MONKEY_FACE "\xF0\x9F\x90\xB5"

[[nodiscard]] constexpr auto cardNameCodepoint(const CardNameView cardName) noexcept -> int
{
#define PREF_X(PREF_CARD_NAME)                                                                                         \
    if (cardName == (PREF_CARD_NAME)) { return PREF_CARD_NAME##_CARD; }
    PREF_CARDS
#undef PREF_X
    PREF_DE(cardName);
    std::unreachable();
}

[[nodiscard]] constexpr auto prettifyNumber(const int tricksTaken) noexcept -> std::string_view
{
    switch (tricksTaken) {
    case 1: return PREF_NUMBER_01;
    case 2: return PREF_NUMBER_02;
    case 3: return PREF_NUMBER_03;
    case 4: return PREF_NUMBER_04;
    case 5: return PREF_NUMBER_05;
    case 6: return PREF_NUMBER_06;
    case 7: return PREF_NUMBER_07;
    case 8: return PREF_NUMBER_08;
    case 9: return PREF_NUMBER_09;
    case 10: return PREF_NUMBER_10;
    }
    std::unreachable();
}

inline constexpr const auto LocalStoragePrefix = "preferans_";

// ♠ - Spades | ♣ - Clubs | ♦ - Diamonds | ♥ - Hearts
// clang-format off
inline constexpr auto BidsRank = std::array{
     PREF_SIX PREF_SPADE,   PREF_SIX PREF_CLUB,   PREF_SIX PREF_DIAMOND,   PREF_SIX PREF_HEART,            PREF_SIX,
   PREF_SEVEN PREF_SPADE, PREF_SEVEN PREF_CLUB, PREF_SEVEN PREF_DIAMOND, PREF_SEVEN PREF_HEART,          PREF_SEVEN,
   PREF_EIGHT PREF_SPADE, PREF_EIGHT PREF_CLUB, PREF_EIGHT PREF_DIAMOND, PREF_EIGHT PREF_HEART,          PREF_EIGHT, PREF_MISER,
    PREF_NINE PREF_SPADE,  PREF_NINE PREF_CLUB,  PREF_NINE PREF_DIAMOND,  PREF_NINE PREF_HEART,           PREF_NINE, PREF_MISER_WT,
            PREF_NINE_WT,  PREF_TEN PREF_SPADE,      PREF_TEN PREF_CLUB, PREF_TEN PREF_DIAMOND, PREF_TEN PREF_HEART, PREF_TEN, PREF_PASS};

inline constexpr auto BidTable = std::array<std::array<std::string_view, 7>, 6>{
{{           "",   PREF_SIX PREF_SPADE,   PREF_SIX PREF_CLUB,   PREF_SIX PREF_DIAMOND,   PREF_SIX PREF_HEART,   PREF_SIX, ""},
 {           "", PREF_SEVEN PREF_SPADE, PREF_SEVEN PREF_CLUB, PREF_SEVEN PREF_DIAMOND, PREF_SEVEN PREF_HEART, PREF_SEVEN, ""},
 {           "", PREF_EIGHT PREF_SPADE, PREF_EIGHT PREF_CLUB, PREF_EIGHT PREF_DIAMOND, PREF_EIGHT PREF_HEART, PREF_EIGHT, ""},
 {   PREF_MISER,  PREF_NINE PREF_SPADE,  PREF_NINE PREF_CLUB,  PREF_NINE PREF_DIAMOND,  PREF_NINE PREF_HEART,  PREF_NINE, PREF_MISER_WT},
 { PREF_NINE_WT,   PREF_TEN PREF_SPADE,   PREF_TEN PREF_CLUB,   PREF_TEN PREF_DIAMOND,   PREF_TEN PREF_HEART,   PREF_TEN, PREF_PASS}}};
// clang-format on

inline constexpr auto AllRanks = std::size(BidsRank);

inline constexpr auto BidCellW = VirtualW / 13.f;
inline constexpr auto BidCellH = BidCellW / 2.f;
inline constexpr auto BidGap = BidCellH / 10.f;
inline constexpr auto BidRows = static_cast<int>(std::size(BidTable));
inline constexpr auto BidCols = static_cast<int>(std::size(BidTable[0]));
inline constexpr auto BidMenuW = BidCols * BidCellW + (BidCols - 1) * BidGap;
inline constexpr auto BidMenuH = BidRows * BidCellH + (BidRows - 1) * BidGap;
inline constexpr auto BidOriginX = (VirtualW - BidMenuW) / 2.f;
inline constexpr auto BidOriginY = (VirtualH - BidMenuH) / 2.f - VirtualH / 10.8f;

enum class GameLang : std::size_t {
    English,
    Ukrainian,
    Alternative,
    Count,
};

// clang-format off
#define PREF_GAME_TEXT                                                                                                 \
    PREF_X(ACCEPT, "ACCEPT", "ПРИЙНЯТИ", "ПРИНЯТЬ")                                                                    \
    PREF_X(Agreements, "AGREEMENTS", "ДОМОВЛЕНОСТІ", "СОГЛАШЕНИЯ")                                                     \
    PREF_X(Alternative, "Alternative", "Альтернативна", "Альтернативный")                                              \
    PREF_X(Amber, "Amber", "Бурштин", "Янтарь")                                                                        \
    PREF_X(Arithmetic123, "Arithmetic (1, 2, 3)", "Арифметична (1, 2, 3)", "Арифметическая (1, 2, 3)")                 \
    PREF_X(Bluish, "Bluish", "Блакитний", "Голубоватый")                                                               \
    PREF_X(Catch, PREF_CATCH, "Ловлю", "Ловлю")                                                                        \
    PREF_X(Check, PREF_CHECK, "Перевіряю", "Проверяю")                                                                 \
    PREF_X(Checked, "Checked", "Перевіряється", "Проверяется")                                                         \
    PREF_X(Closed, "Closed", "У темну", "Втёмную")                                                                     \
    PREF_X(ColorScheme, "Color scheme", "Кольорова схема", "Цветовая схема")                                           \
    PREF_X(ConfirmTitle, "CONFIRM", "ПІДТВЕРДИТИ", "ПОДТВЕРДИТЬ")                                                      \
    PREF_X(Consolation, "Consolation", "Консоляція", "Консоляция")                                                     \
    PREF_X(CurrentPlayers, "Current players:", "Поточні гравці:", "Текущие игроки:")                                   \
    PREF_X(Cyber, "Cyber", "Кібер", "Кибер")                                                                           \
    PREF_X(DECLINE, "DECLINE", "ВІДХИЛИТИ", "ОТКЛОНИТЬ")                                                               \
    PREF_X(DPW, "D/P/W", "Г/П/В", "Г/П/В")                                                                             \
    PREF_X(Dark, "Dark", "Темний", "Тёмный")                                                                           \
    PREF_X(Date, "DATE", "ДАТА", "ДАТА")                                                                               \
    PREF_X(DiscardSelectedCards, "Discard selected cards?", "Скинути вибрані карти?", "Сбросить выбранные карты?")     \
    PREF_X(DownThree, "Down three", "Без трьох", "Без трёх")                                                           \
    PREF_X(Dracula, "Dracula", "Дракула", "Дракула")                                                                   \
    PREF_X(Draw, "Draw", "Нічия", "Ничья")                                                                             \
    PREF_X(Duration, "DURATION", "ТРИВАЛІСТЬ", "ДЛИТЕЛЬНОСТЬ")                                                         \
    PREF_X(EndThePassing, "End the passing", "Вихід із розпасів", "Выход из распасов")                                 \
    PREF_X(English, "English", "Англійська", "Английский")                                                             \
    PREF_X(Enter, "Enter", "Увійти", "Войти")                                                                          \
    PREF_X(GameOver, "GAME OVER", "КІНЕЦЬ ГРИ", "КОНЕЦ ИГРЫ")                                                          \
    PREF_X(Games, "GAMES", "К-ТЬ ІГОР", "К-ВО ИГР")                                                                    \
    PREF_X(GamesWithoutTalon, "Games without talon", "Ігри без прикупу", "Игры без прикупа")                           \
    PREF_X(Genesis, "Genesis", "Генезис", "Генезис")                                                                   \
    PREF_X(GoDownThreeTricks, "Go down three tricks?", "Піти без трьох взяток?", "Уйти без трёх взяток?")              \
    PREF_X(HalfWhist, PREF_HALF_WHIST, "Піввіста", "Полвиста")                                                         \
    PREF_X(Jungle, "Jungle", "Джунглі", "Джунгли")                                                                     \
    PREF_X(Ladder, "LADDER", "ЛІДЕРИ", "ЛИДЕРЫ")                                                                       \
    PREF_X(Language, "Language", "Мова", "Язык")                                                                       \
    PREF_X(Lavanda, "Lavanda", "Лаванда", "Лаванда")                                                                   \
    PREF_X(LogOut, "LOG OUT", "ВИХІД", "ВЫХОД")                                                                        \
    PREF_X(LogOutOfTheAccount, "Leave the game and log\n\nout of your account?","Покинути гру та вийти\n\nз облікового запису?", "Покинуть игру и выйти\n\nиз учётной записи?")    \
    PREF_X(Login, "LOG IN", "ЛОГІН", "ЛОГИН")                                                                          \
    PREF_X(Lost, "Lost", "Поразка", "Поражение")                                                                       \
    PREF_X(MMR, "MMR", "РЕЙТИНГ", "РЕЙТИНГ")                                                                           \
    PREF_X(Miser, PREF_MISER, "Мізéр", "Мизéр")                                                                        \
    PREF_X(MisereCanBeOutbidBy, "Misère can be outbid by", "Мізер перебивається", "Мизер перебивается")                \
    PREF_X(MisereWithoutTalon, "Misère without talon —", "Мізер без прикупу —", "Мизер без прикупа —")                 \
    PREF_X(Nine, "9", "9", "9")                                                                                        \
    PREF_X(NineWTAnd10, "9WT and 10", "9БП и 10", "9БП и 10")                                                          \
    PREF_X(No, "No", "Ні", "Нет")                                                                                      \
    PREF_X(NoMoreForMe, "No more for me", "Більше не беру", "Моих больше нет")                                         \
    PREF_X(None, "", "", "")                                                                                           \
    PREF_X(Normal, "Normal", "Звичайна", "Обычная")                                                                    \
    PREF_X(Offer, "Offer", "Запропонувати", "Предложить")                                                              \
    PREF_X(Ok, "OK", "OK", "OK")                                                                                       \
    PREF_X(Openly, PREF_OPENLY, "У світлу", "В светлую")                                                               \
    PREF_X(Other, "Other", "Інше", "Другое")                                                                           \
    PREF_X(OverallScoreboard, "SCOREBOARD", "ТАБЛИЦЯ РЕЗУЛЬТАТІВ", "ТАБЛИЦА РЕЗУЛЬТАТОВ")                              \
    PREF_X(PLAY, "PLAY", "ГРАТИ", "ИГРАТЬ")                                                                            \
    PREF_X(Pass, PREF_PASS, "Пас", "Пас")                                                                              \
    PREF_X(PassHalfWhistWhist, "Pass, half-whist, whist", "Пас, піввіста, віст", "Пас, полвиста, вист")                \
    PREF_X(Passing, "Passing", "Розпаси", "Распасы")                                                                   \
    PREF_X(PassingProgression, "Passing progression", "Прогресія розпасів", "Прогрессия распасов")                     \
    PREF_X(Phrases, "PHRASES", "ФРАЗИ", "ФРАЗЫ")                                                                       \
    PREF_X(PlayerLeftTheGame, "left the game.", "покинув(а) гру.", "покинул(ла) игру.")                                       \
    PREF_X(PoolLength, "Pool length", "Довжина пульки", "Длина пульки")                                                \
    PREF_X(Preferans, "PREFERANS", "ПРЕФЕРАНС", "ПРЕФЕРАНС")                                                           \
    PREF_X(Ranked, "Ranked", "Рейтингова", "Рейтинговая")                                                              \
    PREF_X(RemainingMine, "Remaining — mine", "Решта — мої", "Остальные — мои")                                        \
    PREF_X(ResponsibleGreedy, "Responsible/Greedy", "Відповідальний/Жлобський", "Ответственный/Жлобский")              \
    PREF_X(Result, "RESULT", "РЕЗУЛЬТАТ", "РЕЗУЛЬТАТ")                                                                 \
    PREF_X(RevealCardsAndOffer, "Reveal cards and offer", "Відкрити карти та запропонувати", "Открыть карты и предложить") \
    PREF_X(Reveals, "Reveals", "Відкривається", "Открывается")                                                         \
    PREF_X(Send, "Send", "Надіслати", "Отправить")                                                                     \
    PREF_X(Settings, "SETTINGS", "НАЛАШТУВАННЯ", "НАСТРОЙКИ")                                                          \
    PREF_X(SevenPlayed, "7 (played)", "7 (зіграною)", "7 (сыгранной)")                                                 \
    PREF_X(ShowFps, "Show Ping & FPS", "Показувати пінг та част. кадрів", "Показывать пинг и част. кадров")            \
    PREF_X(SlidingPassing, "Sliding passing", "Ковзні розпаси", "Скользящие распасы")                                  \
    PREF_X(Sochi, "Sochi", "Сочинка", "Сочинка")                                                                       \
    PREF_X(SoundEffects, "Sound effects", "Звукові ефекти", "Звуковые эффекты")                                        \
    PREF_X(Stalingrad, "Stalingrad", "Сталінград", "Сталинград")                                                       \
    PREF_X(TalonBonus, "Talon bonus", "Премія за прикуп", "Премия за прикуп")                                          \
    PREF_X(TalonOnPassing, "Talon on passing", "Прикуп на розпасах", "Прикуп на распасах")                             \
    PREF_X(Ten, "10", "10", "10")                                                                                      \
    PREF_X(TenGame, "Ten game", "Десятерна гра", "Десятерная игра")                                                    \
    PREF_X(Time, "TIME", "ЧАС", "ВРЕМЯ")                                                                               \
    PREF_X(TooltipVariant, "Sochi is treated as a game against the whister,\n"                                         \
                           "because whisting is difficult in this variant.\n"                                          \
                           "If the whister fails to meet their obligation,\n"                                          \
                           "they are penalized for every missing trick\n"                                              \
                           "at the value of the declared game.",                                                       \
                           "Сочинка вважається грою проти вістуючого,\n"                                               \
                           "бо у цьому варіанті вістувати складно.\n"                                                  \
                           "Якщо вістуючий не виконав зобов'язання,\n"                                                 \
                           "його штрафують за кожну недобрану взятку\n"                                                \
                           "за вартістю оголошеної гри.",                                                              \
                           "Сочинка считается игрой против вистующего,\n"                                              \
                           "поскольку вистование в ней затруднено. Если\n"                                             \
                           "вистующий не выполнил своих обязательств, его\n"                                           \
                           "штрафуют за каждую недобранную взятку исходя из\n"                                         \
                           "стоимости объявленной игры.")                                                              \
    PREF_X(TooltipPoolLength, "The game ends when each player scores at least\n"                                       \
                              "10 points in the pool.",                                                                \
                              "Гра закінчується, коли кожен із гравців\n"                                              \
                              "набирає щонайменше 10 очок у пульці.",                                                  \
                              "Игра заканчивается, когда каждый из игроков\n"                                          \
                              "набирает не менее 10 очков в пульке.")                                                  \
    PREF_X(TooltipStalingrad, "On 6♠, whist is mandatory.",                                                            \
                              "При грі 6♠ віст є обов'язковим.",                                                       \
                              "При игре 6♠ вист является обязательным.")                                               \
    PREF_X(TooltipTenGame, "Ten-game is checked — the whisters can verify,\n"                                          \
                           "without risk, whether there is a card layout or\n"                                         \
                           "line of play that can set the declarer.",                                                  \
                           "Десятерна перевіряється — тобто вістуючі,\n"                                               \
                           "нічим не ризикуючи, перевіряють, чи є розклад\n"                                           \
                           "або план розіграшу, що дозволяє посадити\n"                                                \
                           "розігрувача.",                                                                             \
                           "Десятерная проверяется — то есть вистующие,\n"                                             \
                           "ничем не рискуя, проверяют, существует ли расклад\n"                                       \
                           "или план розыгрыша, позволяющий обремизить\n"                                              \
                           "разыгрывающего.")                                                                          \
    PREF_X(TooltipWhist, "Responsible whist — if whisters take fewer\n"                                                \
                         "tricks than required, they are penalized in full:\n"                                         \
                         "the game value for each missing trick.\n"                                                    \
                         "\n"                                                                                          \
                         "Greedy whist — if declarer goes down, whists are\n"                                          \
                         "recorded only for whisters. If one player passes,\n"                                         \
                         "the passer records only for no-trick. If two\n"                                              \
                         "players whist, each records whists and penalties\n"                                          \
                         "only for their own hand.",                                                                   \
                         "Відповідальний віст — недобір вістуючими взяток\n"                                           \
                         "до обов'язкової кількості карається ремізом у\n"                                             \
                         "повному обсязі, тобто за вартістю гри за кожну\n"                                            \
                         "недобрану взятку.\n"                                                                         \
                         "\n"                                                                                          \
                         "Жлобський віст — у разі ремізу розігрувача вісти\n"                                          \
                         "записують лише вістуючі. Якщо один гравець\n"                                                \
                         "сказав пас, пасуючий пише тільки за безвзяття.\n"                                            \
                         "Якщо вістують двоє, кожен записує вісти й\n"                                                 \
                         "ремізи лише за себе.",                                                                       \
                         "Ответственный вист — недобор вистующими взяток до\n"                                         \
                         "обязательного их количества наказывается ремизом в\n"                                        \
                         "полном объёме, то есть в размере стоимости игры за\n"                                        \
                         "каждую недобранную взятку.\n"                                                                \
                         "\n"                                                                                          \
                         "Жлобский вист — в случае ремиза разыгрывающего\n"                                            \
                         "висты записываются только вистующим. Если один из\n"                                         \
                         "игроков объявил пас, то пасующий записывает только\n"                                        \
                         "за безвзятие. Если вистуют двое, каждый из них\n"                                            \
                         "записывает висты и ремизы только за себя.")                                                  \
    PREF_X(TooltipPassHalfWhistWhist, "If the player after declarer says pass, the\n"                                  \
                                      "second whister may go half-whist: treat the\n"                                  \
                                      "contract as made and record half of required\n"                                 \
                                      "tricks without play. After half-whist is called,\n"                             \
                                      "the partner who passed may return to whist.\n"                                  \
                                      "For a first-hand passer, this right exists only\n"                              \
                                      "on six- and seven-games, because on eight-game\n"                               \
                                      "the required number of tricks is one. Once whist\n"                             \
                                      "is returned, the half-whist player can no longer\n"                             \
                                      "whist; their only possible call is pass.",                                      \
                                      "Якщо гравець після розігрувача сказав пас,\n"                                   \
                                      "другий вістуючий може піти за піввіста - тобто\n"                               \
                                      "визнати контракт зіграним і без розіграшу\n"                                    \
                                      "записати половину обов'язкових взяток. Після\n"                                 \
                                      "заявки піввіста партнер, що пасував, має право\n"                               \
                                      "повернути віст (завістувати). Для гравця, що\n"                                 \
                                      "пасував на першій руці, це право діє лише на\n"                                 \
                                      "шестерних і семерних іграх, бо на восьмерній\n"                                 \
                                      "обов'язкова кількість взяток дорівнює одній.\n"                                 \
                                      "Після повернення віста гравець, який заявив\n"                                  \
                                      "піввіста, втрачає право вістувати; його\n"                                      \
                                      "єдина можлива заявка - пас.",                                                   \
                                      "Если игрок, сидящий следующим за разыгрывающим,\n"                              \
                                      "объявил пас, второй вистующий может уйти за\n"                                  \
                                      "полвиста — то есть признать контракт сыгранным и\n"                             \
                                      "без розыгрыша записать половину обязательного\n"                                \
                                      "количества взяток. После объявления полвиста\n"                                 \
                                      "спасовавший партнёр имеет право вернуть вист\n"                                 \
                                      "(завистовать). Право вернуть вист у игрока,\n"                                  \
                                      "спасовавшего на первой руке, существует только на\n"                            \
                                      "шестерной и семерной играх, поскольку на\n"                                     \
                                      "восьмерной обязательное количество взяток равно\n"                              \
                                      "одной. После возврата виста игрок, ранее\n"                                     \
                                      "объявивший полвиста, теряет право вистовать; его\n"                             \
                                      "единственная возможная заявка — пас.")                                          \
    PREF_X(TooltipWhistPassHalfWhist, "The sequence \"whist, pass, half-whist\" is not\n"                              \
                                      "allowed, because this variant does not use the\n"                               \
                                      "gentleman-whist rules, where open whist is\n"                                   \
                                      "encouraged and first-hand pass is considered good.\n"                           \
                                      "If one player says whist and the other says pass,\n"                            \
                                      "the first player may not switch to half-whist\n"                                \
                                      "after the partner passes.",                                                     \
                                      "Комбінація \"віст, пас, піввіста\" не дозволена,\n"                             \
                                      "бо гра йде без правил джентльменського віста,\n"                                \
                                      "де заохочується гра у світлу, а пас на першій\n"                                \
                                      "руці вважається корисним. Якщо один гравець\n"                                  \
                                      "сказав віст, а інший - пас, перший не може\n"                                   \
                                      "після пасу партнера піти за піввіста.",                                         \
                                      "Комбинация \"вист, пас, полвиста\" не допускается,\n"                           \
                                      "так как игра ведётся без применения правил\n"                                   \
                                      "джентльменского виста, при которых поощряется игра\n"                           \
                                      "в светлую и считается полезным говорить пас на\n"                               \
                                      "первой руке. Если один игрок объявил вист, а\n"                                 \
                                      "другой — пас, первый не может после паса партнёра\n"                            \
                                      "уйти за полвиста.")                                                             \
    PREF_X(TooltipConsolation, "Consolation — a bonus for setting the declarer:\n"                                     \
                               "additional penalty points awarded on top of the\n"                                     \
                               "normal loss. When the declarer goes down, the\n"                                       \
                               "consolation is awarded for each trick short of the\n"                                  \
                               "contract, in the amount equal to the value of one\n"                                   \
                               "trick. For example, if a six-trick contract goes\n"                                    \
                               "down one, each player records the tricks actually\n"                                   \
                               "taken and additionally one more for the\n"                                             \
                               "undertrick. In other words, the whisters score\n"                                      \
                               "whist points for the tricks they actually take,\n"                                     \
                               "and consolation for the actual undertricks,\n"                                         \
                               "without counting the penalty tricks. Consolation\n"                                    \
                               "is recorded by all players, regardless of whether\n"                                   \
                               "they whisted or passed.",                                                              \
                               "Консоляція - це премія за підсад розігрувача:\n"                                       \
                               "додаткові штрафні очки понад звичайного програшу.\n"                                   \
                               "Коли розігрувач іде в реміз, консоляція\n"                                             \
                               "нараховується за кожну недобрану ним взятку\n"                                         \
                               "за вартістю взятки. Наприклад, на шестерній\n"                                         \
                               "без однієї кожен пише за фактично взяті взятки\n"                                      \
                               "і додатково одну - за безвзяття. Тобто\n"                                              \
                               "вістуючі пишуть вісти за взяті взятки, а\n"                                            \
                               "консоляцію - за фактичний підсад, без рахунку\n"                                       \
                               "штрафних взяток. Консоляцію записують усі\n"                                           \
                               "гравці, незалежно від того, вістували чи ні.",                                         \
                               "Консоляция — премия за подсад разыгрывающего,\n"                                       \
                               "дополнительные штрафные очки, начисляемые сверх\n"                                     \
                               "обычного проигрыша. При ремизе разыгрывающего\n"                                       \
                               "консоляция начисляется за каждую недобранную им\n"                                     \
                               "взятку в размере стоимости взятки. Например, при\n"                                    \
                               "подсаде на шестерной без одной каждый пишет за\n"                                      \
                               "фактически взятые взятки и дополнительно одну —\n"                                     \
                               "за безвзятие. Другими словами, вистующие пишут\n"                                      \
                               "висты за фактически взятые взятки, а консоляцию —\n"                                   \
                               "за фактический подсад, без учёта штрафных взяток.\n"                                   \
                               "Консоляцию записывают все игроки, вне зависимости\n"                                   \
                               "от того, вистовали они или нет.")                                                      \
    PREF_X(TooltipDownThree, "A player who has won the bidding may, after taking\n"                                    \
                             "the talon, \"go down three\" without playing the\n"                                      \
                             "hand. In this case, they record \"down three\" in\n"                                     \
                             "the dump at the level of the contract they bid,\n"                                       \
                             "while the whisters record nothing. Sometimes going\n"                                    \
                             "down three is better than, for example, entering a\n"                                    \
                             "passing round.",                                                                         \
                             "Після взяття прикупу розігрувач  може\n"                                                 \
                             "\"піти без трьох\" без розіграшу. У такому разі\n"                                       \
                             "він записує в гору \"без трьох\" на тій грі,\n"                                          \
                             "до якої доторгувався, а вістуючі не пишуть\n"                                            \
                             "нічого. Іноді піти без трьох вигідніше, ніж,\n"                                          \
                             "наприклад, грати розпаси.",                                                              \
                             "Заторговавшийся игрок может после взятия прикупа\n"                                      \
                             "\"уйти без трёх\" без розыгрыша. В этом случае он\n"                                     \
                             "записывает в гору \"без трёх\" на той игре, до\n"                                        \
                             "которой доторговался, а вистующие ничего не пишут.\n"                                    \
                             "Иногда выгоднее уйти без трёх, чем, например,\n"                                         \
                             "играть распасовку.")                                                                     \
    PREF_X(TooltipTalonBonus, "No one should be punished for not knowing how to\n"                                     \
                              "put aces into the talon, nor should that skill be\n"                                    \
                              "rewarded.",                                                                             \
                              "Ніхто не має страждати через невміння\n"                                                \
                              "закладати тузи у прикуп, і саме це вміння\n"                                            \
                              "так само не має винагороджуватися.",                                                    \
                              "Никто не должен страдать из-за неумения класть в\n"                                     \
                              "прикуп тузы, и умение класть в прикуп тузы не\n"                                        \
                              "должно вознаграждаться.")                                                               \
    PREF_X(TooltipTalonOnPassing, "In passing, the talon is turned up and sets the\n"                                  \
                                  "suit for the opening lead (card rank is ignored).",                                 \
                                  "При розпасах прикуп відкривається і показує масть\n"                                \
                                  "першого ходу (номінал карти не враховується).",                                     \
                                  "На распасах прикуп открывается и показывает масть\n"                                \
                                  "первого хода (номинал карты не учитывается).")                                      \
    PREF_X(TooltipSlidingPassing, "Sliding passing means that after a passing\n"                                       \
                                  "round, the deal moves to the next player\n"                                         \
                                  "clockwise instead of staying with the\n"                                            \
                                  "previous dealer.",                                                                  \
                                  "Ковзні розпаси — після розпасів здача\n"                                            \
                                  "переходить до наступного гравця по колу\n"                                          \
                                  "(за годинниковою стрілкою), а не лишається\n"                                       \
                                  "за попереднім здавачем.",                                                           \
                                  "Скользящая распасовка — после распасовки сдача\n"                                   \
                                  "переходит к следующему игроку по кругу (по часовой\n"                               \
                                  "стрелке), а не остаётся за предыдущим сдающим.")                                    \
    PREF_X(TooltipPassingProgression, "If passing happens several times in a row,\n"                                   \
                                      "the value of one trick rises each round.\n"                                     \
                                      "The growth is arithmetic, not geometric:\n"                                     \
                                      "each next passing adds one point per trick.\n"                                  \
                                      "So a trick is worth 1 on first passing,\n"                                      \
                                      "2 on second, and 3 on third.",                                                  \
                                      "Якщо розпаси відбуваються кілька разів поспіль,\n"                              \
                                      "вартість однієї взятки зростає від розпасів\n"                                  \
                                      "до розпасів. Зростання йде в арифметичній,\n"                                   \
                                      "а не в геометричній прогресії: кожні\n"                                         \
                                      "наступні розпаси додають по одному очку\n"                                      \
                                      "за взятку. Тобто на перших розпасах за\n"                                       \
                                      "взятку пишуть 1, на других - 2, на третіх - 3.",                                \
                                      "Если распасовка происходит несколько раз подряд,\n"                             \
                                      "стоимость одной взятки увеличивается от распасовки\n"                           \
                                      "к распасовке. Увеличение происходит в\n"                                        \
                                      "арифметической (в отличие от геометрической)\n"                                 \
                                      "прогрессии — цена каждой следующей распасовки\n"                                \
                                      "возрастает на одно очко. Таким образом, за взятку\n"                            \
                                      "на первой распасовке пишут в гору по одному очку,\n"                            \
                                      "на второй — по два, на третьей — по три.")                                      \
    PREF_X(TooltipEndThePassing, "End the passing (single or otherwise) is allowed\n"                                  \
                                 "only with a played game of seven or higher.",                                        \
                                 "Вийти з розпасів (одинарних чи будь-яких інших)\n"                                   \
                                 "можна тільки зіграною грою не нижче семерної.",                                      \
                                 "Выход из распасовки (как одинарной, так и любой\n"                                   \
                                 "другой) допускается только сыгранной игрой не\n"                                     \
                                 "ниже семерной.")                                                                     \
    PREF_X(TooltipGamesWithoutTalon, "Misère can be declared without talon to\n"                                       \
                                     "outbid nine-game, and nine without talon\n"                                      \
                                     "to outbid misère without talon.",                                                \
                                     "Можна оголосити мізер без прикупу, щоб перебити\n"                               \
                                     "дев'ятерну гру, а дев'ятерну без прикупу - щоб\n"                                \
                                     "перебити мізер без прикупу.",                                                    \
                                     "Можно объявить мизер без прикупа, чтобы перебить\n"                              \
                                     "девятерную игру, а девятерную без прикупа — чтобы\n"                             \
                                     "перебить мизер без прикупа.")                                                    \
    PREF_X(Total, "TOTAL", "УСЬОГО", "ВСЕГО")                                                                          \
    PREF_X(Trust, PREF_TRUST, "Довіряю", "Доверяю")                                                                    \
    PREF_X(Type, "TYPE", "ТИП", "ТИП")                                                                                 \
    PREF_X(Ukrainian, "Ukrainian", "Українська", "Украинский")                                                         \
    PREF_X(Variant, "Variant", "Різновид", "Разновидность")                                                            \
    PREF_X(Whist, PREF_WHIST, "Віст", "Вист")                                                                          \
    PREF_X(WhistPassHalfWhist, "Whist, pass, half-whist", "Віст, пас, піввіста", "Вист, пас, полвиста")                \
    PREF_X(Win, "Win", "Перемога", "Победа")                                                                           \
    PREF_X(WinRate, "WIN RATE", "ПЕРЕМОГ", "ПОБЕД")                                                                    \
    PREF_X(Yes, "Yes", "Так", "Да")                                                                                    \
    PREF_X(YourTurn, "Your turn", "Ваш хід", "Ваш ход")                                                                \
    PREF_X(YourTurnFor, "Your turn for", "Ваш хід за", "Ваш ход за")
// clang-format on

enum class GameText : std::size_t {
#define PREF_X(PREF_TEXT_ID, PREF_LANG_EN, PREF_LANG_UA, PREF_LANG_ALT) PREF_TEXT_ID,
    PREF_GAME_TEXT
#undef PREF_X
        Count
};

struct Localication {
    GameText id = GameText::None;
    std::string_view en;
    std::string_view ua;
    std::string_view alt;
};

inline constexpr auto localization = std::array{
#define PREF_X(PREF_TEXT_ID, PREF_LANG_EN, PREF_LANG_UA, PREF_LANG_ALT)                                                \
    Localication{GameText::PREF_TEXT_ID, PREF_LANG_EN, PREF_LANG_UA, PREF_LANG_ALT},
    PREF_GAME_TEXT
#undef PREF_X
};
static_assert(std::size(localization) == std::to_underlying(GameText::Count));

inline constexpr auto WhistingButtons = std::array{GameText::Whist, GameText::HalfWhist, GameText::Pass};
inline constexpr auto MiserButtons = std::array{GameText::Catch, GameText::Trust};
inline constexpr auto TenGameButtons = std::array{GameText::Check, GameText::Trust};
inline constexpr auto HowToPlayButtons = std::array{GameText::Openly, GameText::Closed};

[[nodiscard]] constexpr auto phrases() noexcept -> std::string_view
{
    static constexpr char result[] = {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc23-extensions"
#if defined(__has_embed)
#if __has_embed("../client/resources/text/phrases.txt")
#embed "../client/resources/text/phrases.txt"
#else
#embed "../client/resources/text/default-phrases.txt"
#endif
#else
#embed "../client/resources/text/default-phrases.txt"
#endif
#pragma GCC diagnostic pop
    };
    return {result, sizeof(result)};
}

[[nodiscard]] constexpr auto getCloseReason(const std::uint16_t code) noexcept -> std::string_view
{
    switch (code) {
    case 1000: return "Normal closure";
    case 1001: return "Going away";
    case 1002: return "Protocol error";
    case 1003: return "Unsupported data";
    case 1005: return "No status heceived";
    case 1006: return "Abnormal closure";
    case 1007: return "Invalid payload";
    case 1008: return "Policy violation";
    case 1009: return "Message too big";
    case 1010: return "Mandatory extension";
    case 1011: return "Internal error";
    case 1012: return "Service sestart";
    case 1013: return "Try again later";
    case 1014: return "Bad gateway";
    case 1015: return "TLS handshake failed";
    }
    return "Unknown";
}

// clang-format off
#define PREF_WS_RESULTS \
    PREF_X(SUCCESS) PREF_X(DEFERRED) PREF_X(NOT_SUPPORTED) PREF_X(FAILED_NOT_DEFERRED) PREF_X(INVALID_TARGET) \
    PREF_X(UNKNOWN_TARGET) PREF_X(INVALID_PARAM) PREF_X(FAILED) PREF_X(NO_DATA) PREF_X(TIMED_OUT)

#define PREF_HTML_EVENTS \
    PREF_X(KEYPRESS) PREF_X(KEYDOWN) PREF_X(KEYUP) PREF_X(CLICK) PREF_X(MOUSEDOWN) PREF_X(MOUSEUP) PREF_X(DBLCLICK) PREF_X(MOUSEMOVE) PREF_X(WHEEL) \
    PREF_X(RESIZE) PREF_X(SCROLL) PREF_X(BLUR) PREF_X(FOCUS) PREF_X(FOCUSIN) PREF_X(FOCUSOUT) PREF_X(DEVICEORIENTATION) PREF_X(DEVICEMOTION)   \
    PREF_X(ORIENTATIONCHANGE) PREF_X(FULLSCREENCHANGE) PREF_X(POINTERLOCKCHANGE) PREF_X(VISIBILITYCHANGE) PREF_X(TOUCHSTART)    \
    PREF_X(TOUCHEND) PREF_X(TOUCHMOVE) PREF_X(TOUCHCANCEL) PREF_X(GAMEPADCONNECTED) PREF_X(GAMEPADDISCONNECTED) PREF_X(BEFOREUNLOAD) \
    PREF_X(BATTERYCHARGINGCHANGE) PREF_X(BATTERYLEVELCHANGE) PREF_X(WEBGLCONTEXTLOST) PREF_X(WEBGLCONTEXTRESTORED)         \
    PREF_X(MOUSEENTER) PREF_X(MOUSELEAVE) PREF_X(MOUSEOVER) PREF_X(MOUSEOUT) PREF_X(CANVASRESIZED) PREF_X(POINTERLOCKERROR)
// clang-format on

[[nodiscard]] constexpr auto emResult(const EMSCRIPTEN_RESULT value) noexcept -> std::string_view
{
    switch (value) {
#define PREF_X(PREF_RESULT)                                                                                            \
    case EMSCRIPTEN_RESULT_##PREF_RESULT: return #PREF_RESULT;
        PREF_WS_RESULTS
#undef PREF_X
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr auto htmlEvent(const int value) noexcept -> std::string_view
{
    switch (value) {
#define PREF_X(PREF_EVENT)                                                                                             \
    case EMSCRIPTEN_EVENT_##PREF_EVENT: return #PREF_EVENT;
        PREF_HTML_EVENTS
#undef PREF_X
    }
    return "UNKNOWN";
}

} // namespace pref
