#!/usr/bin/env python3
"""Submit bundled C++ to Library Checker (judge.yosupo.jp)."""
from __future__ import annotations

import argparse
import json
import os
import ssl
import subprocess
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
API = "https://v3.api.judge.yosupo.jp"
FIREBASE_KEY = "AIzaSyCmpkoMVbKRDm2H0MJHB0iZ43uQtSqiLV0"
FIREBASE_LOGIN = (
    "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword"
    f"?key={FIREBASE_KEY}"
)
FIREBASE_REFRESH = (
    "https://securetoken.googleapis.com/v1/token" f"?key={FIREBASE_KEY}"
)

CTX = ssl.create_default_context()


def http_json(url: str, data=None, headers=None, method=None) -> dict:
    body = None
    hdrs = {"Accept": "application/json", **(headers or {})}
    if data is not None:
        body = json.dumps(data).encode()
        hdrs["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=body, headers=hdrs, method=method)
    try:
        with urllib.request.urlopen(req, context=CTX, timeout=60) as resp:
            return json.loads(resp.read().decode())
    except urllib.error.HTTPError as e:
        err = e.read().decode()
        raise SystemExit(f"HTTP {e.code} {url}: {err}") from e


def login(email: str, password: str) -> tuple[str, str]:
    d = http_json(
        FIREBASE_LOGIN,
        {"email": email, "password": password, "returnSecureToken": True},
    )
    return d["idToken"], d["refreshToken"]


def refresh(refresh_token: str) -> str:
    # x-www-form-urlencoded
    body = urllib.parse.urlencode(
        {"grant_type": "refresh_token", "refresh_token": refresh_token}
    ).encode()
    req = urllib.request.Request(
        FIREBASE_REFRESH,
        data=body,
        headers={"Content-Type": "application/x-www-form-urlencoded"},
    )
    with urllib.request.urlopen(req, context=CTX, timeout=60) as resp:
        d = json.loads(resp.read().decode())
    return d["id_token"]


def bundle(src: Path) -> str:
    out = HERE / "build" / (src.stem + ".bundled.cpp")
    subprocess.check_call(
        [sys.executable, str(HERE / "bundle.py"), str(src), "-o", str(out)],
        cwd=REPO,
    )
    return out.read_text()


def submit(token: str, problem: str, source: str, lang: str = "cpp") -> int:
    d = http_json(
        f"{API}/submit",
        {"problem": problem, "source": source, "lang": lang, "tle_knockout": False},
        headers={"Authorization": f"Bearer {token}"},
    )
    return int(d["id"])


FINAL = {"AC", "WA", "TLE", "RE", "MLE", "QLE", "CE", "IE", "PE", "OLE"}


def in_progress(st: str) -> bool:
    if not st or st in {"WJ", "WR", "-"}:
        return True
    if "/" in st:
        return True
    return st not in FINAL


def poll(sid: int, timeout: float = 600) -> dict:
    t0 = time.time()
    last = None
    while time.time() - t0 < timeout:
        d = http_json(f"{API}/submissions/{sid}")
        st = d.get("overview", {}).get("status", "")
        last = d
        if not in_progress(st):
            return d
        time.sleep(2)
    return last or {}


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("problem")
    ap.add_argument("--src", type=Path)
    ap.add_argument("--lang", default="cpp")
    ap.add_argument("--email", default=os.environ.get("YOSUPO_EMAIL", ""))
    ap.add_argument("--password", default=os.environ.get("YOSUPO_PASSWORD", ""))
    args = ap.parse_args()
    src = args.src or HERE / "problems" / f"{args.problem}.cpp"
    if not src.exists():
        raise SystemExit(f"missing {src}")
    if not args.email or not args.password:
        raise SystemExit("set YOSUPO_EMAIL / YOSUPO_PASSWORD")
    token, _ = login(args.email, args.password)
    source = bundle(src)
    sid = submit(token, args.problem, source, args.lang)
    print(f"submitted {args.problem} id={sid}", flush=True)
    info = poll(sid)
    ov = info.get("overview", {})
    print(json.dumps({"id": sid, "problem": args.problem, "overview": ov}, indent=2))
    (HERE / "results").mkdir(exist_ok=True)
    (HERE / "results" / f"{args.problem}.json").write_text(json.dumps(info, indent=2))
    st = ov.get("status")
    if st != "AC":
        sys.exit(2)


if __name__ == "__main__":
    main()
