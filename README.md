MPI-MapReduce-PageRank
====================

Implementation
--------------

**Compilation Step** - `g++ -std=c++11 -Ofast PageRank.cpp -o PageRank.o`  
**Run Algorithm** - `./PageRank.o {Testfile}`
<!-- PageRank algorithm implemented using MapReduce libraries using MPI. -->

The PageRank Algorithm
---------------------

We will assign to each web page P a measure of its importance I(P), called the page's PageRank.

Suppose that page Pj has lj links. If one of those links is to page Pi, then Pj will pass on 1/lj of its importance to
Pi. The importance ranking of Pi is then the sum of all the contributions made by pages linking to it. That is, if we
denote the set of pages linking to Pi by Bi, then I(P_i)=\sum_{P_j\in B_i} \frac{I(P_j)}{l_j}  

**Hyperlink Matrix**
H := [Hij] = {if Pj in Bi then 1/lj else 0} H>=0  
H := [Hij] = {if Pj -> Pi is Edge then 1/lj else 0} H>=0  
H is a stochastic matrix. The sum of all entries in a column is 1/0

**Importance Vector**
I := [I(Pi)] whose components are the PageRanks or the importance rankings of all the pages.  
I = HI i.e. I is an eigenvector of H with eigenvalue 1. This is the stationary vector of H

**Computing I**
H is very very large matrix of the order of 10e10. However most entries of H are zero.  
Therefore its a sparse matrix.  
I(k+1) = HI(k). Sequence of I(k) converge to I.

-----
**Problems with Convergence of I**:

1. Dangling Node which has no out links. H isn't perfectly stochastic.
2. Circular Reference Problem. S isn't Primitive
3. Importance Sink. S is reducible

-----
**Google Matrix**:

1. Replace all 0 columns in matrix H with 1/n to create S.  S = H + A where A := [Aij] = {if Hi==0 then 1/n else 0}
2. Choose a parameter alpha between 0 and 1.  G := alpha x S + (1-alpha) x 1/n x 1.

**Choosing alpha**
The parameter alpha has an important role. When alpha is 1 we get G:=S and when alpha is 0 G:=(1-alpha)/n x 1.  
The rate of convergence of I depends on alpha. Therefore as a compromise we use alpha = 1.

-----
**Simplifying Formula**
I(k+1) = alpha x H x I(k) + alpha x A x I(k) + (1-alpha)/n x 1 x I(k).  
This can be further simplified. A = J/n where J = [Ji] = {1 if Corresponding Column is 0 else 0}.Then,  
beta := (1/alpha-1) and now M:= (J + beta x 1)/n.  
Then I(k+1) = alpha x (H x I(k) + (J + (1/alpha-1) x 1)/n x I(k)) which simplifies to.
I(k+1) = alpha x (H x I(k) + M x I(k)).  

**Calculating M x I(k)**
As all rows of M are identical, we can write M = 1 x N and M x I(k) = 1 x N x I(k).  
factor:= N x I(k) = (1 + beta if Corresponding column is 0 else beta) x I(k)/n
Now the expression simplifies to.
I(k+1) = alpha x H x I(k) + 1 x factor.  
