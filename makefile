all: server.o runServer.o
	g++ server.o runServer.o -o run -lpthread -std=c++11; ./run 8888

server.o: server.cpp
	g++ server.cpp -c -lpthread -std=c++11

runServer.o: runServer.cpp
	g++ runServer.cpp -c -lpthread -std=c++11

clean:
	rm -rf *.o
