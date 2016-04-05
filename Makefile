

src=$(wildcard *.rst)


RSTOPTS=--time --date --generator --documentoptions="letterpaper,12pt"
RSTOPTS+=--no-section-numbering
RSTOPTS+=--toc-entry-backlinks

tex=$(patsubst %.rst,%.tex,$(src))
pdf=$(patsubst %.rst,%.pdf,$(src))


all: $(pdf)

%.tex: %.rst
	rst2latex $(RSTOPTS) $^ > $@

%.pdf: %.tex
	rubber --pdf $<
	rubber --clean $<


.PHONY:
clean:
	rubber --clean $(tex)
