.PHONY: all clean

hello:
	g++ ./hello.cpp -lrf24 -lpthread -g
	sudo ./a.out

bench:
	g++ ./benchmark.cpp -lrf24 -lpthread -g
	sudo ./a.out

r: 
	g++ ./reciver.cpp -lrf24 -lpthread -g
	sudo ./a.out

t:
	g++ ./transmitter.cpp -lrf24 -lpthread -g
	sudo ./a.out

gettings:
	g++ ./getting_started.cpp -lrf24 -lpthread -g
	sudo ./a.out

tun:
	g++ ./tun_test.cpp -g
	sudo ./a.out	

clean:
	rm a.out