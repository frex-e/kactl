"""Tests for PR-oriented stress-test selection."""

from __future__ import annotations

import subprocess
import unittest
from pathlib import Path

from tools.kactl import REPO_ROOT
from tools.stress_select import list_tests, select


class TestSelectHelpers(unittest.TestCase):
    def test_lists_cpp_tests_only(self):
        tests = list_tests(REPO_ROOT)
        self.assertGreater(len(tests), 50)
        self.assertTrue(all(t.startswith("stress-tests/") and t.endswith(".cpp") for t in tests))
        self.assertIn("stress-tests/graph/2sat.cpp", tests)
        self.assertNotIn("stress-tests/utilities/template.h", tests)

    def test_header_include_selects_direct_and_cross_test(self):
        sel = select(REPO_ROOT, ["content/data-structures/LiChao.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertEqual(sel.tests, ["stress-tests/data-structures/LiChao.cpp"])

        sel = select(REPO_ROOT, ["content/data-structures/LineContainer.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertIn("stress-tests/data-structures/LineContainer.cpp", sel.tests)
        self.assertIn("stress-tests/data-structures/LiChao.cpp", sel.tests)

    def test_transitive_include_rmq_via_lca(self):
        sel = select(REPO_ROOT, ["content/data-structures/RMQ.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertIn("stress-tests/data-structures/RMQ.cpp", sel.tests)
        self.assertIn("stress-tests/graph/LCA.cpp", sel.tests)
        self.assertNotIn("stress-tests/graph/2sat.cpp", sel.tests)

    def test_shared_header_selects_geometry_not_unrelated(self):
        sel = select(REPO_ROOT, ["content/geometry/Point.h"])
        self.assertEqual(sel.mode, "subset")
        geo = [t for t in sel.tests if t.startswith("stress-tests/geometry/")]
        self.assertGreaterEqual(len(geo), 10)
        self.assertNotIn("stress-tests/graph/2sat.cpp", sel.tests)
        self.assertNotIn("stress-tests/various/LIS.cpp", sel.tests)

    def test_pasted_algorithm_uses_path_convention(self):
        # SolveLinear.cpp copies the snippet and does not #include it.
        sel = select(REPO_ROOT, ["content/numerical/SolveLinear.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertIn("stress-tests/numerical/SolveLinear.cpp", sel.tests)
        self.assertNotIn("stress-tests/graph/2sat.cpp", sel.tests)

    def test_case_insensitive_stem_inside_polygon(self):
        sel = select(REPO_ROOT, ["content/geometry/InsidePolygon.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertIn("stress-tests/geometry/insidePolygon.cpp", sel.tests)

    def test_docs_only_skips(self):
        sel = select(REPO_ROOT, ["README.md", "agents/verify.md", "web/README.md"])
        self.assertEqual(sel.mode, "skip")
        self.assertEqual(sel.tests, [])

    def test_empty_change_skips(self):
        sel = select(REPO_ROOT, [])
        self.assertEqual(sel.mode, "skip")
        self.assertEqual(sel.tests, [])

    def test_infra_makefile_runs_all(self):
        sel = select(REPO_ROOT, ["Makefile"])
        self.assertEqual(sel.mode, "all")
        self.assertEqual(sel.tests, list_tests(REPO_ROOT))

    def test_workflow_dotfile_is_infra(self):
        # lstrip("./") would turn this into github/workflows/... and skip tests.
        sel = select(REPO_ROOT, [".github/workflows/ccpp.yml"])
        self.assertEqual(sel.mode, "all")
        self.assertIn(".github/workflows/ccpp.yml", sel.reason)

    def test_test_file_itself(self):
        sel = select(REPO_ROOT, ["stress-tests/graph/2sat.cpp"])
        self.assertEqual(sel.mode, "subset")
        self.assertEqual(sel.tests, ["stress-tests/graph/2sat.cpp"])

    def test_mixed_docs_and_header(self):
        sel = select(REPO_ROOT, ["README.md", "content/graph/2sat.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertEqual(sel.tests, ["stress-tests/graph/2sat.cpp"])

    def test_template_utility_selects_tests_that_include_it(self):
        sel = select(REPO_ROOT, ["stress-tests/utilities/template.h"])
        self.assertEqual(sel.mode, "subset")
        self.assertGreater(len(sel.tests), 50)
        self.assertIn("stress-tests/graph/2sat.cpp", sel.tests)
        # A few tests include <bits/stdc++.h> instead of template.h.
        self.assertLess(len(sel.tests), len(list_tests(REPO_ROOT)))

    def test_new_unreadable_header_without_test_skips(self):
        sel = select(REPO_ROOT, ["content/various/DoesNotExist.h"])
        self.assertEqual(sel.mode, "skip")


class TestSelectCli(unittest.TestCase):
    def _run(self, *args: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            ["python3", "-m", "tools.stress_select", "--repo", str(REPO_ROOT), *args],
            cwd=REPO_ROOT,
            check=False,
            capture_output=True,
            text=True,
        )

    def test_mode_line_subset(self):
        proc = self._run("--mode-line", "--changed", "content/graph/2sat.h")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        lines = proc.stdout.splitlines()
        self.assertEqual(lines[0], "subset")
        self.assertEqual(lines[1:], ["stress-tests/graph/2sat.cpp"])

    def test_mode_line_skip(self):
        proc = self._run("--mode-line", "--changed", "README.md")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(proc.stdout.splitlines(), ["skip"])

    def test_mode_line_all(self):
        proc = self._run("--mode-line", "--all")
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertEqual(proc.stdout.splitlines(), ["all"])
