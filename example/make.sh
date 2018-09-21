mpicc jalon1.c -o jalon1
mpicc prg1.c -o prg1
mpicc coordinateur.c -o coordinateur
mpirun -np 1 --hostfile hostfile ./jalon1 3
