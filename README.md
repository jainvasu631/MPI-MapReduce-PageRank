MPI-MapReduce-PageRank
====================

Implementation
--------------

PageRank algorithm implemented using MapReduce libraries using MPI.

The PageRank Algorithm
---------------------

We will assign to each web page P a measure of its importance I(P), called the page's PageRank.

Suppose that page Pj has lj links. If one of those links is to page Pi, then Pj will pass on 1/lj of its importance to
Pi. The importance ranking of Pi is then the sum of all the contributions made by pages linking to it. That is, if we
denote the set of pages linking to Pi by Bi, then I(P_i)=\sum_{P_j\in B_i} \frac{I(P_j)}{l_j}  

**Hyperlink Matrix**
H := [Hij] = {if Pj in Bi then 1/lj else 0} H>=0  
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

1. Replace all 0 columns in matrix H with 1/n to create S.  S = H + A where A := [Aij] = {if Pj in Bi then 0 else 1/n}
2. Choose a parameter alpha between 0 and 1.  G := alpha x S + (1-alpha) x 1/n x 1.

**Choosing alpha**
The parameter alpha has an important role. When alpha is 1 we get G:=S and when alpha is 0 G:=(1-alpha)/n x 1.  
The rate of convergence of I depends on alpha. Therefore as a compromise we use alpha = 1.

-----
**Final Formula**
I(k+1) = alphaHI(k) + alphaAI(k) + (1-alpha)/n1I(k).
