clang++ -E -P STL1Iter15.cpp > STL1Iter15.i

clang++ -c STL1Iter15.i

clang++ -x c++-cpp-output -c STL1Iter15.i -o STL1Iter15.o

clang++ -E -P main.cpp > main.i

clang++ -c main.i

clang++ -x c++-cpp-output -c main.i -o main.o

clang++ STL1Iter15.o main.o -o STL1Iter15.exe

(Использую, т.к. выдавало ошибки при ассемблирование)

clang++ -c -Wno-deprecated -Wno-invalid-constexpr main.i

clang++ -c -Wno-deprecated -Wno-invalid-constexpr STL1Iter15.i