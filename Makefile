

doc=specifications


default: $(doc).pdf

.SECONDARY:

RSTOPTS=--generator --documentoptions="letterpaper,12pt"

%.tex: %.rst
	rst2latex $(RSTOPTS) $^ > $@

%.pdf: %.tex
	rubber --pdf $?

