// **************************************************************************
// File       [ algorithm.cpp ]
// Author     [ Po-Kai Chang, almost all ] [ 陳則彣 : 1019~ 1072 + 1276 ~ 1315] [Chien Yu : line 1319~ 1327 and line 39]
// Synopsis   [ the part in implementing the greedy alorithm solving this problem, checking whether 2-colorable]
// Date       [ 2015/05/24 modified ]
// **************************************************************************

#include "user_cmd.h"
#include "graph.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "../lib/tm_usage.h"
#include "shape.h"

#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <algorithm>
using namespace std;
using namespace CommonNs;
const char ourgraph_name[] = "ourgraph";
const char theEnd[] = "}\n";
const char _2Color_[] = "// 2--Coloring produced by Po-Kai\n";
const char _Not2Colorable_[] = "obviously, Not_2Colorable\n";
const char _OutPutFile_[] = "../outputs/QQ";
const char _Reading_[] = "read_graph";
const int _LuckyNum_ = 1126;
const int _MaxContributor_ = 11;
int we_cant_ignore_write, we_cant_ignore_read;

vector<int> ourNodes1;
vector<int> ourNodes2;

void each_window_density (int*** Window, int width, int height);

void empty_ContributorList(int*** Contributor_list, int X_block_Num, int Y_block_Num)
{
	for(int i = 0;i<= X_block_Num; i++)
		for(int j = 0;j<= Y_block_Num; j++)
			for(int k = 0;k<_MaxContributor_;k++)
				Contributor_list[i][j][k] = 0;
	return;
}

void Add_area_perShape(vector<int>& which_window_x, vector<int>& which_window_y, vector<int>& area_sumShape_neg, vector<int>& area_sumShape_pos, int additional_area, int leftest_grid, int lowest_grid, int ind)
{
	int j, init_ind;
	for(j = 0, init_ind = 1;j<which_window_x.size();j++)
    	if(which_window_x[j] == leftest_grid && which_window_y[j] == lowest_grid)
    	{
    		if(ind == 0)
    			area_sumShape_neg[j] += additional_area;
    		else
    			area_sumShape_pos[j] += additional_area;
    		init_ind = 0;
  			break;
    	}
    if(init_ind == 1)
   	{
    	if(ind == 0)
    	{
 	   		area_sumShape_neg.push_back(additional_area);
    		area_sumShape_pos.push_back(0);
    	}
 		else
 		{
 	   		area_sumShape_pos.push_back(additional_area);
 			area_sumShape_neg.push_back(0);
 		}
    	which_window_x.push_back(leftest_grid);
    	which_window_y.push_back(lowest_grid);
   	}
}

void Cal_contribution(int*** Contributor_list, int*** Area_list, int x_grid, int y_grid, int gp_id, int diff)
{
	int abs_diff_area = abs(diff);
	if(Contributor_list[x_grid][y_grid][0] == _MaxContributor_ - 1)
	{
		if(abs_diff_area > abs(Area_list[x_grid][y_grid][_MaxContributor_ - 1]))
		{
			Area_list[x_grid][y_grid][_MaxContributor_ - 1] = diff;
			Contributor_list[x_grid][y_grid][_MaxContributor_ - 1] = gp_id;
		}
		else
			return;
	}
	else
		Contributor_list[x_grid][y_grid][0] += 1; 

	int num_contributors = Contributor_list[x_grid][y_grid][0];
	Contributor_list[x_grid][y_grid][num_contributors] = gp_id;
	Area_list[x_grid][y_grid][num_contributors] = diff;
	for(int i = num_contributors - 1;i > 0; i--)
	{
		if(abs(Area_list[x_grid][y_grid][i+1]) > abs(Area_list[x_grid][y_grid][i]))
		{
			swap(Area_list[x_grid][y_grid][i+1], Area_list[x_grid][y_grid][i]);
			swap(Contributor_list[x_grid][y_grid][i+1], Contributor_list[x_grid][y_grid][i]);
		}
		else
			break;
	}
	return;
}

bool detect_2coloring(int argc, char argv[_ParsingNum_][_BUFFSIZE_], Graph* ourGraph)
{
    char mybuf[_BUFFSIZE_];

    //get file name to be writen
    int fd = open(argv[4], O_WRONLY | O_CREAT, 777);
    we_cant_ignore_write = write(fd, _2Color_, strlen(_2Color_));
    int seqWindNum = 0;
    for(int i = 0;i <strlen(argv[5]); i++)
	seqWindNum += seqWindNum * 10 + (argv[5][i] - '0');
    sprintf(mybuf, "graph gn%d_2coloring {\n", seqWindNum);
    we_cant_ignore_write = write(fd, mybuf, strlen(mybuf));
    int i, j;
    if(!fd)
    {
		fprintf(stderr, "damn, cant open the file you appoint, the file doesn't exist");
		return false;
    }

    //directly return true if only one shape
    if(ourGraph->nodes.size() == 1)
    {
    	//printf("~~~~~ only one shape~~~~\n");
    	return true;
    }

    //get the source node
    int Not_2Colorable, sourceNum = 0;
    sourceNum = ourGraph->nodes[0]->id;
    //printf("sourceNum = %d\n", sourceNum);

//TODO:	perform BFS traversal
    //set all vertex untraveled first
    for(i = 0; i< ourGraph->nodes.size(); i++)
    {
		ourGraph->nodes[i]->traveled = false;
		ourGraph->nodes[i]->color = 0;
    }

    Node* newnode = ourGraph->getNodeById(sourceNum);
    Node* sidenode;
    Edge* newedge;

    vector<Node* > remaindernodes;
    remaindernodes.push_back(newnode);
    int index = 0;
    //int traveled = 0;
    int edgeCounter = 0;
    vector<int> wating_list;
    for(i = 0; i< ourGraph->nodes.size(); i++)
	wating_list.push_back(ourGraph->nodes[i]->id);
    while(wating_list.size() != 0)
    {
		//traverse
		if(newnode->traveled == true)
		{
		    index += 1;
		    continue;
		}	

		//  first check for whether there's a conflict or not
		Not_2Colorable = 0;
		for(i = 0;i<newnode->edge.size(); i++)
		{
		    newedge = newnode->edge[i];
		    sidenode = newedge->getNeighbor(newnode);
		    //printf("pick one of neighbors of %d == %d\n", newnode->id, sidenode->id);
		    if((sidenode->color == 1 && Not_2Colorable == 1) || (sidenode->color == -1 && Not_2Colorable == -1))
			Not_2Colorable = -2;
		    else if(sidenode->color == 1 && Not_2Colorable == 0)
			Not_2Colorable = -1;
		    else if(sidenode->color == -1 && Not_2Colorable == 0)
			Not_2Colorable = 1;
		}

		if(Not_2Colorable == -2)
		{
		    we_cant_ignore_write = write(fd, _Not2Colorable_, strlen(_Not2Colorable_));
		    //printf("in 2color: Not 2colorable, Im going to break...\n");
		    break;
		}
		else
		{
		    if(Not_2Colorable == 1)
			newnode->color = 1;
		    else if(Not_2Colorable == -1)
			newnode->color = -1;
		    else if(Not_2Colorable == 0)
			newnode->color = 1;
		    for(i = 0;i<newnode->edge.size(); i++)
		    {
			newedge = newnode->edge[i];
			sidenode = newedge->getNeighbor(newnode);
			if(sidenode->color == 0)
			{
			    remaindernodes.push_back(sidenode);
			    if(newnode->id < sidenode->id)
			    {
				sprintf(mybuf, "v%d -- v%d;\n", newnode->id, sidenode->id);
				we_cant_ignore_write = write(fd, mybuf, strlen(mybuf));
				edgeCounter += 1;
			    }
			    else
			    {
				sprintf(mybuf, "v%d -- v%d;\n", sidenode->id, newnode->id);
				we_cant_ignore_write = write(fd, mybuf, strlen(mybuf));
				edgeCounter += 1;	
			    }
			    //renew the meata data
			    sidenode->color = (-1) * newnode->color;
			}
			//printf("in 2color: newnode->id == %d, newnode->color == %d; sidenode->id == %d, sidenode->color == %d\n", newnode->id, newnode->color, sidenode->id, sidenode->color);	
		    }					
		}

		//renew the meta data
		newnode->traveled = true;
		for(i = 0; i< wating_list.size(); i++)
		    if(wating_list[i] == newnode->id)
		    {
				swap(wating_list[i], wating_list[wating_list.size() - 1]);
				wating_list.pop_back();
				break;
		    }
		if(wating_list.size() == 0)
		    break;

		index += 1;
		newnode = ourGraph->getNodeById(remaindernodes[index]->id);
    }

    //	final check for whether there's a conflict or not
    for(i = 0; i< ourGraph->nodes.size() && Not_2Colorable != -2; i++)
    {
		for(j = 0;j<ourGraph->nodes[i]->edge.size(); j++)
		{	
		    newedge = ourGraph->nodes[i]->edge[j];
		    sidenode = newedge->getNeighbor(ourGraph->nodes[i]);
		    if(ourGraph->nodes[i]->color == sidenode->color && ourGraph->nodes.size() != 1)
		    {
				//printf("in 2color: Not 2colorable, Im going to break...; ourGraph.nodes[i]->id == %d, sidenode->id == %d\n", ourGraph->nodes[i]->id, sidenode->id);
				Not_2Colorable = -2;
				break;
		    }
		}
    }

    we_cant_ignore_write = write(fd, theEnd, strlen(theEnd));

    sprintf(mybuf, "// vertices = %d\n", (int)(ourGraph->nodes.size()));
    we_cant_ignore_write = write(fd, mybuf, strlen(mybuf));
    sprintf(mybuf, "// edges = %d\n", edgeCounter);
    we_cant_ignore_write = write(fd, mybuf, strlen(mybuf));

    close(fd); 

    //	return whether it's 2colorable or not (true == 2colorable; false == not 2colorabel)
    if(Not_2Colorable == -2)
		return false;
    return true;
}

void reRead(int argc, char argv[_ParsingNum_][_BUFFSIZE_], Graph* ourGraph)
{
    //clear garbage
    vector<int>().swap(ourNodes1);
    vector<int>().swap(ourNodes2);

    int fd = open(argv[1], O_RDONLY);
    int num1, num2 = 0;
    int i, j;
    char mybuf[_BUFFSIZE_];
    if(!fd)
    {
		fprintf(stderr, "damn, cant open the file you appoint, the file doesn't exist");
		return;
    }
    //printf("open %s, successful\n", argv[1]);
    while((read(fd, mybuf, 1) == 1))
    {
		//int counter = 0;
		if(mybuf[0] == '/')
		    while(mybuf[0] != '\n')
				we_cant_ignore_read = read(fd, mybuf, 1);
		else if(mybuf[0] == 'g')
		{

		    while(mybuf[0] != '}')
		    {
				while(mybuf[0] != '\n')
				    we_cant_ignore_read = read(fd, mybuf, 1);

				we_cant_ignore_read = read(fd, mybuf, 1);
				if(mybuf[0] == '}')
				    break;

				num1 = 0;
				//start reading the first number
				while(1)
				{
				    we_cant_ignore_read = read(fd, mybuf, 1);
				    if(mybuf[0] == ' ')
						break;
				    num1 = num1 * 10 + (mybuf[0] - '0');
				}

				Node nodeTemp(num1);
				ourNodes1.push_back(num1);

				while(mybuf[0] != 'v')
				    we_cant_ignore_read = read(fd, mybuf, 1);

				//start reading the next node
				num2 = 0;
				while(1)
				{
				    we_cant_ignore_read = read(fd, mybuf, 1);
				    if(mybuf[0] == ';')
					break;
				    num2 = num2 * 10 + (mybuf[0] - '0'); 
				}
				Node nodeTemp2(num2);
				ourNodes2.push_back(num2);
				ourGraph->addEdge(num1, num2);
		    }
		    return;	   		   
		}
    }
}

Color_2coloring::Color_2coloring()
{}

int* Color_2coloring::findBoundingBox(int omega, Shape* Data, int dataSize, int* legalGPNum)
{
    int up = _MinNum_;
    int right = _MinNum_;
    int left = _MaxNum_;
    int down = _MaxNum_;
    int i, j;

    for(i = 0; i<dataSize; i++)
    {
		for(j= 1;j< legalGPNum[0]+ 1; j++)
		    if(Data[i].group_index == legalGPNum[j])
		    {
				//printf("i == %d, legalGPNum == %d\n", i, legalGPNum[j]);
				if(up < Data[i].y[1])
				    up = Data[i].y[1];
				if(right < Data[i].x[1])
				    right = Data[i].x[1];
				if(down > Data[i].y[0])
				    down = Data[i].y[0];
				if(left > Data[i].x[0])
				    left = Data[i].x[0];
				break;
		    }
    }

    int* up_down_left_right = new int[4];
    up_down_left_right[0] = up;
    up_down_left_right[1] = down;
    up_down_left_right[2] = left;
    up_down_left_right[3] = right;

    return up_down_left_right;
}

int Min(int a, int b)
{
    if(a >= b)
	return b;
    return a;
}

int** test_InBound(int** grid, int omega, int which_node, int type, Shape* Data, Graph* ourGraph, int leftest_grid, int rightest_grid, int toppest_grid, int lowest_grid)
{
    int** which_to_fill;
    int i;
    int counter = 0;
    if(type == 1)
    {
		which_to_fill = new int*[3 +1];
		for(i = 0;i<3+1;i++)
		    which_to_fill[i] = new int[4];
    }
    else if(type == 2)
    {
		which_to_fill = new int*[3 + toppest_grid - lowest_grid];
		for(i = 0;i<3 + toppest_grid - lowest_grid;i++)
		    which_to_fill[i] = new int[4];
    }
    else if(type == 3)
    {
		which_to_fill = new int*[3 + rightest_grid - leftest_grid];
		for(i = 0;i<3 + rightest_grid - leftest_grid;i++)
		    which_to_fill[i] = new int[4];	
    }
    else if(type == 4)
    {
		which_to_fill = new int*[2 + rightest_grid - leftest_grid + toppest_grid - lowest_grid];
		for(i = 0;i<2 + rightest_grid - leftest_grid + toppest_grid - lowest_grid;i++)
		    which_to_fill[i] = new int[4];	
    }


    int rightest_implicit_left_bound = grid[0][grid[0][0] + 1] - omega;
    int toppest_implicit_lower_bound = grid[1][grid[1][0] + 1] - omega;
    int rightest_explicit_left_grid = grid[0][0];
    int toppest_explicit_lower_grid = grid[1][0];
    int delta_x = Min(Data[ourGraph->nodes[which_node]->id].x[1] - rightest_implicit_left_bound, Data[ourGraph->nodes[which_node]->id].x[1] - Data[ourGraph->nodes[which_node]->id].x[0]);
    int delta_y = Min(Data[ourGraph->nodes[which_node]->id].y[1] - toppest_implicit_lower_bound, Data[ourGraph->nodes[which_node]->id].y[1] - Data[ourGraph->nodes[which_node]->id].y[0]);

    if(type == 1)
    {
		if(delta_x > 0)
		{
		    // $$$ this implies that it's either type3 or type4 actually
		    counter += 1;
		    which_to_fill[counter][0] = rightest_explicit_left_grid;
		    which_to_fill[counter][1] = toppest_grid - 1;
		    which_to_fill[counter][2] = delta_x;
		    which_to_fill[counter][3] = Data[ourGraph->nodes[which_node]->id].y[1] - Data[ourGraph->nodes[which_node]->id].y[0];
		    //printf("type1--1===========we r on the up side======\n");

		    if(delta_y > 0)
		    {
				if(Data[ourGraph->nodes[which_node]->id].x[0] -  grid[0][rightest_explicit_left_grid]< 0)
				{
				    counter += 1;
				    which_to_fill[counter][0] = rightest_explicit_left_grid - 1;
				    which_to_fill[counter][1] = toppest_explicit_lower_grid;
				    which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - Data[ourGraph->nodes[which_node]->id].x[0];
				    which_to_fill[counter][3] = delta_y;
				    //printf("type1--2===========we r on the horizontal side======\n");
				}

				if(toppest_grid - 1 != toppest_explicit_lower_grid)
				{
				    // $$$ this implies that it's type4 actually
				    counter += 1;
				    which_to_fill[counter][0] = rightest_explicit_left_grid;
				    which_to_fill[counter][1] = toppest_explicit_lower_grid;
				    which_to_fill[counter][2] = delta_x;
				    which_to_fill[counter][3] = delta_y;
				    //printf("type1--3===========we r on the corner side======\n");
				}
		    }
		}
		else if(delta_y > 0)
		{
		    // $$$ this implies that it's type2 actually
		    counter += 1;
		    which_to_fill[counter][0] = leftest_grid;
		    which_to_fill[counter][1] = toppest_explicit_lower_grid;
		    which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - Data[ourGraph->nodes[which_node]->id].x[0];
		    which_to_fill[counter][3] = delta_y;				
		    //printf("type1--there's no 1, there's 2===========we r on the uppfer side======\n");
		}
    }
    else if(type == 2)
    {
		if(delta_x > 0)
		{
		    for(i = 0;i<toppest_grid - lowest_grid;i++)
		    {	
				counter += 1;
				which_to_fill[counter][0] = rightest_explicit_left_grid;
				which_to_fill[counter][1] = lowest_grid + i;
				which_to_fill[counter][2] = delta_x;
				if(i == 0)
				    which_to_fill[counter][3] = grid[1][lowest_grid + 1] - Data[ourGraph->nodes[which_node]->id].y[0];
				else if(i == toppest_grid - lowest_grid - 1)
				{
				    if(delta_y <= 0)
					which_to_fill[counter][3] = Data[ourGraph->nodes[which_node]->id].y[1] - grid[1][toppest_grid - 1];
				    else
					which_to_fill[counter][3] = delta_y;
				}
				else
				    which_to_fill[counter][3] = grid[1][lowest_grid + i +1] - grid[1][lowest_grid + i];
				//printf("type2--1===========we r on the up side======\n");
		    }
		    if(delta_y > 0 && grid[0][rightest_explicit_left_grid] - Data[ourGraph->nodes[which_node]->id].x[0] > 0)
		    {
				counter += 1;
				which_to_fill[counter][0] = rightest_explicit_left_grid - 1;
				which_to_fill[counter][1] = toppest_explicit_lower_grid;
				which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - Data[ourGraph->nodes[which_node]->id].x[0];
				which_to_fill[counter][3] = delta_y;
				//printf("type2--2===========we r on the horizontal side======\n");


				if(toppest_grid - 1 != toppest_explicit_lower_grid)
				{		
				    // $$$ this implies that it's type4 actually
				    counter += 1;
				    which_to_fill[counter][0] = rightest_explicit_left_grid;
				    which_to_fill[counter][1] = toppest_explicit_lower_grid;
				    which_to_fill[counter][2] = delta_x;
				    which_to_fill[counter][3] = delta_y;
				    //printf("type2--3===========we r on the corner side======\n");
				}
		    }
		}
		else if(delta_y > 0)
		{
		    counter += 1;
		    which_to_fill[counter][0] = leftest_grid;
		    which_to_fill[counter][1] = toppest_explicit_lower_grid;
		    which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - Data[ourGraph->nodes[which_node]->id].x[0];
		    which_to_fill[counter][3] = delta_y;				
		    //printf("type2--there's no 1, there's 2===========we r on the horizontal side======\n");
		}
    }
    else if(type == 3)
    {
		if(delta_y > 0)
		{
		    for(i = 0;i<rightest_grid - leftest_grid;i++)
		    {	
				counter += 1;
				which_to_fill[counter][0] = leftest_grid + i;
				which_to_fill[counter][1] = toppest_explicit_lower_grid;
				which_to_fill[counter][3] = delta_y;
				if(i == 0)
				    which_to_fill[counter][2] = grid[0][leftest_grid + 1] - Data[ourGraph->nodes[which_node]->id].x[0];
				else if(i == rightest_grid - leftest_grid - 1)
				{
				    if(delta_x <= 0)
					which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - grid[0][rightest_grid - 1];
				    else
					which_to_fill[counter][2] = delta_x;
				}
				else
				    which_to_fill[counter][2] = grid[1][leftest_grid + i +1] - grid[1][leftest_grid + i];
				//printf("type3--1===========we r on the horizontal side======\n");
		    }
		    if(delta_x > 0 && grid[1][toppest_explicit_lower_grid] - Data[ourGraph->nodes[which_node]->id].y[0] > 0)
		    {
				counter += 1;
				which_to_fill[counter][0] = rightest_explicit_left_grid; 
				which_to_fill[counter][1] = toppest_explicit_lower_grid - 1;
				which_to_fill[counter][2] = delta_x; 
				which_to_fill[counter][3] = Data[ourGraph->nodes[which_node]->id].y[1] - Data[ourGraph->nodes[which_node]->id].y[0];
				//printf("type3--2===========we r on the up side======\n");

				if(rightest_grid - 1 != rightest_explicit_left_grid)
				{		
				    // $$$ this implies that it's type4 actually
				    counter += 1;
				    which_to_fill[counter][0] = rightest_explicit_left_grid;
				    which_to_fill[counter][1] = toppest_explicit_lower_grid;
				    which_to_fill[counter][2] = delta_x;
				    which_to_fill[counter][3] = delta_y;
				    //printf("type3--3===========we r on the corner side======\n");
				}
			}
		}
		else if(delta_x > 0)
		{

		    counter += 1;
		    which_to_fill[counter][0] = rightest_explicit_left_grid;
		    which_to_fill[counter][1] = lowest_grid;
		    which_to_fill[counter][2] = delta_x;
		    which_to_fill[counter][3] = Data[ourGraph->nodes[which_node]->id].y[1] - Data[ourGraph->nodes[which_node]->id].y[0];				
		    //printf("type3--there's no 1, there's 2===========we r on the uppfer side======\n");
		}
    }
    else if(type == 4)
    {
		if(delta_y > 0)
		{
		    for(i = 0;i<rightest_grid - leftest_grid;i++)
		    {	
				counter += 1;
				which_to_fill[counter][0] = leftest_grid + i;
				which_to_fill[counter][1] = toppest_explicit_lower_grid;
				which_to_fill[counter][3] = delta_y;
				if(i == 0)
				    which_to_fill[counter][2] = grid[0][leftest_grid + 1] - Data[ourGraph->nodes[which_node]->id].x[0];
				else if(i == rightest_grid - leftest_grid - 1)
				{
				    if(delta_x <= 0)
						which_to_fill[counter][2] = Data[ourGraph->nodes[which_node]->id].x[1] - grid[0][rightest_grid - 1];
				    else
						which_to_fill[counter][2] = delta_x;
				}
				else
				    which_to_fill[counter][2] = grid[1][leftest_grid + i +1] - grid[1][leftest_grid + i];
		    }
		}
		if(delta_x > 0 && grid[1][toppest_explicit_lower_grid] - Data[ourGraph->nodes[which_node]->id].y[0] > 0)
		{
		    for(i = 0;i<toppest_grid - lowest_grid;i++)
		    {	
				counter += 1;
				which_to_fill[counter][0] = rightest_explicit_left_grid;
				which_to_fill[counter][1] = lowest_grid + i;
				which_to_fill[counter][2] = delta_x;
				if(i == 0)
				    which_to_fill[counter][3] = grid[1][lowest_grid + 1] - Data[ourGraph->nodes[which_node]->id].y[0];
				else if(i == toppest_grid - lowest_grid - 1)
				    continue;
				else
				    which_to_fill[counter][3] = grid[1][lowest_grid + i +1] - grid[1][lowest_grid + i];
		    }
		    if(toppest_grid >= toppest_explicit_lower_grid && rightest_grid - 1 != rightest_explicit_left_grid)
		    {	
				counter += 1;
				which_to_fill[counter][0] = rightest_explicit_left_grid;
				which_to_fill[counter][1] = toppest_explicit_lower_grid;
				which_to_fill[counter][2] = delta_x;
				which_to_fill[counter][3] = delta_y;				
		    }
		}
    }

    which_to_fill[0][0] = counter;
    //for(i = 1; i<which_to_fill[0][0]+1; i++)
	//	printf("	### in test_InBound: type == %d, x == %d, y == %d, deltax == %d, deltay == %d\n", type, which_to_fill[i][0], which_to_fill[i][1], which_to_fill[i][2], which_to_fill[i][3]);
    return which_to_fill;
}

void each_Area(int*** eachArea, int** grid, Shape* Data, Graph* ourGraph, int* boxbounding, int omega, int first_assignment, int*** Contributor_list, int*** Area_list, int which_gp)	//	assuming having a group/graph, calculating its density contribution to each window
{
    //get the source node
    int i,j, k, checkCounter, type;
    int sourceNum = ourGraph->nodes[0]->id;
    int rightest_explicit_left_grid = grid[0][0];
    int toppest_explicit_lower_grid = grid[1][0];
    //printf("	>>sourceNum = %d\n", sourceNum);

//	TODO: perform BFS traversal
    //set all vertex untraveled first
    for(i = 0; i< ourGraph->nodes.size(); i++)
    {
		ourGraph->nodes[i]->traveled = false;
		ourGraph->nodes[i]->color = 0;
    }

    //preprocessing
    Node* newnode = ourGraph->getNodeById(sourceNum);
    Node* sidenode;
    Edge* newedge;

    vector<Node* > remaindernodes;
    remaindernodes.push_back(newnode);
    int index = 0;
    int edgeCounter = 0;
    vector<int> wating_list;
    for(i = 0; i< ourGraph->nodes.size(); i++)
	wating_list.push_back(ourGraph->nodes[i]->id);

    while(wating_list.size() != 0)
    {
		//	traverse
		if(newnode->traveled == true)
		{
		    index += 1;
		    continue;
		}	

		//	TODO: first check sidenodes' colors
		for(i = 0;i<newnode->edge.size(); i++)
		{
		    newedge = newnode->edge[i];
		    sidenode = newedge->getNeighbor(newnode);
		    //printf("pick one of neighbors of %d == %d\n", newnode->id, sidenode->id);
		    if(sidenode->color == 1)
		    {
				newnode->color = -1;
				break;
		    }
		    else if(sidenode->color == -1)
		    {
				newnode->color = 1;
				break;
		    }
		}

		if(newnode->color == 0)
		{
		    first_assignment = first_assignment * (-1);
		    newnode->color = first_assignment;
		}
		for(i = 0;i<newnode->edge.size(); i++)
		{
		    newedge = newnode->edge[i];
		    sidenode = newedge->getNeighbor(newnode);
		    if(sidenode->color == 0)
		    {
				remaindernodes.push_back(sidenode);
				edgeCounter += 1;	

				//renew the meata data
				sidenode->color = (-1) * newnode->color;
		    }
		    //printf("in 2color: newnode->id == %d, newnode->color == %d; sidenode->id == %d, sidenode->color == %d\n", newnode->id, newnode->color, sidenode->id, sidenode->color);	
		}					

		//renew the meta data
		newnode->traveled = true;
		for(i = 0; i< wating_list.size(); i++)
		    if(wating_list[i] == newnode->id)
		    {
				swap(wating_list[i], wating_list[wating_list.size() - 1]);
				wating_list.pop_back();
			break;
		    }
		if(wating_list.size() == 0)
		    break;

		index += 1;
		newnode = ourGraph->getNodeById(remaindernodes[index]->id);
    }

    //for(i = 0;i<ourGraph->nodes.size(); i++)
	//	printf("	>>cal nodes area: node == %d, color == %d\n", ourGraph->nodes[i]->id, ourGraph->nodes[i]->color);

//	TODO: calculating density for initial = first_assignment
    int area_1, area_negative1, leftest_grid, rightest_grid, toppest_grid, lowest_grid, ind, additional_area;
    vector<int> area_sumShape_neg, area_sumShape_pos;
	vector<int> which_window_x, which_window_y;
    for(i = 0;i<ourGraph->nodes.size(); i++)
    {
		//	spread area to windows
		//	check leftest' position
		for(j = grid[0][0] + 1;j > 0;j--)
		    if(grid[0][j] <= Data[ourGraph->nodes[i]->id].x[0])
		    {
				leftest_grid = j;
				break;
		    }
		//	check rightest' position
		for(j = leftest_grid + 1; j< grid[0][0] + 2;j++)
		    if(grid[0][j] >= Data[ourGraph->nodes[i]->id].x[1])
		    {
				rightest_grid = j;
				break;
		    }
		//	check lowest' position
		for(j = grid[1][0] + 1; j>0;j--)
		    if(grid[1][j] <= Data[ourGraph->nodes[i]->id].y[0])
		    {
				lowest_grid = j;
				break;
		    }
		//	check toppest' position
		for(j = lowest_grid + 1; j< grid[1][0] + 2;j++)
		    if(grid[1][j] >= Data[ourGraph->nodes[i]->id].y[1])
		    {
				toppest_grid = j;
				break;
		    }

		ind = 1;
		if(ourGraph->nodes[i]->color == -1)
		    ind = 0;
		//printf("-----starting picking node == %d, toppest == %d, lowest == %d, leftest == %d, rightest == %d-----\n", ourGraph->nodes[i]->id, toppest_grid, lowest_grid, leftest_grid, rightest_grid);

		int** checkBoundary;
		//	TODO: fill in each area;
		if(rightest_grid - leftest_grid == 1)
		{	
		    if(toppest_grid - lowest_grid == 1)	//	"seemly" type1
	    	{	
	    		type = _type1_;
	    			if(leftest_grid != rightest_explicit_left_grid && lowest_grid != toppest_explicit_lower_grid)
	    			{
	    				additional_area = (Data[ourGraph->nodes[i]->id].x[1] - Data[ourGraph->nodes[i]->id].x[0]) * (Data[ourGraph->nodes[i]->id].y[1] - Data[ourGraph->nodes[i]->id].y[0]);
	    				eachArea[leftest_grid][lowest_grid][ind] += additional_area;
	    				//printf("	###type1, cal nodes area: x_grid == %d, y_grid == %d, area == %d\n", leftest_grid, lowest_grid, eachArea[leftest_grid][lowest_grid][ind]);
	    					
	    				Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, leftest_grid, lowest_grid, ind);
	    			}
	    	}
	    	else	 //	"seemly" type2
	    	{
	    		type = _type2_;
	    		if(leftest_grid != rightest_explicit_left_grid)
	    		{
	    			for(k = lowest_grid; k<= toppest_grid - 1; k++)
	    			{
	    				if(k == rightest_explicit_left_grid)
	    					break;
	    				if(k == lowest_grid)
	    				{
	    					additional_area = (Data[ourGraph->nodes[i]->id].x[1] - Data[ourGraph->nodes[i]->id].x[0]) * (grid[1][lowest_grid + 1] - Data[ourGraph->nodes[i]->id].y[0]);
		    					eachArea[leftest_grid][k][ind] += additional_area;
	   					}
		    			else if(k == toppest_grid - 1)
		    			{
		    				additional_area = (Data[ourGraph->nodes[i]->id].x[1] - Data[ourGraph->nodes[i]->id].x[0]) * (Data[ourGraph->nodes[i]->id].y[1] - grid[1][toppest_grid - 1]);
	    					eachArea[leftest_grid][k][ind] += additional_area;
		    			}
	    				else
	    				{
	    					additional_area = (Data[ourGraph->nodes[i]->id].x[1] - Data[ourGraph->nodes[i]->id].x[0]) * (grid[1][k + 1] - grid[1][k]);
	    					eachArea[leftest_grid][k][ind] += additional_area;
	    				}
	   					
	   					//printf("	###type2, cal nodes area:  x_grid == %d, y_grid == %d, area == %d\n", leftest_grid, k, eachArea[leftest_grid][k][ind]);		
	    				Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, leftest_grid, k, ind);
	    			}
	    		}
	    	}

	    	//	TODO: assign those windows close boundaries
			checkBoundary = test_InBound(grid, omega, i, type, Data, ourGraph, leftest_grid, rightest_grid, toppest_grid, lowest_grid);
		    for(checkCounter = 1; checkCounter < checkBoundary[0][0] + 1; checkCounter ++)
		    {
				if(checkBoundary[checkCounter][0] > 0 && checkBoundary[checkCounter][1] > 0)
				{
					additional_area = checkBoundary[checkCounter][2] * checkBoundary[checkCounter][3];
					eachArea[checkBoundary[checkCounter][0]][checkBoundary[checkCounter][1]][ind] += additional_area;			
					Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, checkBoundary[checkCounter][0], checkBoundary[checkCounter][1], ind);
				}
   			}
		}
		else if(toppest_grid - lowest_grid == 1)//	"seemly" type3
	    {
	    	type = _type3_;
	    	if(lowest_grid != toppest_explicit_lower_grid)
	    	{
	    		for(j = leftest_grid; j<= rightest_grid - 1; j++)
	    		{	
	    			if(j == rightest_explicit_left_grid)
	    				break;
	    			if(j == leftest_grid)
	    			{
	    				additional_area = (grid[0][leftest_grid + 1] - Data[ourGraph->nodes[i]->id].x[0]) * (Data[ourGraph->nodes[i]->id].y[1] - Data[ourGraph->nodes[i]->id].y[0]);
	    				eachArea[j][lowest_grid][ind] += additional_area;
	    			}
	    			else if(j == rightest_grid - 1)
	    			{
	    				additional_area = (Data[ourGraph->nodes[i]->id].x[1] - grid[0][rightest_grid - 1]) * (Data[ourGraph->nodes[i]->id].y[1] - Data[ourGraph->nodes[i]->id].y[0]);
	    				eachArea[j][lowest_grid][ind] += additional_area;
	   				}
	    			else
	    			{
	    				additional_area = (grid[0][j+1] - grid[0][j]) * (Data[ourGraph->nodes[i]->id].y[1] - Data[ourGraph->nodes[i]->id].y[0]);
	    				eachArea[j][lowest_grid][ind] += additional_area;
	    			}
	    		
	    			//printf("	###type3, cal nodes area:  x_grid == %d, y_grid == %d, area == %d\n", j, lowest_grid, eachArea[j][lowest_grid][ind]);
	    			Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, j, lowest_grid, ind);
	    		}
	    	}

	    	//	TODO: assign those windows near boundaries
	    	checkBoundary = test_InBound(grid, omega, i, type, Data, ourGraph, leftest_grid, rightest_grid, toppest_grid, lowest_grid);
		    for(checkCounter = 1; checkCounter < checkBoundary[0][0] + 1; checkCounter ++)
		    {
				if(checkBoundary[checkCounter][0] > 0 && checkBoundary[checkCounter][1] > 0)
				{
					additional_area = checkBoundary[checkCounter][2] * checkBoundary[checkCounter][3];
					eachArea[checkBoundary[checkCounter][0]][checkBoundary[checkCounter][1]][ind] += additional_area;			
					Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, checkBoundary[checkCounter][0], checkBoundary[checkCounter][1], ind);
				}
			}
	    }
	    else	//type4
	    {
	    	type = _type4_;	
			for(k = lowest_grid; k <= toppest_grid - 1; k++)
	    	{
			    if(k != toppest_explicit_lower_grid)
		    		for(j = leftest_grid; j <= rightest_grid - 1;j++)
					{
					    if(j == rightest_explicit_left_grid)
		    				break;
					    if(k == lowest_grid)
						{
					    	if(j == leftest_grid)
							{
					    		additional_area = (grid[0][leftest_grid + 1] - Data[ourGraph->nodes[i]->id].x[0]) * (grid[1][lowest_grid + 1] - Data[ourGraph->nodes[i]->id].y[0]);
								eachArea[j][k][ind] += additional_area;
					    	}
					    	else if(j == rightest_grid - 1)
							{
					    		additional_area = (Data[ourGraph->nodes[i]->id].x[1] - grid[0][rightest_grid - 1]) * (grid[1][lowest_grid + 1] - Data[ourGraph->nodes[i]->id].y[0]);
					   			eachArea[j][k][ind] += additional_area;
			 				}
					    	else
							{
					    		additional_area = (grid[0][j + 1] - grid[0][j]) * (grid[1][lowest_grid + 1] - Data[ourGraph->nodes[i]->id].y[0]);
					  			eachArea[j][k][ind] += additional_area;
			  				}
					    }
					    else if(k == toppest_grid - 1)
					    {
					    	if(j == leftest_grid)
					    	{
					    		additional_area = (grid[0][leftest_grid + 1] - Data[ourGraph->nodes[i]->id].x[0]) * (Data[ourGraph->nodes[i]->id].y[1] - grid[1][toppest_grid - 1]);
					    		eachArea[j][k][ind] += additional_area;
					    	}
					    	else if(j == rightest_grid - 1)
					    	{
					    		additional_area = (Data[ourGraph->nodes[i]->id].x[1] - grid[0][rightest_grid - 1]) * (Data[ourGraph->nodes[i]->id].y[1] - grid[1][toppest_grid - 1]);
					    		eachArea[j][k][ind] += additional_area;
					    	}
					    	else
					    	{
					    		additional_area = (grid[0][j + 1] - grid[0][j]) * (Data[ourGraph->nodes[i]->id].y[1] - grid[1][toppest_grid - 1]);
								eachArea[j][k][ind] += additional_area;
		    				}
		    			}
					    else if(j == leftest_grid)
					    {
							additional_area = (grid[0][leftest_grid + 1] - Data[ourGraph->nodes[i]->id].x[0]) * (grid[1][k+1] - grid[1][k]);
			  				eachArea[j][k][ind] += additional_area;
		    			}
			    		else if(j == rightest_grid - 1)
					    {
					    	additional_area = (Data[ourGraph->nodes[i]->id].x[1] - grid[0][rightest_grid - 1]) * (grid[1][k+1] - grid[1][k]);
					    	eachArea[j][k][ind] += additional_area;
					    }
					    else	//	i.e. internal blocks
					    {
					    	additional_area = (grid[0][j + 1] - grid[0][j]) * (grid[1][k+1] - grid[1][k]);
					    	eachArea[j][k][ind] += additional_area;
					    }
					    	//printf("	###type4, cal nodes area:  x_grid == %d, y_grid == %d, area == %d\n", j, k, eachArea[j][k][ind]);

					    Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, j, k, ind);
					}
			}

			//	TODO: assign those windows near boundaries
			checkBoundary = test_InBound(grid, omega, i, type, Data, ourGraph, leftest_grid, rightest_grid, toppest_grid, lowest_grid);
		    for(checkCounter = 1; checkCounter < checkBoundary[0][0] + 1; checkCounter ++)
		    {
				if(checkBoundary[checkCounter][0] > 0 && checkBoundary[checkCounter][1] > 0)
				{
					additional_area = checkBoundary[checkCounter][2] * checkBoundary[checkCounter][3];
					eachArea[checkBoundary[checkCounter][0]][checkBoundary[checkCounter][1]][ind] += additional_area;			
					Add_area_perShape(which_window_x, which_window_y, area_sumShape_neg, area_sumShape_pos, additional_area, checkBoundary[checkCounter][0], checkBoundary[checkCounter][1], ind);
				}
			}
	    }

		//	TODO: release mem
	    int temp = checkBoundary[0][0];
		for(checkCounter = 0; checkCounter < temp; checkCounter ++)
		    delete [] checkBoundary[checkCounter];
		delete [] checkBoundary;
    }

    //	TODO: memoize the diff made by this gp
    for(j = 0;j< which_window_x.size(); j++)
   		Cal_contribution(Contributor_list, Area_list, which_window_x[j], which_window_y[j], which_gp, area_sumShape_pos[j] - area_sumShape_neg[j]);
}

void empty_eachArea(int*** eachArea, int X_block_Num, int Y_block_Num)
{
    for (int i= 1; i<= X_block_Num; i++)
		for (int j= 0; j<= Y_block_Num; j++)
		    for (int k= 0; k< 2; k++)
				eachArea[i][j][k] = 0;
    return;
}

int** Color_2coloring::CalGrid(int* boxbounding, int omega)
{
    int i, j, up_Num, right_Num, grid_RightUp;

    if((boxbounding[0] - boxbounding[1]) % omega != 0)
		up_Num = 1;
	else
		up_Num = 0;
    up_Num += (boxbounding[0] - boxbounding[1])/omega;

    if((boxbounding[3] - boxbounding[2]) % omega != 0)
		right_Num = 1;
	else
		right_Num = 0;
    right_Num += (boxbounding[3] - boxbounding[2])/omega;

    //printf("%d, %d\n", up_Num, right_Num);
    //	grid[0] for x, grid[1] for y
    int** grid = new int*[2];
    grid[0] = new int[right_Num + 1 + 1];
    grid[1] = new int[up_Num + 1 + 1];

    grid[0][0] = right_Num;
    grid[1][0] = up_Num;
    grid_RightUp = boxbounding[2];
    for(i = 1; i< right_Num+1; i++)
    {
	grid[0][i] = grid_RightUp;
	grid_RightUp += omega;
    }
    grid[0][right_Num + 1] = boxbounding[3];

    grid_RightUp = boxbounding[1];
    for(i = 1; i< up_Num+1; i++)
    {
	grid[1][i] = grid_RightUp;
	grid_RightUp += omega;
    }
    grid[1][up_Num + 1] = boxbounding[0];

    return grid;
}

/*
int main_group_in_grid(int** grid,int j, int k, Shape* Data, Graph* ourGraph,int* checkingGP){
    int main=checkingGP[1];
    int max_area=0;
    int area;
    for(int i=1;i<checkingGP[0]+1;i++){
	area=0;
	for(int l=0;l<ourGraph[checkingGP[i]].nodes.size();l++){
	    int node_area=0;
	    int node=ourGraph[checkingGP[i]].nodes[l]->id;
	    int x0=Data[node].x[0];
	    int x1=Data[node].x[1];
	    int y0=Data[node].y[0];
	    int y1=Data[node].y[1];

	    if(x0<grid[0][j]){
		if(x1<grid[0][j]){node_area=-1;}
		x0=grid[0][j];
	    }
	    if(y0<grid[1][k]){
		if(y1<grid[1][k]){node_area=-1;}
		y0=grid[1][k];
	    }
	    if(x1>grid[0][j+1]){
		if(x0>grid[0][j+1]){node_area=-1;}
		x1=grid[0][j+1];
	    }
	    if(y1>grid[1][k+1]){
		if(y0>grid[1][k+1]){node_area=-1;}
		y1=grid[1][k+1];
	    }
	    if(node_area==0){node_area=(x1-x0)*(y1-y0);}
	    area+=node_area;
	}
	if(area>max_area){max_area=area;main=checkingGP[i];}
    }
    return main-1;
}

int compare_color_area(int***& eachArea, int** grid, Shape* Data, Graph* ourGraph, int* boxbounding, int omega, int* assignment_list,int* checkingGP){
    for(int i = 1;i<checkingGP[0] + 1; i++)
    {
	each_Area(eachArea, grid, Data, &ourGraph[checkingGP[i]], boxbounding, omega, assignment_list[i]);
    }
    for(int j = 1; j<grid[0][0] + 1; j++)
    {	
	for(int k = 1; k<grid[1][0] + 1;k++)
	{
	    cout<<"for("<<j<<","<<k<<"):";
	    int diff=abs(eachArea[j][k][0]-eachArea[j][k][1]);
	    cout<<diff<<endl;
	}
    }
}*/

int* Color_2coloring::exec(int argc, char argv[_ParsingNum_][_BUFFSIZE_], Shape* Data, int omega, int dataSize) {

    TmUsage tmusg;
    TmStat stat;
    tmusg.periodStart();

    int i, j, k;
    char ourBuf[_BUFFSIZE_];
    char parsingBuf_write[_ParsingNum_][_BUFFSIZE_], parsingBuf_read[_ParsingNum_][_BUFFSIZE_];

    for(j = 0; j< _ParsingNum_ - 2; j++)
	sprintf(parsingBuf_write[j], "%d", j);
    sprintf(parsingBuf_write[_ParsingNum_ - 2], _OutPutFile_);
    sprintf(parsingBuf_read[0], _Reading_);

//	TODO:	checking which groups are 2colorable (if 2colorabel, then push_back in checkingGP from the lowest number to the highest)	
    int howmanygroups = atoi( argv[1] );
    int legalGPNum = 0;
    int* checkingGP = new int[howmanygroups + 1];
    Graph* ourGraph = new Graph[howmanygroups + 1];
    for(i = 0; i < howmanygroups; i++)
    {
		sprintf(parsingBuf_read[1], "inputs/gn%d.dot", i + 1);
		sprintf(parsingBuf_write[_ParsingNum_ - 1], "%d", i + 1);
		reRead(2, parsingBuf_read, &ourGraph[i + 1]);
		if(detect_2coloring(6, parsingBuf_write, &ourGraph[i+1]))
		{
		    checkingGP[legalGPNum + 1] = i + 1;
		    legalGPNum += 1;
		}
		//else
		//    printf("gn%d is NOT 2colorable\n\n", i + 1);
	}
    checkingGP[0] = legalGPNum;

    //	find boundings
    int* boxbounding = findBoundingBox(omega, Data, dataSize, checkingGP);

    //	claculate grid
    int** grid = CalGrid(boxbounding, omega);

    //	initialize and empty eachArea
    int*** eachArea = new int**[ grid[0][0]+ 1 ];

    for (i= 1; i< grid[0][0] + 1; i++) {
		eachArea[i] = new int*[ grid[1][0]+ 1 ];
		for (j= 0;j< grid[1][0] + 1; j++)
		{
		    eachArea[i][j] = new int[4];
			eachArea[i][j][3] = 0;
		}
    }

    //	initialize and empty Contributor_list, Area_list
    int*** Contributor_list = new int**[grid[0][0] + 1];
    int*** Area_list = new int**[grid[0][0] + 1];
    for(i = 0; i< grid[0][0] + 1;i++)
    {
    	Contributor_list[i] = new int*[grid[1][0] + 1];
    	Area_list[i] = new int*[grid[1][0] + 1];
    	for(j = 0;j<grid[1][0] + 1;j++)
    	{
    		Contributor_list[i][j] = new int[_MaxContributor_];
    		Contributor_list[i][j][0] = 0;
    		Area_list[i][j] = new int[_MaxContributor_];
    	}
    }
    empty_ContributorList(Contributor_list, grid[0][0], grid[1][0]);
    empty_ContributorList(Area_list, grid[0][0], grid[1][0]);

    //	array to store assignment
    int assignment_list[checkingGP[0]+1];

//	TDOO: initial score calculation
    empty_eachArea( eachArea, grid[0][0], grid[1][0] );
    srand(_LuckyNum_);
    
    for(i = 1;i<checkingGP[0] + 1; i++)
    {
    	//printf("<group == %d: ", checkingGP[i]);
    	//for(j = 0;j<ourGraph[checkingGP[i]].nodes.size();j++)
    	//	printf("node == %d ", ourGraph[checkingGP[i]].nodes[j]->id);
    	//printf(">\n");

    	assignment_list[i] = 2*(rand()%2) - 1;
    	each_Area(eachArea, grid, Data, &ourGraph[checkingGP[i]], boxbounding, omega, assignment_list[i], Contributor_list, Area_list, checkingGP[i]);
    }

    double ans = 30;
    double OMEGA = omega;
    for(i = 1;i<grid[0][0] + 1; i++)
    	for(j = 1;j<grid[1][0] + 1;j++)
	    {
	    	if(70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA)  > 0 )
	    		ans += 70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA);
	    	else
	    		ans -= 70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA);
    	}
    printf("================================================\n");
    printf("initial ans == %lf\n", ans);

    int substitute, temp_sub, subcounter, our_ind;
    int indexArr[checkingGP[0]];
    for(j = 0; j< checkingGP[0] + 1; j++)
    	indexArr[j] = 0;
    
    printf("\nBefore being assigned by this algorithm: \n");
    for(i = 1;i < checkingGP[0] + 1; i++)
    	printf("	group == %d, assignment == %d\n",i, assignment_list[i]);
    printf("================================================\n");

//	TODO: algorithm implementation
    int temp2temp[grid[0][0] + 1][grid[1][0] + 1];
    for(i = 1;i<grid[0][0] + 1; i++)
    {	
    	for(j = 1;j<grid[1][0] + 1;j++)
    	{
    		substitute =  eachArea[i][j][1] - eachArea[i][j][0];
    		subcounter = Contributor_list[i][j][0];
    		//printf("i == %d, j == %d; subcounter == %d, substitute == %d\n",i,j, subcounter, substitute);
    		while(subcounter > 0)
    		{
	    		if(substitute > 0)
	    			temp_sub = -1;
	    		else if(substitute < 0)
	    			temp_sub = 1;
	    		for(k = 1; k < Contributor_list[i][j][0] + 1; k++)
	    		{
	    			//printf("finding: substitute == %d, Contributor_list[i][j][k] == %d, Area_list[i][j][k] == %d\n",substitute, Contributor_list[i][j][k], Area_list[i][j][k]);
	    			if(substitute > 0 && indexArr [ Contributor_list[i][j][k] ] != 1)
	    			{
	    				if(Area_list[i][j][k] > 0 && Area_list[i][j][k] > temp_sub && abs((-2) *Area_list[i][j][k] + substitute) < substitute)
	    				{
	    					temp_sub = Area_list[i][j][k];
	    					our_ind = Contributor_list[i][j][k];
	    					//printf("IN~~~~~~~area > 0~~~\n");	
	    					//printf("temp_sub == %d, our_ind == %d\n", temp_sub, our_ind);
	    				}
	    			}
	    			else if(substitute < 0 && indexArr [ Contributor_list[i][j][k] ] != 1)
	    			{
	    				if(Area_list[i][j][k] < 0 && Area_list[i][j][k] < temp_sub && abs((-2) *Area_list[i][j][k] + substitute) < (-1)*substitute)
	    				{
	    					temp_sub = Area_list[i][j][k];
	    					our_ind = Contributor_list[i][j][k];
	    					//printf("IN~~~~~~~area < 0~~~\n");
	    					//printf("temp_sub == %d, our_ind == %d\n", temp_sub, our_ind);
	    				}
	    			}
	    			else
	    				continue;
	    			//printf("~~~~num of Contributors == %d\n", Contributor_list[i][j][0]);
	    		}

	    		if(substitute == 0 || (substitute < 0 && temp_sub > 0) || (substitute > 0 && temp_sub < 0))
	    		{
	    			subcounter = 0;
	    			for(k = 1; k < Contributor_list[i][j][0] + 1; k++)
	    			{
	    				our_ind = Contributor_list[i][j][k];
	    				if(indexArr[our_ind] == 0)
	    					indexArr[our_ind] = 1;
	    			}
	    		}
	    		else if((substitute > 0 && temp_sub > 0) || (substitute < 0 && temp_sub < 0))
	    		{
	    			substitute -= 2*temp_sub;
	    			indexArr[our_ind] = 1;
	    			//printf("@@@our index == %d@@@\n", our_ind);
	    			assignment_list[our_ind] = (-1) * assignment_list[our_ind];
	    			subcounter -= 1;
	    		}
    		}
    		temp2temp[i][j] = substitute;
    		//printf("i == %d, j == %d, temp2temp[i][j] = %d\n\n", i,j,temp2temp[i][j]);
    	}
    }

//	TODO: calculation of the score determined by the assignment determined by this algorithm
    empty_eachArea( eachArea, grid[0][0], grid[1][0] );
    for(i = 1;i < checkingGP[0] + 1; i++)
    	each_Area(eachArea, grid, Data, &ourGraph[checkingGP[i]], boxbounding, omega, assignment_list[i], Contributor_list, Area_list, checkingGP[i]);

    printf("\n\n================================================\n");
    printf("\nAfter being assigned by this algorithm: \n");
    for(i = 1;i < checkingGP[0] + 1; i++)
    	printf("	group == %d, assignment == %d\n",i, assignment_list[i]);

    ans = 30;
    for(i = 1;i<grid[0][0] + 1; i++)
    	for(j = 1;j<grid[1][0] + 1;j++)
	    {
	    	if(70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA)  > 0 )
	    		ans += 70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA);
	    	else
	    		ans -= 70/double(grid[0][0] * grid[1][0]) -  double( 100* abs( eachArea[i][j][1] - eachArea[i][j][0] ) )/ (5* OMEGA * OMEGA);
    	}
    printf("\nalgorithm ans == %lf\n", ans);
    printf("================================================\n");

    return checkingGP;
    
    /*
    for(j = 1; j<grid[0][0] + 1; j++)
    {	
	for(k = 1; k<grid[1][0] + 1;k++)
	{
	    int main=main_group_in_grid(grid,j,k,Data,ourGraph,checkingGP);
	    int area_a,area_b,diff;
	    area_a=area_b=0;
	    cout<<"grid:("<<j<<","<<k<<")"<<endl;
	    cout<<"main_group:"<<main<<endl;
	    cout<<"coloring assignment compare:"<<endl;
	    empty_eachArea(eachArea, grid[0][0], grid[1][0]);
	    each_Area(eachArea, grid, Data, &ourGraph[checkingGP[main]], boxbounding, omega, 1);
	    for(int a = 1; a<grid[0][0] + 1; a++){
		for(int b = 1; b<grid[1][0] + 1;b++){
		    area_a+=eachArea[a][b][0];
		    area_b+=eachArea[a][b][1];
		}
	    }
	    diff=abs(area_a-area_b);
	    empty_eachArea(eachArea, grid[0][0], grid[1][0]);
	    each_Area(eachArea, grid, Data, &ourGraph[checkingGP[main]], boxbounding, omega, -1);
	    for(int a = 1; a<grid[0][0] + 1; a++){
		for(int b = 1; b<grid[1][0] + 1;b++){
		    area_a+=eachArea[a][b][0];
		    area_b+=eachArea[a][b][1];
		}
	    }
	    if(diff<abs(area_a-area_b)){cout<<"assign color: 1"<<endl;}
	    else{cout<<"assign color: -1"<<endl;assignment_list[main]=-1;}
	    cout<<endl;
	}
	//sleep(2);
    }

    //compare color area

    empty_eachArea(eachArea, grid[0][0], grid[1][0]);
    compare_color_area(eachArea,grid,Data,ourGraph,boxbounding,omega,assignment_list,checkingGP);
	*/

}

void each_window_density (int*** Window, int width, int height) {
    for (int i= 1; i<= width; i++) {
	for (int j= 1; j<= height; j++) {
	    Window[i][j][2]= Window[i][j][1] - Window[i][j][0];
	    cout << " " << i << ", " << j << " = " << Window[i][j][2] << endl;
	}
    }
}
