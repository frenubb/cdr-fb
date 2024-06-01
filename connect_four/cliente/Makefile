CXX = g++
CXXFLAGS = -std=c++11

all: cliente

cliente: cliente.o
	$(CXX) $(CXXFLAGS) -o cliente cliente.o

cliente.o: cliente.cpp
	$(CXX) $(CXXFLAGS) -c cliente.cpp

clean:
	rm-f cliente cliente.o
