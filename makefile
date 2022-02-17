.PHONY: all mobileUnit baseStation

mobileUnit:
	g++ src/mobileUnit.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp -lpthread -lrf24 -g
	sudo ./a.out

baseStation:
	g++ src/baseStation.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp -lpthread -lrf24 -g
	sudo ./a.out

clean:
	rm a.out
