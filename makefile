.PHONY: all mobileUnit baseStation

mobileUnit:
	g++ src/mobileUnit.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp src/frames.cpp -lpthread -lrf24 -g
	sudo gdb -ex run ./a.out

baseStation:
	g++ src/baseStation.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp src/frames.cpp -lpthread -lrf24 -g
	sudo gdb -ex run ./a.out

clean:
	rm a.out
