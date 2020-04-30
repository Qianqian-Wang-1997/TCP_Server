all: client.o runClient.o
	g++ client.o runClient.o -o client -lpthread -std=c++11

client.o: client.cpp
	g++ client.cpp -c -lpthread -std=c++11

runClient.o: runClient.cpp
	g++ runClient.cpp -c -lpthread -std=c++11

clean:
	rm -rf *.o