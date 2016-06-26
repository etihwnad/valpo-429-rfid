

VWFfiles=$(wildcard *.vwf)
PWLfiles=$(VWFfiles:.vwf=.pwl)

all: $(PWLfiles)

%.pwl: %.vwf
	vwf2pwl.py $<
