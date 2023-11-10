all:
	g++ -O3 -o pi.out pi.cpp -lpthread

debug:
	g++ -Wall -Wextra -g3 -fsanitize=address,undefined -o pi.out pi.cpp -lpthread
