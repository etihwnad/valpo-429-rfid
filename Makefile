

doc=specifications


default: $(doc).pdf

.SECONDARY:

RSTOPTS=--time --date --generator --documentoptions="letterpaper,12pt"
RSTOPTS+=--no-section-numbering
RSTOPTS+=--toc-entry-backlinks

%.tex: %.rst
	rst2latex $(RSTOPTS) $^ > $@

%.pdf: %.tex
	rubber --pdf $?

