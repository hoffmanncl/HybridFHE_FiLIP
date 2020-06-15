all : filip

filip : src/filip/filip.cpp src/filip/tgsw-operations.cpp src/filip/main.cpp
	g++ -I./src/ -o filip src/filip/main.cpp src/filip/filip.cpp src/filip/tgsw-operations.cpp -ltfhe-spqlios-fma

run : filip
	./filip

test : src/filip/tgsw-operations.cpp src/test/test-tgsw.cpp
	g++ -I./src/ -o test src/test/test-tgsw.cpp src/filip/tgsw-operations.cpp -ltfhe-spqlios-fma

clean :
	rm -f filip
