

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


.PHONY: clean fig

.SECONDARY:

all: pdf html

pdf: $(pdf)

html: $(html) fig

gitversion.txt: $(src)
	@echo "re-creating gitversion.txt"
	@echo ".. |date| date:: %Y-%m-%d %H:%M" > $@
	@echo "" >> $@
	@echo ".. |version| replace:: $$(git describe --tags --long)" >> $@
	@echo "" >> $@

fig:
	$(MAKE) -C fig

%.tex: %.rst gitversion.txt
	rst2latex $(RST2TEXOPTS) $< > $@
	@#remove image extension to use better version (pdf) if available
	perl -p -i -e 's/\.png//' $@

%.pdf: %.tex fig
	rubber --pdf $<
	rubber --clean $<

%.html: %.rst gitversion.txt
	rst2html $(RST2HTMLOPTS) $< > $@

clean:
	rubber --clean $(tex)

web: $(html) $(pdf)
	scp $^ dan@tesla.whiteaudio.com:/var/www/www.agnd.net/valpo/429/
	scp -r fig  dan@tesla.whiteaudio.com:/var/www/www.agnd.net/valpo/429/
	cp specifications.pdf "/home/dan/insync-valpo/ECE429/429 final project/"

