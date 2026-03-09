g++ -E STL1Iter15.cpp -o STL1Iter15.i

g++ -E -P STL1Iter15.cpp -o STL1Iter15.i

g++ -S STL1Iter15.i

g++ -с STL1Iter15.s

g++ -E main.cpp -o main.i

g++ -E -P main.cpp -o main.i

g++ -S main.i

g++ -c main.s

g++ STL1Iter15.o main.o -o myprogram
