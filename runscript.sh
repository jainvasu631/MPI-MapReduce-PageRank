#!/bin/bash
# ./C++.o Tests/barabasi-10000.txt >>output.txt
while IFS="" read -r p || [ -n "$p" ]
do
	./C++.o "Tests/$p.txt" >> "C++.txt"
	echo "mpirun -np 1 MPIBase.o \"Tests/$p.txt\" >> \"MPI Base.txt\"" >> temp.sh
	echo "mpirun -np 1 MPISelf.o \"Tests/$p.txt\" >> \"MPI Self.txt\"" >> temp.sh
done < Tests/all-tests.txt
./temp.sh
rm -rf temp.sh