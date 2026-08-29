#!/usr/bin/env python3
"""Submit every lc-verify problem and wait for judge results."""
from __future__ import annotations

import json
import os
import sys
import time
from pathlib import Path

import submit as S

HERE = Path(__file__).resolve().parent
PROBLEMS = sorted(p.stem for p in (HERE / "problems").glob("*.cpp"))


def main() -> None:
    email = os.environ.get("YOSUPO_EMAIL", "")
    password = os.environ.get("YOSUPO_PASSWORD", "")
    if not email or not password:
        raise SystemExit("set YOSUPO_EMAIL / YOSUPO_PASSWORD")
    token, refresh_tok = S.login(email, password)
    me = S.http_json(
        f"{S.API}/auth/current_user",
        headers={"Authorization": f"Bearer {token}"},
    )
    print("user", me, flush=True)

    ids = {}
    for name in PROBLEMS:
        src = HERE / "problems" / f"{name}.cpp"
        source = S.bundle(src)
        try:
            sid = S.submit(token, name, source)
        except SystemExit as e:
            if "401" in str(e) or "UNAUTHENTICATED" in str(e):
                token = S.refresh(refresh_tok)
                sid = S.submit(token, name, source)
            else:
                raise
        ids[name] = sid
        print(f"submitted {name} {sid}", flush=True)
        time.sleep(0.4)

    (HERE / "results").mkdir(exist_ok=True)
    (HERE / "results" / "ids.json").write_text(json.dumps(ids, indent=2))

    pending = dict(ids)
    done = {}
    t0 = time.time()
    while pending and time.time() - t0 < 1800:
        for name, sid in list(pending.items()):
            try:
                info = S.http_json(f"{S.API}/submissions/{sid}")
            except SystemExit:
                continue
            st = info.get("overview", {}).get("status", "")
            if st and st not in ("WJ", "WR"):
                done[name] = info
                pending.pop(name)
                ov = info["overview"]
                print(
                    f"{st:4} {name} id={sid} time={ov.get('time')} mem={ov.get('memory')}",
                    flush=True,
                )
                (HERE / "results" / f"{name}.json").write_text(
                    json.dumps(info, indent=2)
                )
        if pending:
            time.sleep(3)

    summary = {
        name: done.get(name, {}).get("overview", {}).get("status", "TIMEOUT")
        for name in PROBLEMS
    }
    print(json.dumps(summary, indent=2), flush=True)
    (HERE / "results" / "summary.json").write_text(json.dumps(summary, indent=2))
    if any(v != "AC" for v in summary.values()):
        sys.exit(2)


if __name__ == "__main__":
    sys.path.insert(0, str(HERE))
    main()
