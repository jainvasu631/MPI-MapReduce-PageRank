# Makefile 
CXX = g++
CXXFLAGS = -pthread -Ofast
LIBS = /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a

PageRank:
	$(CXX) PageRank.cpp $(LIBS) $(CXXFLAGS) -o PageRank.o
	