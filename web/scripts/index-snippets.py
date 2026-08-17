#!/usr/bin/env python3
"""Shim: snippets.json is produced by `python3 -m tools.kactl preprocess`."""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from tools.kactl.__main__ import main

if __name__ == "__main__":
    raise SystemExit(main(["preprocess", *sys.argv[1:]]))
