"""Presentation-neutral document model shared by the PDF and web adapters."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Literal, TypedDict

from .snippet import ProcessedSnippet


class HeadingBlock(TypedDict):
    type: Literal["heading"]
    id: str
    chapter: str
    level: int
    title: str
    searchText: str


class ProseBlock(TypedDict):
    type: Literal["prose"]
    id: str
    chapter: str
    latex: str
    searchText: str


class SnippetBlock(TypedDict):
    type: Literal["snippet"]
    id: str
    chapter: str
    includedInPdf: bool


DocumentBlock = HeadingBlock | ProseBlock | SnippetBlock


@dataclass(frozen=True)
class Chapter:
    id: str
    title: str


@dataclass(frozen=True)
class Snippet:
    id: str
    chapter: str
    processed: ProcessedSnippet
    included_in_pdf: bool


@dataclass(frozen=True)
class Document:
    """One parsed content tree, before either output adapter formats it."""

    chapters: list[Chapter]
    snippets: list[Snippet]
    blocks: list[DocumentBlock]

    def __post_init__(self) -> None:
        snippet_ids = [snippet.id for snippet in self.snippets]
        if len(snippet_ids) != len(set(snippet_ids)):
            raise ValueError("Duplicate snippet ids in document model")

        block_ids = [
            block["id"] for block in self.blocks if block["type"] == "snippet"
        ]
        if len(block_ids) != len(set(block_ids)):
            raise ValueError("Duplicate snippet blocks in document model")
        if set(block_ids) != set(snippet_ids):
            raise ValueError("Snippet records and document blocks do not match")

        included = {
            snippet.id: snippet.included_in_pdf for snippet in self.snippets
        }
        for block in self.blocks:
            if block["type"] != "snippet":
                continue
            if block["includedInPdf"] != included[block["id"]]:
                raise ValueError(
                    f"PDF inclusion differs for snippet {block['id']}"
                )
