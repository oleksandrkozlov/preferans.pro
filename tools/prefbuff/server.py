#!/usr/bin/env python3
"""Simple web viewer for preferans game.dat."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from dataclasses import dataclass, field
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import unquote, urlparse


ROOT = Path(__file__).resolve().parents[2]
PREF_PB2 = None
EXCLUDED_PLAYER_IDS = {
    "e0c7ad08-dba9-40f5-a226-b1f3e301c4ca",
    "0bf3a0c0-06c9-48a5-af23-731d2fd82f78",
    "5d6c8439-a8d4-4cef-94c2-79938f16124e",
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

    def get(self) -> dict[str, Any]:
        try:
            current_mtime = os.stat(self.data_path).st_mtime_ns
        except FileNotFoundError:
            return {
                "users": [],
                "games": {},
                "error": f"Data file not found: {self.data_path}",
            }
        if self._mtime_ns != current_mtime:
            self._payload = self._load()
            self._mtime_ns = current_mtime
        return self._payload

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
                    }
                )
            games[str(game.id)] = {"id": game.id, "deals": deals}

        users.sort(key=lambda u: (-u["total_mmr"], u["player_name"].lower()))
        return {"users": users, "games": games}


def _json_response(handler: BaseHTTPRequestHandler, payload: Any, status: int = 200) -> None:
    body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
    handler.send_response(status)
    handler.send_header("Content-Type", "application/json; charset=utf-8")
    handler.send_header("Content-Length", str(len(body)))
    handler.end_headers()
    handler.wfile.write(body)


def _text_response(handler: BaseHTTPRequestHandler, text: str, status: int = 200) -> None:
    body = text.encode("utf-8")
    handler.send_response(status)
    handler.send_header("Content-Type", "text/plain; charset=utf-8")
    handler.send_header("Content-Length", str(len(body)))
    handler.end_headers()
    handler.wfile.write(body)


def _binary_response(handler: BaseHTTPRequestHandler, body: bytes, content_type: str, status: int = 200) -> None:
    handler.send_response(status)
    handler.send_header("Content-Type", content_type)
    handler.send_header("Content-Length", str(len(body)))
    handler.end_headers()
    handler.wfile.write(body)


def make_handler(store: DataStore):
    index_path = Path(__file__).with_name("index.html")
    cards_dir = Path(__file__).with_name("cards")

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
                if game is None:
                    _json_response(self, {"error": f"Unknown game: {game_id}"}, status=404)
                    return
                _json_response(self, game)
                return

            if path == "/api/health":
                _json_response(self, {"ok": True})
                return

            _text_response(self, "Not found", status=404)

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
