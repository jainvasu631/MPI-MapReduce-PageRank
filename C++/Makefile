# Makefile 
CXX = g++
CXXFLAGS = -pthread -Ofast -std=c++17
BOOSTDIR = /usr/lib/x86_64-linux-gnu/
LIBS = -I$(BOOSTDIR) -L$(BOOSTDIR) -lboost_system -lboost_iostreams -lboost_filesystem

PageRank:	
	$(CXX) PageRank.cpp $(LIBS) $(CXXFLAGS) -o PageRank.o
	