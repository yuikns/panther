Panther: Fast Top-k Similarity Search in Large Networks
======================================

## Introduction

Estimating similarity between vertices is a fundamental issue in network analysis across various domains, such as social networks and biological networks. Methods based on common neighbors and structural contexts have received much attention. However, both categories of methods are difficult to scale up to handle large networks (with billions of nodes). In this paper, we propose a sampling method that provably and accurately estimates the similarity between vertices. The algorithm is based on a novel idea of random path, and an extended method is also presented, to enhance the structural similarity when two vertices are completely disconnected. We provide theoretical proofs for the error-bound and confidence of the proposed algorithm. We perform extensive empirical study and show that our algorithm can obtain top-k similar vertices for any vertex in a network approximately 300x faster than state-of-the-art methods. We also use identity resolution and structural hole spanner finding, two important applications in social networks, to evaluate the accuracy of the estimated similarities. Our experimental results demonstrate that the proposed algorithm achieves clearly better performance than several alternative methods.

For more detail of this paper, please visit [this page](http://arxiv.org/abs/1504.02577).

This project is just a simple implementation of this problem.

## Requirement

### Compile Tool Chain

+ cmake 2.8.8+
+ gcc 4.8.2+ or clang 3.3+
+ git

### System Dependence Libraries

+ pthread

*network is necessary when you compile this project.*

## Usage 
````
$ mkdir build
$ cd build
$ cmake ..
$ make
$ # copy your data files into “data” folder
$ ./panther data T D epsilon [thread_number]
````

## Graph file

Please check [this url](https://aminer.org/billboard/Panther) for more detail.


