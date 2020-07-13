
.PHONY: all

all: index.html oscillators.html

%.html: %.md
	pandoc --filter pandoc-plot --filter pandoc-crossref -s -c pandoc.css $< > $@

