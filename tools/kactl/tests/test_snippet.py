"""Unit tests for shared snippet/chapter preprocessing."""

from __future__ import annotations

import unittest

from tools.kactl import CONTENT
from tools.kactl.chapter import chapter_order, parse_chapter_imports
from tools.kactl.snippet import process_path, resolve_language


class TestLanguageMap(unittest.TestCase):
    def test_contest_overrides_without_dash_l(self):
        self.assertEqual(resolve_language("template.cpp"), ("raw", "C++"))
        self.assertEqual(resolve_language(".bashrc"), ("raw", "bash"))
        self.assertEqual(resolve_language(".vimrc"), ("raw", "raw"))
        self.assertEqual(resolve_language("hash.sh"), ("raw", "raw"))
        self.assertEqual(resolve_language("techniques.txt"), ("raw", "raw"))

    def test_header_extension(self):
        self.assertEqual(resolve_language("Dinic.h"), ("comments", "C++"))


class TestStripAnnotations(unittest.TestCase):
    def test_include_line_and_exclude_line(self):
        path = CONTENT / "numerical" / "FastFourierTransform.h"
        processed = process_path(path)
        self.assertFalse(processed.error)
        self.assertIn("C z = rt[j+k] * a[i+j+k];", processed.code)
        self.assertNotIn("exclude-line", processed.code)
        self.assertNotIn("include-line", processed.code)
        self.assertNotIn("(double *)&rt", processed.code)

    def test_keep_include_stays_in_listing(self):
        path = CONTENT / "data-structures" / "OrderStatisticTree.h"
        processed = process_path(path)
        self.assertFalse(processed.error)
        self.assertIn("#include <bits/extc++.h>", processed.code)
        self.assertNotIn("<bits/extc++.h>", processed.includes)

    def test_exclude_line_drops_mod_constant(self):
        path = CONTENT / "numerical" / "LinearRecurrence.h"
        processed = process_path(path)
        self.assertFalse(processed.error)
        self.assertNotIn("const ll mod = 5", processed.code)
        self.assertTrue(processed.hash_prefix)
        self.assertEqual(len(processed.hash_prefix.strip(", ")), 6)


class TestChapterParse(unittest.TestCase):
    def test_order_matches_kactl_tex(self):
        self.assertEqual(
            chapter_order(),
            [
                "contest",
                "math",
                "data-structures",
                "numerical",
                "number-theory",
                "combinatorial",
                "graph",
                "geometry",
                "strings",
                "various",
                "appendix",
            ],
        )

    def test_commented_import_excluded_from_pdf(self):
        imports = parse_chapter_imports("data-structures")
        self.assertIn("UnionFind.h", imports)
        self.assertFalse(imports["UnionFind.h"].included_in_pdf)
        self.assertTrue(imports["LazySegmentTree.h"].included_in_pdf)

    def test_raw_template_keeps_include(self):
        path = CONTENT / "contest" / "template.cpp"
        processed = process_path(path)
        self.assertEqual(processed.mode, "raw")
        self.assertIn("#include <bits/stdc++.h>", processed.code)


if __name__ == "__main__":
    unittest.main()
