all : gateserver

gateserver : gateserver.h gateserver.cpp main.cpp
	g++ -o $@ $^

.PHONY:clean

clean:
	rm -f gateserver*.o
