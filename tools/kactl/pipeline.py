"""Build the shared document model from KACTL's content tree."""

from __future__ import annotations

from . import CONTENT
from .chapter import KactlImport, chapter_order, discover_files, parse_chapter
from .model import Chapter, Document, Snippet
from .snippet import ProcessedSnippet, process_path


def _process_chapter_files(
    chapter_id: str,
    imports: dict[str, KactlImport],
) -> dict[str, tuple[ProcessedSnippet, bool]]:
    processed_by_name: dict[str, tuple[ProcessedSnippet, bool]] = {}
    for path in discover_files(chapter_id, imports):
        spec = imports.get(path.name)
        processed = process_path(path, spec.lang_flag if spec else None)
        if not processed.code and not processed.commands.get("Description"):
            continue
        if (
            path.name not in imports
            and path.suffix not in {".h", ".hpp", ".cpp", ".java"}
        ):
            continue
        processed_by_name[path.name] = (
            processed,
            spec.included_in_pdf if spec else False,
        )
    return processed_by_name


def build_document() -> Document:
    """Parse and process all chapters once for both output adapters."""
    chapters: list[Chapter] = []
    snippets: list[Snippet] = []
    blocks = []

    for chapter_id in chapter_order():
        if not (CONTENT / chapter_id / "chapter.tex").is_file():
            continue

        parsed = parse_chapter(chapter_id)
        by_name = _process_chapter_files(chapter_id, parsed.imports)
        chapter_snippets: list[Snippet] = []

        # Imported snippets follow their typesetting order. Unreferenced source
        # files follow afterward so the web app can still expose them.
        for name in parsed.imports:
            if name not in by_name:
                continue
            processed, included = by_name.pop(name)
            chapter_snippets.append(
                Snippet(
                    id=f"{chapter_id}/{name}",
                    chapter=chapter_id,
                    processed=processed,
                    included_in_pdf=included,
                )
            )
        for name, (processed, included) in by_name.items():
            chapter_snippets.append(
                Snippet(
                    id=f"{chapter_id}/{name}",
                    chapter=chapter_id,
                    processed=processed,
                    included_in_pdf=included,
                )
            )

        mentioned = {
            block["id"] for block in parsed.blocks if block["type"] == "snippet"
        }
        for snippet in chapter_snippets:
            if snippet.id in mentioned:
                continue
            parsed.blocks.append(
                {
                    "type": "snippet",
                    "id": snippet.id,
                    "chapter": snippet.chapter,
                    "includedInPdf": snippet.included_in_pdf,
                }
            )

        if parsed.blocks or chapter_snippets:
            chapters.append(Chapter(id=chapter_id, title=parsed.title))
            blocks.extend(parsed.blocks)
            snippets.extend(chapter_snippets)

    return Document(chapters=chapters, snippets=snippets, blocks=blocks)
