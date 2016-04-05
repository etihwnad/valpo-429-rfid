

pdf=$(wildcard *.pdf)

# convert -trim -units PixelsPerInch -density 150 in.pdf out.png


%.png: %.pdf
	convert -trim -units PixelsPerInch -density 150 $< $@
