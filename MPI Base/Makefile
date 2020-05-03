# Makefile
CXX =		mpic++
CXXFLAGS =	-O3  -std=c++11
MPIDIR = mrmpi-7Apr14/src
USRLIB =	-I $(MPIDIR) $(MPIDIR)/libmrmpi_mpicc.a

PageRank:
	$(CXX) PageRank.cpp $(USRLIB) $(CXXFLAGS) -o PageRank.o