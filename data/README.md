# Introduction of Data Input

Sample graph file content:

```
7	8
0	1	1.0
0	2	1.0
0	6	1.0
1	2	1.0
2	3	1.0
3	4	1.0
3	5	1.0
4	6	1.0
```

The size of node and edge shall be given in the first line, and then each line gives one edge with weight.


Path of graph file

```
.
├── data
│   └── sample.graph
└── panther
```

Run command:

```
./panther sample 2 5 0.01
```


