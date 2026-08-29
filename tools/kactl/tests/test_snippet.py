"""Unit tests for shared snippet/chapter preprocessing."""

from __future__ import annotations

import shutil
import tempfile
import unittest
from io import StringIO
from pathlib import Path

from tools.kactl import CONTENT
from tools.kactl.chapter import chapter_order, parse_chapter_imports, strip_figures
from tools.kactl.emit_tex import print_header
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

    def test_output_format_is_included_as_code(self):
        path = CONTENT / "contest" / "Output.h"
        processed = process_path(path)
        self.assertFalse(processed.error)
        self.assertIn('cout << format("{:06b}", b);', processed.code)
        self.assertIn('cout << format("{:b}", b);', processed.code)
        self.assertIn("setprecision(6)", processed.code)
        self.assertNotIn("include-line", processed.code)
        self.assertNotIn("bitset", processed.code)

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
                "strings",
                "various",
                "geometry",
                "appendix",
            ],
        )

    def test_commented_import_excluded_from_pdf(self):
        imports = parse_chapter_imports("data-structures")
        self.assertIn("UnionFind.h", imports)
        self.assertFalse(imports["UnionFind.h"].included_in_pdf)
        self.assertTrue(imports["LazySegmentTree.h"].included_in_pdf)

    def test_simd_excluded_from_pdf(self):
        imports = parse_chapter_imports("various")
        self.assertIn("SIMD.h", imports)
        self.assertFalse(imports["SIMD.h"].included_in_pdf)
        self.assertTrue(imports["Pragmas.h"].included_in_pdf)

    def test_trivial_graph_and_nt_excluded_from_pdf(self):
        graph = parse_chapter_imports("graph")
        self.assertFalse(graph["BellmanFord.h"].included_in_pdf)
        self.assertFalse(graph["FloydWarshall.h"].included_in_pdf)
        self.assertTrue(graph["TopoSort.h"].included_in_pdf)
        nt = parse_chapter_imports("number-theory")
        self.assertFalse(nt["FloorBlocks.h"].included_in_pdf)
        self.assertFalse(nt["Mobius.h"].included_in_pdf)
        self.assertTrue(nt["LinearSieve.h"].included_in_pdf)

    def test_kactlfigdesc_unwrapped_for_site(self):
        desc = process_path(CONTENT / "geometry" / "lineDistance.h").commands["Description"]
        stripped = strip_figures(desc)
        self.assertIn("signed distance", stripped)
        self.assertNotIn("kactlfigdesc", stripped)
        self.assertNotIn("includegraphics", stripped)
        self.assertNotIn("minipage", stripped)
        self.assertNotIn("content/geometry/lineDistance", stripped)
        self.assertFalse(stripped.rstrip().endswith("%"))

    def test_geometry_included_in_pdf(self):
        imports = parse_chapter_imports("geometry")
        self.assertTrue(imports["Point.h"].included_in_pdf)
        self.assertTrue(imports["lineDistance.h"].included_in_pdf)
        self.assertTrue(imports["ConvexHull.h"].included_in_pdf)
        self.assertTrue(imports["HalfplaneIntersection.h"].included_in_pdf)
        self.assertTrue(imports["LineProjectionReflection.h"].included_in_pdf)
        self.assertTrue(imports["CircleLine.h"].included_in_pdf)
        self.assertTrue(imports["PolygonUnion.h"].included_in_pdf)
        self.assertTrue(imports["ManhattanMST.h"].included_in_pdf)
        self.assertTrue(imports["DelaunayTriangulation.h"].included_in_pdf)
        self.assertTrue(imports["FastDelaunay.h"].included_in_pdf)
        order = chapter_order()
        self.assertEqual(order[-2], "geometry")
        self.assertEqual(order[-1], "appendix")

    def test_geometry_site_descriptions_drop_figures(self):
        names = [
            "lineDistance.h",
            "SegmentDistance.h",
            "SegmentIntersection.h",
            "lineIntersection.h",
            "linearTransformation.h",
            "circumcircle.h",
            "PolygonCut.h",
            "ConvexHull.h",
        ]
        for name in names:
            with self.subTest(name=name):
                desc = process_path(CONTENT / "geometry" / name).commands["Description"]
                stripped = strip_figures(desc)
                self.assertTrue(stripped)
                self.assertNotIn("kactlfigdesc", stripped)
                self.assertNotIn("includegraphics", stripped)
                self.assertNotIn("minipage", stripped)
                self.assertNotIn("vspace", stripped)
                self.assertFalse(stripped.rstrip().endswith("%"), stripped)

    def test_raw_template_keeps_include(self):
        path = CONTENT / "contest" / "template.cpp"
        processed = process_path(path)
        self.assertEqual(processed.mode, "raw")
        self.assertIn("#include <bits/stdc++.h>", processed.code)

    def test_header_seed_matches_import_order(self):
        from tools.kactl.__main__ import preprocess
        from tools.kactl import BUILD

        self.assertEqual(preprocess(), 0)
        seed = (BUILD / "header.tmp.seed").read_text(encoding="utf-8").splitlines()
        self.assertEqual(
            seed[:6],
            [
                "template.cpp",
                ".bashrc",
                ".vimrc",
                "hash.sh",
                "troubleshoot.txt",
                "Random.h",
            ],
        )


class TestPrintHeader(unittest.TestCase):
    def setUp(self):
        self.tmp = Path(tempfile.mkdtemp())
        self.queue = self.tmp / "header.tmp"
        self.queue.write_text(
            "template.cpp\n.bashrc\nRandom.h\nSegmentTree.h\n",
            encoding="utf-8",
        )

    def tearDown(self):
        shutil.rmtree(self.tmp)

    def test_consumes_through_first_mark(self):
        out = StringIO()
        print_header(".bashrc|.bashrc", out, self.queue)
        self.assertEqual(
            out.getvalue(),
            "\\fontsize{10}{10}\\hspace{3mm}\\textbf{template\\enspace{}.bashrc}\n",
        )
        remaining = self.queue.read_text(encoding="utf-8").splitlines()
        self.assertEqual(remaining, ["Random.h", "SegmentTree.h"])

    def test_second_page_consumes_the_rest(self):
        print_header(".bashrc|.bashrc", StringIO(), self.queue)
        out = StringIO()
        print_header("SegmentTree.h|SegmentTree.h", out, self.queue)
        self.assertEqual(
            out.getvalue(),
            "\\fontsize{10}{10}\\hspace{3mm}\\textbf{Random\\enspace{}SegmentTree}\n",
        )
        self.assertEqual(self.queue.read_text(encoding="utf-8"), "")

    def test_unknown_mark_leaves_queue(self):
        before = self.queue.read_text(encoding="utf-8")
        out = StringIO()
        print_header("Missing.h|Missing.h", out, self.queue)
        self.assertEqual(out.getvalue(), "")
        self.assertEqual(self.queue.read_text(encoding="utf-8"), before)

    def test_long_header_uses_smaller_font(self):
        long_names = [f"VeryLongSnippetName{i:02d}.h" for i in range(12)]
        self.queue.write_text("".join(n + "\n" for n in long_names), encoding="utf-8")
        out = StringIO()
        print_header(f"{long_names[-1]}|{long_names[-1]}", out, self.queue)
        self.assertTrue(out.getvalue().startswith("\\fontsize{8}{8}"))


if __name__ == "__main__":
    unittest.main()
