"""Shared KACTL snippet/chapter processing for the PDF and snippets site."""

from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CONTENT = REPO_ROOT / "content"
BUILD = REPO_ROOT / "build"
WEB_PUBLIC = REPO_ROOT / "web" / "public"
