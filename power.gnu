set terminal pngcairo size 800,600; 

set xlabel 'frequency hz'
set ylabel 'power'

set logscale y; 

set yrange [ARG2:100000]

plot ARG1 using 1:($2 > ARG2 ? $2 : 0) title ARG1 with impulses lw 2
