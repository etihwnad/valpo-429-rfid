

src=$(wildcard *.rst)


RST2TEXOPTS=--time --date --generator --documentoptions="letterpaper,12pt"
RST2TEXOPTS+=--no-section-numbering
RST2TEXOPTS+=--toc-entry-backlinks
RST2TEXOPTS+=--table-style=booktabs
RST2TEXOPTS+=--stylesheet=lmodern

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
	@echo ".. |version| replace:: $$(git describe --tags --long)" >> $@
	@echo "" >> $@

%.tex: %.rst gitversion.txt
	rst2latex $(RST2TEXOPTS) $< > $@
	@#remove image extension to use better version (pdf) if available
	perl -p -i -e 's/\.png//' $@

%.pdf: %.tex
	rubber --pdf $<
	rubber --clean $<

%.html: %.rst gitversion.txt
	rst2html $(RST2HTMLOPTS) $< > $@

.PHONY: clean
clean:
	rubber --clean $(tex)

web: $(html) $(pdf)
	scp $^ dan@tesla.whiteaudio.com:/var/www/www.agnd.net/valpo/429/

