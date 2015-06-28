// **************************************************************************
// File       [ test_cmd.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2016/06/27 modified ]
// **************************************************************************

#ifndef _TEST_CMD_H_
#define _TEST_CMD_H_

#include "../lib/cmd.h"
#include "shape.h"

const int _ParsingNum_ = 6;
const int _theSameBuffsize_ = 40;
const int _MaxNum_ = 2147483647;
const int _MinNum_ = -2147463646;

const int _type1_  = 1;
const int _type2_  = 2;
const int _type3_  = 3;
const int _type4_  = 4;


class Color_2coloring
{
	public:
		Color_2coloring();

		int* findBoundingBox(int omega, Shape* Data, int dataSize, int* legalGPNum);
		int** CalGrid(int* boxbounding, int omega);
		int* exec(int argc, char argv[_ParsingNum_][_theSameBuffsize_], Shape* Data, int omega, int dataSize);
		int* calDensity(int* boxbounding, int omega, int* whichColor);

};

#endif
