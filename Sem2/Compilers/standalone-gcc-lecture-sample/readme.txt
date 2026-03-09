g++ -E function.cpp -o function.i

g++ -E -P function.cpp -o function.i

g++ -S function.i

g++ -c function.s

g++ function.o main.o -o myprogram

g++ -E main.cpp -o main.i

g++ -E -P main.cpp -o main.i

g++ -S main.i

g++ -c main.s

g++ function.o main.o -o myprogram