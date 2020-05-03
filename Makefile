# Makefile 
CXX = g++
CXXFLAGS = -O3 -std=c++11

PGSerial:
	$(CXX) Serial/PageRank.cpp $(CXXFLAGS) -o Serial.o


# C++ MapReduce Library Includes
CPPLIB = -pthread /usr/lib/x86_64-linux-gnu/libboost_system.a /usr/lib/x86_64-linux-gnu/libboost_iostreams.a /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
PGC++:
	$(CXX) C++/PageRank.cpp $(CPPLIB) $(CXXFLAGS) -o C++.o


# MPI Sandia Map Reduce Library Includes 
MPICXX = mpic++
MPIDIR = MPI\ Base/mrmpi-7Apr14/src
USRLIB = -I $(MPIDIR) $(MPIDIR)/libmrmpi_mpicc.a
PGMPIBase:
	$(MPICXX) MPI\ Base/PageRank.cpp $(USRLIB) $(CXXFLAGS) -o MPIBase.o

# MPI Base -- Not yet Implemented
PGMPISelf:
	$(MPICXX) MPI\ Self/PageRank.cpp $(CXXFLAGS) -o MPISelf.o