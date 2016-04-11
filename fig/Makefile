

svg=$(wildcard *.svg)

pdf=$(wildcard *.pdf)

# convert -trim -units PixelsPerInch -density 150 in.pdf out.png

all: png pdf

# PDF files whose source is SVG
pdf: $(patsubst %.svg,%.pdf,$(svg))

%.pdf: %.svg
	inkscape -f $< --export-pdf=$@

png: $(patsubst %.pdf,%.png,$(pdf)) $(patsubst %.svg,%.png,$(svg))


%.png: %.pdf
	convert -trim -units PixelsPerInch -density 300 $< $@
