all : gateserver

gateserver : user_list.h user_list.cpp chat_room.h chat_room.cpp room_manager.h room_manager.cpp gate_server.h gate_server.cpp server_main.cpp 
	g++ -g -o $@ $^ -levent

.PHONY:clean

clean:
	rm -f gateserver*.o
