#!/bin/bash
# ./C++.o Tests/barabasi-10000.txt >>output.txt
while IFS="" read -r p || [ -n "$p" ]
do
	./C++.o "Tests/$p.txt" >> "C++.txt"
	mpirun -np 1 MPIBase.o "$p.txt" >> "MPI Base.txt"
	mpirun -np 1 MPISelf.o "$p.txt" >> "MPI Self.txt"
done < Tests/all-tests.txt