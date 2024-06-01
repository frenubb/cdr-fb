CXX = g++
CXXFLAGS = -pthread -std=c++11

all: servidor

servidor: servidor.o
	$(CXX) $(CXXFLAGS) -o servidor servidor.o

servidor.o: servidor.cpp
	$(CXX) $(CXXFLAGS) -c servidor.cpp

clean:
	rm -f servidor servidor.o
