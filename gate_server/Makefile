all : gateserver

gateserver : chat_room.h chat_room.cpp room_manager.h room_manager.cpp gate_server.h gate_server.cpp server_main.cpp
	g++ -o $@ $^

.PHONY:clean

clean:
	rm -f gateserver*.o
