.PHONY: all clean

all:
	g++ src/transmitter.cpp -lrf24 -lpthread -g
	sudo ./a.out

bench:
	g++ src/benchmark.cpp -lrf24 -lpthread -g
	sudo ./a.out

r: 
	g++ src/reciver.cpp -lrf24 -lpthread -g
	sudo ./a.out

t:
	g++ src/transmitter.cpp -lrf24 -lpthread -g
	sudo ./a.out

gettings:
	g++ src/getting_started.cpp -lrf24 -lpthread -g
	sudo ./a.out

clean:
	rm a.out
