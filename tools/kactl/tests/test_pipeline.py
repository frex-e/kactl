"""Consistency tests for the shared document model and output adapters."""

from __future__ import annotations

import unittest

from tools.kactl.emit_json import document_payload
from tools.kactl.emit_tex import listing_tex
from tools.kactl.pipeline import build_document


class TestSharedDocument(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.document = build_document()
        cls.payload = document_payload(cls.document)

    def test_snippet_blocks_match_processed_records(self):
        records = {
            snippet.id: snippet.included_in_pdf
            for snippet in self.document.snippets
        }
        blocks = {
            block["id"]: block["includedInPdf"]
            for block in self.document.blocks
            if block["type"] == "snippet"
        }
        self.assertEqual(blocks, records)

    def test_web_language_comes_from_shared_processing(self):
        web_snippets = {
            snippet["id"]: snippet for snippet in self.payload["snippets"]
        }
        records = {snippet.id: snippet for snippet in self.document.snippets}
        for snippet_id, record in records.items():
            with self.subTest(snippet_id=snippet_id):
                self.assertEqual(
                    web_snippets[snippet_id]["language"],
                    record.processed.syntax_lang,
                )

        self.assertEqual(web_snippets["contest/template.cpp"]["language"], "cpp")
        self.assertEqual(web_snippets["contest/.bashrc"]["language"], "bash")
        self.assertEqual(web_snippets["contest/.vimrc"]["language"], "plaintext")
        self.assertEqual(web_snippets["contest/hash.sh"]["language"], "bash")

    def test_pdf_adapter_uses_same_processed_snippet(self):
        record = next(
            snippet
            for snippet in self.document.snippets
            if snippet.id == "contest/template.cpp"
        )
        tex = listing_tex(record.processed)
        self.assertIn("language=C++", tex)
        self.assertIn(record.processed.code, tex)


if __name__ == "__main__":
    unittest.main()
