# !/bin/bash
gcc -w -g server.c -o server.out
gcc -w -g client.c -o client.out
./server.out &
sleep 1
./client.out &
sleep 2
pkill server.out

