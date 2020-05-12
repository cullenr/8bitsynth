#!/usr/bin/env bash

prog='set terminal pngcairo size 800,600; set logscale y; plot "-" using 1:($2 > 15000 ? $2 : 0) with impulses lw 2'

sox -t raw -r 8k -e unsigned -b 8 -c 1 <(./multi-osc -s -f110) -n stat -freq 2>&1 | head -n 4000 | gnuplot -e "$prog" > sin110.png
sox -t raw -r 8k -e unsigned -b 8 -c 1 <(./multi-osc -q -f110) -n stat -freq 2>&1 | head -n 4000 | gnuplot -e "$prog" > sqr110.png
sox -t raw -r 8k -e unsigned -b 8 -c 1 <(./multi-osc -t -f110) -n stat -freq 2>&1 | head -n 4000 | gnuplot -e "$prog" > tri110.png
sox -t raw -r 8k -e unsigned -b 8 -c 1 <(./multi-osc -w -f110) -n stat -freq 2>&1 | head -n 4000 | gnuplot -e "$prog" > saw110.png
