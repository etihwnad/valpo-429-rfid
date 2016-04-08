

src=$(wildcard *.rst)


RST2TEXOPTS=--time --date --generator --documentoptions="letterpaper,12pt"
RST2TEXOPTS+=--no-section-numbering
RST2TEXOPTS+=--toc-entry-backlinks
RST2TEXOPTS+=--table-style=booktabs

tex=$(patsubst %.rst,%.tex,$(src))
pdf=$(patsubst %.rst,%.pdf,$(src))
html=$(patsubst %.rst,%.html,$(src))


SHELL=/bin/bash


.SECONDARY:

all: pdf html

pdf: $(pdf)

html: $(html)

gitversion.txt: $(src)
	@echo "re-creating gitversion.txt"
	@echo ".. |date| date:: %Y-%m-%d %H:%M" > $@
	@echo "" >> $@
	@echo ".. |version| replace:: $$(git describe --tags --long --dirty=-**dirty**)" >> $@
	@echo "" >> $@

%.tex: %.rst gitversion.txt
	rst2latex $(RST2TEXOPTS) $< > $@

%.pdf: %.tex
	rubber --pdf $<
	rubber --clean $<

%.html: %.rst gitversion.txt
	rst2html $(RST2HTMLOPTS) $< > $@

.PHONY: clean
clean:
	rubber --clean $(tex)
