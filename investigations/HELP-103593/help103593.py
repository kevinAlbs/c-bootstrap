# /// script
# requires-python = ">=3.9"
# dependencies = [
#     "pymongo>=4.9",
# ]
# ///
"""HELP-103593: print PyMongo server changes while a secondary is disrupted.

Mirrors help103593.c (C driver) so the two can be compared directly:
  * Pings the cluster every ~50ms.
  * Prints "." about once per second as progress.
  * Prints server type changes (SERVER_CHANGED), heartbeat started /
    succeeded / failed events (to spot a hung heartbeat), plus ping errors
    and slow pings (>1s) since those are what an application would see.

Usage:
    MONGODB_URI='mongodb://...' uv run help103593.py   # runs until Ctrl+C
    # Local 3-node RS: in another terminal
    sh servers/sigstop.sh                      # SIGSTOP the 27019 secondary
    kill -CONT $(pgrep -f "mongod.*27019")     # resume
"""

import os
import time
from datetime import datetime, timezone

from pymongo import MongoClient, monitoring

# Same URI options as help103593.c.
DEFAULT_URI = (
    "mongodb://localhost:27017,localhost:27018,localhost:27019/"
    "?readPreference=primary&retryWrites=true&retryReads=true"
    "&serverSelectionTryOnce=false&serverSelectionTimeoutMS=3000"
    "&connectTimeoutMS=1000&socketTimeoutMS=2000"
    "&heartbeatFrequencyMS=2000&socketCheckIntervalMS=1000"
)
URI = os.environ.get("MONGODB_URI", DEFAULT_URI)


def log(msg):
    now = datetime.now(timezone.utc).strftime("%H:%M:%S.%f")[:-3]
    # Start on a fresh line so output doesn't follow a progress "." line.
    print(f"[{now}] {msg}", flush=True)


class Servers(monitoring.ServerListener):
    def opened(self, event):
        pass

    def closed(self, event):
        pass

    def description_changed(self, event):
        prev, new = event.previous_description, event.new_description
        if prev.server_type != new.server_type:
            host, port = event.server_address
            log(
                f"SERVER_CHANGED: {host}:{port} "
                f"{prev.server_type_name} -> {new.server_type_name}"
            )


class Heartbeats(monitoring.ServerHeartbeatListener):
    @staticmethod
    def _addr(event):
        host, port = event.connection_id
        return f"{host}:{port}"

    def started(self, event):
        log(f"HEARTBEAT_STARTED: {self._addr(event)} awaited={event.awaited}")

    def succeeded(self, event):
        log(
            f"HEARTBEAT_SUCCEEDED: {self._addr(event)} awaited={event.awaited} "
            f"duration={event.duration * 1000:.0f}ms"
        )

    def failed(self, event):
        log(
            f"HEARTBEAT_FAILED: {self._addr(event)} awaited={event.awaited} "
            f"duration={event.duration * 1000:.0f}ms error={event.reply}"
        )


monitoring.register(Servers())
monitoring.register(Heartbeats())


def main():
    client = MongoClient(URI)
    timer_ms = 0.0

    # Runs until interrupted (Ctrl+C).
    try:
        while True:
            t0 = time.monotonic()
            try:
                client.admin.command("ping")
                ms = (time.monotonic() - t0) * 1000
                if ms > 1000:
                    log(f"ping | total: {ms:.2f} ms *** SLOW")
            except Exception as exc:  # noqa: BLE001
                log(f"ERROR: {exc}")

            time.sleep(0.05)

            timer_ms += (time.monotonic() - t0) * 1000
            if timer_ms > 1000:
                print(".", end="", flush=True)
                timer_ms = 0.0
    except KeyboardInterrupt:
        pass
    finally:
        client.close()
        print("\nDONE")


if __name__ == "__main__":
    main()
