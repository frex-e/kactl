LATEXCMD = pdflatex -shell-escape -output-directory build/
export TEXINPUTS=.:content/tex/:
export max_print_line = 1048576

help:
	@echo "This makefile builds KACTL (KTH Algorithm Competition Template Library)"
	@echo ""
	@echo "Available commands are:"
	@echo "	make fast		- to build KACTL, quickly (only runs LaTeX once)"
	@echo "	make kactl		- to build KACTL"
	@echo "	make web-pdf		- to build KACTL for the snippets site (no test-session)"
	@echo "	make clean		- to clean up the build process"
	@echo "	make veryclean		- to clean up and remove kactl.pdf"
	@echo "	make test		- to run all the stress tests in stress-tests/"
	@echo "	make test-compiles	- to test compiling all headers"
	@echo "	make help		- to show this information"
	@echo "	make showexcluded	- to show files that are not included in the doc"
	@echo ""
	@echo "For more information see the file 'doc/README'"

# Repo-root copy plus the file GitHub Pages serves at /kactl.pdf.
INSTALL_PDF = cp build/kactl.pdf kactl.pdf && mkdir -p web/public && cp build/kactl.pdf web/public/kactl.pdf

fast: | build
	$(LATEXCMD) content/kactl.tex </dev/null
	$(INSTALL_PDF)

kactl: test-session.pdf | build
	$(LATEXCMD) content/kactl.tex && $(LATEXCMD) content/kactl.tex
	$(INSTALL_PDF)

web-pdf: | build
	$(LATEXCMD) content/kactl.tex && $(LATEXCMD) content/kactl.tex
	$(INSTALL_PDF)

clean:
	cd build && rm -f kactl.aux kactl.log kactl.tmp kactl.toc kactl.pdf kactl.ptc

veryclean: clean
	rm -f kactl.pdf test-session.pdf web/public/kactl.pdf

.PHONY: help fast kactl web-pdf clean veryclean

build:
	mkdir -p build/

test:
	./doc/scripts/run-all.sh .

test-compiles:
	./doc/scripts/compile-all.sh .

test-session.pdf: content/test-session/test-session.tex content/test-session/chapter.tex | build
	$(LATEXCMD) content/test-session/test-session.tex
	cp build/test-session.pdf test-session.pdf

showexcluded: build
	grep -RoPh '^\s*\\kactlimport{\K.*' content/ | sed 's/.$$//' > build/headers_included
	find ./content -name "*.h" -o -name "*.py" -o -name "*.java" | grep -vFf build/headers_included
