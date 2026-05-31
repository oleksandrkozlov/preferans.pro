#!/usr/bin/env python3
"""Simple web viewer for preferans game.dat."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import subprocess
import sys
import threading
import urllib.error
import urllib.request
from dataclasses import dataclass, field
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import unquote, urlparse


ANALYSIS_CACHE: dict[str, str] = {}
ANALYSIS_MODEL = os.environ.get("PREFBUFF_ANALYSIS_MODEL", "claude-sonnet-4-6")
ANALYSIS_SYSTEM_PROMPT = (
    "You are an experienced Preferans analyst (3-player Russian variant).\n"
    "Input JSON: `player_name`, `stats`, `gameTotals` for the target player; "
    "`opponents` maps other players to the same structure. All played the same deals, so comparisons are valid.\n\n"
    "`gameTotals`: `totalGames`, `totalMmr` (PRIMARY rating metric — base final conclusions on it), "
    "`gameProfitabilityPct` (positive MMR / total absolute MMR across games), `totalDump`, `totalPool`, `totalWhists`.\n\n"
    "`stats`: `handsCount`, `dealsPositive`, `dealsNegative`, "
    "`avgHcp` (~12.5 random), "
    "`avgCardPoints` (~45 random), contract counts `declared6Count`..`declared10Count`, `miserCount`, `passingZeroTricksCount`.\n\n"
    "Categories — each object has `{count, positive, negative, mmrSum, positiveMmr, negativeAbsMmr}`:\n"
    "- `declarerCategory` + `declarerPct` (share), `declarerProfitabilityPct`\n"
    "- `miserCategory` + `miserProfitabilityPct`\n"
    "- `passCategory` + `passRatePct`, `passProfitabilityPct`\n"
    "- `whistCategory` + `whistRatePct`, `whistProfitabilityPct` (whist/half-whist)\n"
    "- `passingCategory` + `passingRatePct`, `passingProfitabilityPct` (all-pass deals)\n"
    "- `passingZeroCategory` + `passingZeroTricksPct` (all-pass, 0 tricks taken)\n"
    "- `suitedAKQCategory` + `suitedAKQPct` (~9.54% random)\n"
    "- `suitedAKQJCategory` + `suitedAKQJPct` (~2.33% random)\n"
    "- `threePlusAcesCategory` + `threePlusAcesPct` (~7.93% random)\n\n"
    "`Pct` fields are percent strings (e.g. \"9.8%\"). "
    "Profitability means positive MMR divided by total absolute MMR, not share of winning deals.\n\n"
    "Task: write a short analysis of the player's style — ONE paragraph, 2–4 sentences, "
    "no headings/lists/markdown, NO exact numbers or percentages, starting with the player's name.\n"
    "Use qualitative comparisons (\"more often than others\", \"on average\", \"noticeably weaker\", \"consistently profitable\", \"stands out\"). "
    "Compare with opponents where meaningful.\n"
    "Cover: overall style (aggressive/conservative/balanced), entry threshold (tight/loose), "
    "performance as declarer / on pass / in all-pass / whisting, behavior on strong vs weak hands, "
    "main strengths and weaknesses, and a final verdict on long-term result (driven primarily by `totalMmr`, with `gameProfitabilityPct` as a supporting efficiency signal).\n"
    "Tone: analytical, neutral, confident. Do not invent anything beyond the data."
)


def _strip_leading_name(text: str, player_name: Any) -> str:
    if not player_name:
        return text
    name = str(player_name).strip().lower()
    if not name:
        return text
    lines = text.lstrip().split("\n")
    if not lines:
        return text
    first = lines[0].strip()
    stripped = first.lstrip("#").strip().strip("*_` ").rstrip(":").strip()
    if stripped.lower() == name:
        rest = "\n".join(lines[1:]).lstrip("\n")
        return rest
    return text


def _call_claude(stats_payload: dict[str, Any]) -> str:
    api_key = os.environ.get("ANTHROPIC_API_KEY")
    if not api_key:
        raise RuntimeError("ANTHROPIC_API_KEY is not set")
    body = {
        "model": ANALYSIS_MODEL,
        "max_tokens": 4096,
        "system": [{"type": "text", "text": ANALYSIS_SYSTEM_PROMPT, "cache_control": {"type": "ephemeral"}}],
        "messages": [
            {
                "role": "user",
                "content": f"Player statistics (JSON):\n{json.dumps(stats_payload, ensure_ascii=False)}",
            }
        ],
    }
    req = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps(body).encode("utf-8"),
        headers={
            "x-api-key": api_key,
            "anthropic-version": "2023-06-01",
            "content-type": "application/json",
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            data = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"Anthropic API {exc.code}: {body}") from None
    chunks = [block.get("text", "") for block in data.get("content", []) if block.get("type") == "text"]
    return "".join(chunks).strip()


ROOT = Path(__file__).resolve().parents[2]
PREF_PB2 = None
EXCLUDED_PLAYER_IDS = {
    "e0c7ad08-dba9-40f5-a226-b1f3e301c4ca",
    "0bf3a0c0-06c9-48a5-af23-731d2fd82f78",
    "5d6c8439-a8d4-4cef-94c2-79938f16124e",
    "4a5303ba-c6e7-4b86-bec2-ad9109e5c40c",
}


def _load_pref_pb2() -> Any:
    gen_dir = Path(__file__).with_name("_generated")
    gen_dir.mkdir(parents=True, exist_ok=True)
    pref_proto = ROOT / "proto" / "pref.proto"
    cpp_features_proto = ROOT / "client" / "deps" / "protobuf" / "src" / "google" / "protobuf" / "cpp_features.proto"
    cmd = [
        "protoc",
        f"--python_out={gen_dir}",
        f"--proto_path={ROOT / 'proto'}",
        f"--proto_path={ROOT / 'client' / 'deps' / 'protobuf' / 'src'}",
        str(pref_proto),
        str(cpp_features_proto),
    ]
    subprocess.run(cmd, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if str(gen_dir) not in sys.path:
        sys.path.insert(0, str(gen_dir))

    import google.protobuf  # type: ignore

    extra_google_protobuf = gen_dir / "google" / "protobuf"
    if extra_google_protobuf.exists() and str(extra_google_protobuf) not in google.protobuf.__path__:
        google.protobuf.__path__.append(str(extra_google_protobuf))

    import pref_pb2  # type: ignore

    return pref_pb2


def _sum_mmr(user: Any) -> int:
    return sum(game.mmr for game in user.games)


@dataclass
class DataStore:
    data_path: Path
    _mtime_ns: int | None = None
    _payload: dict[str, Any] = field(default_factory=dict)
    _lock: threading.Lock = field(default_factory=threading.Lock)

    def get(self) -> dict[str, Any]:
        try:
            current_mtime = os.stat(self.data_path).st_mtime_ns
        except FileNotFoundError:
            return {
                "users": [],
                "games": {},
                "error": f"Data file not found: {self.data_path}",
            }
        with self._lock:
            if self._mtime_ns != current_mtime:
                self._payload = self._load()
                self._mtime_ns = current_mtime
            return self._payload

    def version(self) -> int:
        try:
            return os.stat(self.data_path).st_mtime_ns
        except FileNotFoundError:
            return 0

    def _load(self) -> dict[str, Any]:
        global PREF_PB2
        if PREF_PB2 is None:
            PREF_PB2 = _load_pref_pb2()
        game_data = PREF_PB2.GameData()
        with self.data_path.open("rb") as f:
            game_data.ParseFromString(f.read())

        users = []
        user_names_by_id = {}
        for user in game_data.users:
            user_names_by_id[user.player_id] = user.player_name
            if user.player_id in EXCLUDED_PLAYER_IDS:
                continue
            users.append(
                {
                    "player_id": user.player_id,
                    "player_name": user.player_name,
                    "games_count": len(user.games),
                    "total_mmr": _sum_mmr(user),
                    "games": [
                        {
                            "id": g.id,
                            "game_type": PREF_PB2.GameType.Name(g.game_type),
                            "timestamp": g.timestamp,
                            "duration": g.duration,
                            "pool": g.pool,
                            "dump": g.dump,
                            "whists": g.whists,
                            "mmr": g.mmr,
                        }
                        for g in sorted(user.games, key=lambda v: v.id, reverse=True)
                    ],
                }
            )

        games = {}
        for game in sorted(game_data.games, key=lambda g: g.id, reverse=True):
            deals = []
            for deal in sorted(game.deals, key=lambda d: d.id):
                hands = {
                    pid: sorted(list(cards.cards))
                    for pid, cards in sorted(deal.hands.items(), key=lambda kv: kv[0])
                }
                decisions = dict(sorted(deal.decisions.items(), key=lambda kv: kv[0]))
                tricks = dict(sorted(deal.tricks.items(), key=lambda kv: kv[0]))
                scores = {
                    pid: {
                        "pool": int(score.pool),
                        "dump": int(score.dump),
                        "whists": int(score.whists),
                        "mmr": int(score.mmr),
                    }
                    for pid, score in sorted(deal.scores.items(), key=lambda kv: kv[0])
                }
                trick_history = [
                    {
                        "winner_player_id": trick.winner_player_id,
                        "plays": [
                            {
                                "player_id": play.player_id,
                                "card": play.card,
                            }
                            for play in trick.plays
                        ],
                    }
                    for trick in deal.trick_history
                ]
                deals.append(
                    {
                        "id": deal.id,
                        "talon": sorted(list(deal.talon)),
                        "discarded_cards": sorted(list(deal.discarded_cards)),
                        "declarer_id": deal.declarer_id,
                        "declarer_name": user_names_by_id.get(deal.declarer_id, deal.declarer_id),
                        "contract": deal.contract,
                        "hands": hands,
                        "decisions": decisions,
                        "tricks": tricks,
                        "scores": scores,
                        "trick_history": trick_history,
                    }
                )
            games[str(game.id)] = {"id": game.id, "deals": deals}

        users.sort(key=lambda u: (-u["total_mmr"], u["player_name"].lower()))
        return {"users": users, "games": games}


def _safe_write(handler: BaseHTTPRequestHandler, status: int, content_type: str, body: bytes) -> None:
    try:
        handler.send_response(status)
        handler.send_header("Content-Type", content_type)
        handler.send_header("Content-Length", str(len(body)))
        handler.end_headers()
        handler.wfile.write(body)
    except (BrokenPipeError, ConnectionResetError):
        pass


def _json_response(handler: BaseHTTPRequestHandler, payload: Any, status: int = 200) -> None:
    body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
    _safe_write(handler, status, "application/json; charset=utf-8", body)


def _text_response(handler: BaseHTTPRequestHandler, text: str, status: int = 200) -> None:
    _safe_write(handler, status, "text/plain; charset=utf-8", text.encode("utf-8"))


def _binary_response(handler: BaseHTTPRequestHandler, body: bytes, content_type: str, status: int = 200) -> None:
    _safe_write(handler, status, content_type, body)


def make_handler(store: DataStore):
    index_path = Path(__file__).with_name("index.html")
    cards_dir = Path(__file__).with_name("cards")
    fonts_dir = Path(__file__).resolve().parents[2] / "client" / "resources" / "fonts"

    class Handler(BaseHTTPRequestHandler):
        def log_message(self, *_: Any) -> None:
            return

        def do_GET(self) -> None:  # noqa: N802
            parsed = urlparse(self.path)
            path = unquote(parsed.path)
            if path == "/" or path == "/index.html":
                html = index_path.read_text(encoding="utf-8")
                body = html.encode("utf-8")
                self.send_response(200)
                self.send_header("Content-Type", "text/html; charset=utf-8")
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)
                return
            if path == "/favicon.ico" or path == "/prefbuff/favicon.ico":
                icon_path = cards_dir / "favicon.ico"
                if icon_path.exists() and icon_path.is_file():
                    _binary_response(self, icon_path.read_bytes(), "image/x-icon")
                else:
                    _text_response(self, "Not found", status=404)
                return
            if path.startswith("/assets/fonts/"):
                filename = Path(path).name
                font_path = fonts_dir / filename
                if (
                    filename.endswith(".ttf")
                    and font_path.exists()
                    and font_path.is_file()
                    and font_path.resolve().parent == fonts_dir.resolve()
                ):
                    _binary_response(self, font_path.read_bytes(), "font/ttf")
                else:
                    _text_response(self, "Not found", status=404)
                return
            if path.startswith("/assets/cards/"):
                filename = Path(path).name
                card_path = cards_dir / filename
                if (
                    filename.endswith(".png")
                    and card_path.exists()
                    and card_path.is_file()
                    and card_path.resolve().parent == cards_dir.resolve()
                ):
                    _binary_response(self, card_path.read_bytes(), "image/png")
                else:
                    _text_response(self, "Not found", status=404)
                return

            data = store.get()
            if path == "/api/users":
                users = [
                    {
                        "player_id": user["player_id"],
                        "player_name": user["player_name"],
                        "games_count": user["games_count"],
                        "total_mmr": user["total_mmr"],
                    }
                    for user in data["users"]
                ]
                _json_response(self, users)
                return

            if path.startswith("/api/users/") and path.endswith("/games"):
                player_id = path[len("/api/users/") : -len("/games")].strip("/")
                user = next((u for u in data["users"] if u["player_id"] == player_id), None)
                if user is None:
                    _json_response(self, {"error": f"Unknown user: {player_id}"}, status=404)
                    return
                _json_response(self, user["games"])
                return

            if path.startswith("/api/games/"):
                game_id = path.split("/")[-1]
                game = data["games"].get(game_id)
                _json_response(self, game if game is not None else None)
                return

            if path == "/api/health":
                _json_response(self, {"ok": True})
                return

            if path == "/api/version":
                _json_response(self, {"version": store.version()})
                return

            _text_response(self, "Not found", status=404)

        def do_POST(self) -> None:  # noqa: N802
            parsed = urlparse(self.path)
            path = unquote(parsed.path)
            if not path.endswith("/api/analysis"):
                _text_response(self, "Not found", status=404)
                return
            length = int(self.headers.get("Content-Length") or 0)
            raw = self.rfile.read(length) if length > 0 else b""
            try:
                payload = json.loads(raw.decode("utf-8") or "{}")
            except ValueError:
                _json_response(self, {"error": "Invalid JSON"}, status=400)
                return
            stats = payload.get("stats")
            if not isinstance(stats, dict):
                _json_response(self, {"error": "Missing stats object"}, status=400)
                return
            opponents = payload.get("opponents") if isinstance(payload.get("opponents"), dict) else {}
            game_totals = payload.get("gameTotals") if isinstance(payload.get("gameTotals"), dict) else {}
            cache_payload = {
                "player_name": payload.get("player_name"),
                "stats": stats,
                "gameTotals": game_totals,
                "opponents": opponents,
            }
            # Cache only by number of games played — regenerate only when a new game is added
            # for the target player or any opponent, not on every data refresh.
            cache_signature = {
                "player_name": payload.get("player_name"),
                "totalGames": game_totals.get("totalGames"),
                "opponents": {
                    pid: (op.get("gameTotals") or {}).get("totalGames")
                    for pid, op in opponents.items()
                    if isinstance(op, dict)
                },
            }
            cache_blob = json.dumps(cache_signature, ensure_ascii=False, sort_keys=True)
            cache_key = hashlib.sha256(cache_blob.encode("utf-8")).hexdigest()
            if cache_key in ANALYSIS_CACHE:
                _json_response(self, {"text": ANALYSIS_CACHE[cache_key], "cached": True})
                return
            try:
                text = _call_claude(cache_payload)
            except Exception as exc:  # noqa: BLE001
                _json_response(self, {"error": str(exc)}, status=500)
                return
            text = _strip_leading_name(text, payload.get("player_name"))
            ANALYSIS_CACHE[cache_key] = text
            _json_response(self, {"text": text, "cached": False})

    return Handler


def main() -> int:
    parser = argparse.ArgumentParser(description="Preferans game.dat viewer")
    parser.add_argument("data", help="Path to game.dat")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()

    data_path = Path(args.data).resolve()
    store = DataStore(data_path=data_path)
    server = ThreadingHTTPServer((args.host, args.port), make_handler(store))
    print(f"Serving {data_path} at http://{args.host}:{args.port}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
