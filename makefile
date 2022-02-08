.PHONY: all clean

all:
	g++ src/hello.cpp -lrf24 -lpthread -g
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

tun:
	g++ src/tun.cpp -g
	sudo ./a.out	

mobileUnit:
	g++ src/mobileUnit.cpp src/transmittBuffer.cpp src/tunMobileUnit.cpp -lpthread -lrf24 -g
	sudo ./a.out

baseStation:
	g++ src/baseStation.cpp src/transmittBuffer.cpp src/tunMobileUnit.cpp -lpthread -lrf24 -g
	sudo ./a.out

clean:
	rm a.out
