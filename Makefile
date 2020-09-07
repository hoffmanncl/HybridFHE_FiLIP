all : filip

filip : src/filip/file_management.cpp src/filip/filip_user.cpp src/filip/filip_server.cpp src/filip/aes.c src/filip/tgsw_operations.cpp src/filip/main.cpp src/filip/filip.cpp src/filip/tlwe_filip_server.cpp
	g++ -I./src/ -o filip src/filip/main.cpp src/filip/file_management.cpp src/filip/filip.cpp src/filip/filip_user.cpp src/filip/filip_server.cpp src/filip/aes.c src/filip/tgsw_operations.cpp src/filip/tlwe_filip_server.cpp -ltfhe-spqlios-fma

run : filip
	./filip

test : src/filip/tgsw_operations.cpp src/test/test-tgsw.cpp
	g++ -I./src/ -o test src/test/test-tgsw.cpp src/filip/tgsw_operations.cpp -ltfhe-spqlios-fma

clean :
	rm -f filip test

fullclean : clean
	rm -r filip_keys/*
