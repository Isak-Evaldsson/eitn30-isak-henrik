.PHONY: all mobileUnit baseStation

mobileUnit:
	g++ src/mobileUnit.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp src/frames.cpp -lpthread -lrf24 -g
	sudo ./a.out

baseStation:
	./scripts/iptables_bs.sh
	g++ src/baseStation.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp src/frames.cpp -lpthread -lrf24 -g
	sudo ./a.out

clean:
	rm a.out
