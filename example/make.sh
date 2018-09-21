mpicc maitre.c -o maitre
mpicc esclave.c -o esclave
mpicc coordinateur.c -o coordinateur
mpirun -np 1 --hostfile hostfile ./maitre 3
