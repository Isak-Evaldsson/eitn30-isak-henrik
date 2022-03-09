.PHONY: all mobileUnit baseStation

mobileUnit:
	g++ src/mobileUnit.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp -lpthread -lrf24 -g
	sudo ./a.out

baseStation:
	g++ src/baseStation.cpp src/transmittBuffer.cpp src/tun.cpp src/fragmentBuffer.cpp -lpthread -lrf24 -g
	sudo ./a.out

measure_client:
	python measure_client.py 1000

measure_server:
	python measure_server.py

clean:
	rm a.out
