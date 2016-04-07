

pdf=$(wildcard *.pdf)

# convert -trim -units PixelsPerInch -density 150 in.pdf out.png

all: png

png: $(patsubst %.pdf,%.png,$(pdf))


%.png: %.pdf
	convert -trim -units PixelsPerInch -density 300 $< $@
