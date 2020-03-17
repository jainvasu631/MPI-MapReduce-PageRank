# Makefile 
CXX = g++
CXXFLAGS = -Ofast
SERIAL = -pthread /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a

PGSerial:
	$(CXX) Serial/PageRank.cpp $(CXXFLAGS) -o Serial.o

PGC++:
	$(CXX) C++/PageRank.cpp $(SERIAL) $(CXXFLAGS) -o C++.o

PGMPIBase:
	$(CXX) MPI\ Base/PageRank.cpp $(CXXFLAGS) -o MPIBase.o

PGMPISelf:
	$(CXX) MPI\ Self/PageRank.cpp $(CXXFLAGS) -o MPISelf.o