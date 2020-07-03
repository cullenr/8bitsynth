
.PHONY: all

all: index.html oscillators.html

%.html: %.md
	pandoc -s -c pandoc.css $< > $@
