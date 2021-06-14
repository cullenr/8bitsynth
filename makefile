SHELL:=/bin/bash
WAVEFORMS := sin saw sqr tri 

# file name format:
# <osc>-<shape>-<duration>ms-<frequency>hz-<bit depth>bit.<extension>
# sweep the naive osc
NAIVE_SWEEP_RAWS = $(patsubst %,naive-saw-100ms-%hz-8bit.raw,$(shell seq 40 5 1200))

NAIVE_RAWS := $(WAVEFORMS:%=naive-%-1000ms-110hz-8bit.raw) $(WAVEFORMS:%=naive-%--1000ms-880hz-8bit.raw) $(NAIVE_SWEEP_RAWS)
PROGRAMS := naive-sin naive-multi additive-osc

LDLIBS += -lm

# get the third part of the filename and strip the 'hz' suffix
get_osc_duration = $(subst ms,,$(word 3, $(subst -, ,$(1))))
get_osc_freq     = $(subst hz,,$(word 4, $(subst -, ,$(1))))
#translate the 3char wave name to a CLI flag for the oscilator executables
get_osc_flag = $(shell sed 's/.*\-sin.*/-s/;s/.*-sqr.*/-q/;s/.*\-saw.*/-w/;s/.*-tri.*/-t/' <<< $(1))

.PHONY: all
.PHONY: clean

all: html $(PROGRAMS) $(NAIVE_RAWS) audio-files power-plots

# html -------------------------------------------------------------------------
html: index.html oscillators.html

%.html: %.md
	pandoc --from markdown \
		--mathjax \
		--filter pandoc-include-code \
		--filter pandoc-plot \
		--filter pandoc-crossref \
		-s -c pandoc.css $< > $@

# videos -----------------------------------------------------------------------
naive-sweep.mp4: naive-sweep.txt naive-sweep.wav
	ffmpeg -y -f concat -i naive-sweep.txt -i naive-sweep.wav -c:v libx264 -pix_fmt yuv420p -c:a aac -b:a 256k $@

naive-sweep.wav: naive-sweep-wav.txt 
	ffmpeg -y -f concat -i naive-sweep-wav.txt $@

naive-sweep.txt: $(NAIVE_SWEEP_RAWS:.raw=.png)
	printf "$(patsubst %,file %\nduration 0.1\n,$^)" > $@

naive-sweep-wav.txt: $(NAIVE_SWEEP_RAWS:.raw=.wav)
	printf "$(patsubst %,file %\nduration 0.1\n,$^)" > $@

naive-sweep.mp3: naive-sweep.raw
	ffmpeg -f u8 -ar 8000 -ac 1 -i naive-sweep.raw -acodec libmp3lame -ar 48000 $@

naive-sweep.raw: $(NAIVE_SWEEP_RAWS)
	cat $^ > $@

# power plots ------------------------------------------------------------------
power-plots: $(NAIVE_RAWS:.raw=.png)

%-8bit.png: %-8bit.dft
	gnuplot -c power.gnu $< 1000 > $@

%-32bit.png: %-32bit.dft
	gnuplot -c power.gnu $< 100 > $@

%.dft: %.wav
	sox $< -n stat -freq 2>&1 | head -n 2000 > $@

# audio samples ----------------------------------------------------------------

audio-files: $(NAIVE_RAWS:.raw=.wav)

# target for 8bit files
%-8bit.wav: %-8bit.raw
	sox -t raw -r 8k -e unsigned -b 8 -c 1 $< $@

#target for fp files
%-32bit.wav: %-32bit.raw
	sox -t raw -r 48k -e floating-point -b 32 -c 1 $< $@

# raw files from the oscilators --------------------------------------------
$(NAIVE_RAWS): naive-multi
	./naive-multi -d $(call get_osc_duration,$@) \
		-f $(call get_osc_freq,$@) $(call get_osc_flag,$@) > $@

# c programs -------------------------------------------------------------------
$(PROGRAMS):

debug: CFLAGS += -DDEBUG -g
debug: $(PROGRAMS)

# housekeeping -----------------------------------------------------------------
# TODO : make this safer, implement a build dir and an install target to move
# the index.html
clean: 
	rm -f index.html ${PROGRAMS}
	rm -f *.raw
	rm -f *.wav
	rm -f *.dat
	rm -f *.png
	rm -f *.dft

