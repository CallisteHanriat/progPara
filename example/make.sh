mpicc maitre.c -o maitre
mpicc esclave.c -o esclave
mpicc coordinateur.c -o coordinateur `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -lcairo
mpirun -np 1 --hostfile hostfile ./maitre 3
