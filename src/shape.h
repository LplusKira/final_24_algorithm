// **************************************************************************
//  File       [shape.h]
//  Author     [Chien Yu][Po-Kai Chang]
//  Synopsis   [The header file of shape]
//  Modify     [2015/05/23 Chien Yu][2015/05/24 Po-Kai Chang]
// **************************************************************************

#ifndef _SHAPE_H
#define _SHAPE_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int _BUFFSIZE_ = 40;
const int _sameParsingNum_ = 6;

using namespace std;

class Shape {
    public:
	    int x[2], y[2];
	    int color;
	    int group_index;
//    private:
};

class ProtocolAndGroup
{
	public:
		int* protocal_list;
		int** Group;
};

class Classifier
{
	public:
		Classifier();
		ProtocolAndGroup classify(int arg_num, char* argvector[]);
		void help_you_ending(int total_group_Num);
		void help_you_class(int group_Num, int first_node, int second_node, int first_time);
		Shape* Data;
};

#endif
