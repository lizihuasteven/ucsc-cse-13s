#!/bin/bash


make clean && make monte_carlo
./monte_carlo -n 20000 > /tmp/monte_carlo.dat
sleep 1
make clean && make monte_carlo
./monte_carlo -n 20000 > /tmp/monte_carlo_2.dat
sleep 1
make clean && make monte_carlo
./monte_carlo -n 20000 > /tmp/monte_carlo_3.dat
sleep 1
make clean && make monte_carlo
./monte_carlo -n 20000 > /tmp/monte_carlo_4.dat
sleep 1
make clean && make monte_carlo
./monte_carlo -n 20000 > /tmp/monte_carlo_5.dat
sleep 1
./monte_carlo -n 1000 > /tmp/monte_carlo_6.dat

gnuplot <<END
    set terminal pdf
    set output "monte_carlo.pdf"
    set size square 1,1
    set xlabel 'x'
    set ylabel 'y'
    set title 'Plot 1'
    set xrange[0:1]
    set yrange[0:1]
    set style lines 1 lc rgb 'blue' pt 7 ps 0.25
    set style lines 2 lc rgb 'red' pt 7 ps 0.25
    f(x) = sqrt(1**2 - x**2)
    plot f(x) lw 3 t "", \
    	 "/tmp/monte_carlo_6.dat" u (\$3):(\$5 == 0 ? 1/0 :(\$4)) w p ls 1 t '', \
    	 "/tmp/monte_carlo_6.dat" u (\$3):(\$5 == 1 ? 1/0 :(\$4)) w p ls 2 t ''
    
    set title "Monte Carlo Error Estimation"
    set ylabel "Error"
    set yrange [-1:1]
    set xrange [1:20000]
    set zeroaxis
    set log x 4
    set size ratio 0.7
    set style fill transparent solid 0.2 noborder
    plot "/tmp/monte_carlo.dat" u (\$1):(\$2 - pi) w l title "", \
         "/tmp/monte_carlo_2.dat" u (\$1):(\$2 - pi) w l title "", \
         "/tmp/monte_carlo_3.dat" u (\$1):(\$2 - pi) w l title "", \
	 "/tmp/monte_carlo_4.dat" u (\$1):(\$2 - pi) w l title "", \
	 "/tmp/monte_carlo_5.dat" u (\$1):(\$2 - pi) w l title ""
    
END
