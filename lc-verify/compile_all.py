#!/usr/bin/env python3
"""Bundle and compile every lc-verify problem."""
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
CXX = "g++"
problems = sorted((HERE / "problems").glob("*.cpp"))
failed = []
for src in problems:
    out = HERE / "build" / (src.stem + ".bundled.cpp")
    binp = HERE / "build" / src.stem
    print(f"== {src.stem} ==", flush=True)
    subprocess.check_call([sys.executable, str(HERE / "bundle.py"), str(src), "-o", str(out)])
    r = subprocess.run(
        [CXX, "-std=c++20", "-O2", "-pipe", "-o", str(binp), str(out)],
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        print(r.stderr)
        failed.append(src.stem)
    else:
        print("ok")
if failed:
    print("FAILED", failed)
    sys.exit(1)
print("all compiled", len(problems))
