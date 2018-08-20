rm input.txt
gcc -std=c99 random.c -o random
gcc -fopenmp -std=c99 edit_distance_parallel.c -o serial
./random $1
./serial $1 
