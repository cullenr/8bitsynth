#!/usr/bin/env gnuplot

set terminal svg size 800,600 dynamic lw 2 font "sans,16"

set xlabel 'phase ϕ'
set ylabel 'amplitude A'

set xrange [0:2*pi]
set yrange [-1.5:1.5]

set xtics (0, 'π' pi, '2π' 2*pi)
set ytics 1

_sin(x) = sin(x)
_saw(x) = 1 - x / pi
_sqr(x) = x >= pi ? 1 : -1
_tri(x) = x <= pi ? -1 + x * 2 / pi : 3 - x * 2 / pi

# Plot
plot _sin(x) title 'sin' with lines, \
     _saw(x) title 'saw' with lines, \
     _sqr(x) title 'square' with lines, \
     _tri(x) title 'triangle' with lines 
