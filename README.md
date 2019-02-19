# Algorithm 2015 Final Project Team24

## Synopsis

[ICCAD 2015 Contest - Color Balancing for Double Patterning](http://cad-contest.el.cycu.edu.tw/problem_E/default.htm)

This is a quick greedy solver to solve the "ICCAD 2015 Contest Color Balancing for Double Patterning" problem through graphlab library. The problem is: given multiple graphs, decide whether they are 2-colorable; then for those each of 2-colorable graphs, allocate a 2-color schema for it so that, overally, the two colors are assigned more __uniformly__ within a containing bounding box. 

Our greedy solver solves this problem in O(NlogN) where N is rougly the total number of the nodes from all graphs. 

## Architecture

```
bin/                        Execution files
lib/                        C/C++ Libraries used
inputs/                     Officail test cases
outputs/                    Corresponding outputs
src/                        Main source files
    Makefile                    Makefile
    graph.cpp                   All member functions
    graph.h                     Its header               
    GDalgorithm.cpp             Main file for our Greedy algorithm
    main.cpp                    Main function, entry point
    shape.cpp                   Helper file
    shape.h                     Its header
    sys_cmd.cpp                 Stale file
    sys_cmd.h                   Stale file
    user_cmd.cpp                Stale file
    user_cmd.h                  Stale file
doc/                        Documents
Makefile                    Makefile for all
README.md                   Readme file
```

## Installation
Unix-like environment:
```
$make clean 
$make
```

## Execution
Unix-like environment:
```
$./bin/DPT_balance_color ./inputs/your_input ./outputs/no_output_implemented

# e.g. $./bin/DPT_balance_color ./inputs/iccad2015_input.case1 ./outputs/no_output_implemented
```

## Contributors
Po-Kai Chang [LplusKira](https://github.com/LplusKira)

Chien-Yu

陳則彣 

黃瑞諭
