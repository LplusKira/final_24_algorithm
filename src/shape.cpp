// **************************************************************************
//  File       [shape.cpp]
//  Author     [Chien Yu][Po-Kai Chang : line 17~ 45, 189~ 224]
//  Synopsis   []
//  Modify     [2015/05/23 Chien Yu][2015/06/27 Po-Kai Chang]
// **************************************************************************
#include "shape.h"
using namespace std;

void help_message() {
    cout << "usage: ./a.out <input_file> <output_file>" << endl;
}

Classifier::Classifier()
{}

void Classifier::help_you_class(int group_Num, int first_node, int second_node, int first_time) {
    char ourBuf[_BUFFSIZE_];
    sprintf(ourBuf, "inputs/gn%d.dot", group_Num);
    FILE* fp = fopen(ourBuf, "a");
    if(first_time > 0) {
	sprintf(ourBuf, "graph gn%d {\n", group_Num);
	fprintf(fp, "%s", ourBuf);
    }

    //  write in one line of vertex -- vertex;\n
    sprintf(ourBuf, "v%d -- v%d;\n", first_node, second_node);
    fprintf(fp, "%s", ourBuf);
    //printf("%s\n", ourBuf);
    fclose(fp);
}

void Classifier::help_you_ending(int total_group_Num)
{
    int i;
    char ourBuf[_BUFFSIZE_];

    for(i = 1; i<= total_group_Num; i++) {
	sprintf(ourBuf, "inputs/gn%d.dot", i);
	FILE* fp = fopen(ourBuf, "a");
	fprintf(fp, "}\n");
	fclose(fp);
    }

}

ProtocolAndGroup Classifier::classify(int arg_num, char* argvector[]) {
    //////////// read the input file /////////////

    char buffer[10], comma;
    int alpha, beta, omega, in_count = 0;
    int x1, y1, x2, y2;
    vector<int> x1v, y1v, x2v, y2v;
    fstream fin(argvector[1]);
    fstream fout;
    fout.open(argvector[2],ios::out);

    fin.getline(buffer,10,'=');
    fin >> alpha;
    fin.getline(buffer,10,'=');
    fin >> beta;
    fin.getline(buffer,10,'=');
    fin >> omega;

    while (fin >> x1 >> comma >> y1 >> comma >> x2 >> comma >> y2) {
	x1v.push_back(x1);
	y1v.push_back(y1);
	x2v.push_back(x2);
	y2v.push_back(y2);

	in_count++;
    }

    Data = new Shape [in_count];

    for (int i=0; i< in_count; i++) {
	Data[i].x[0] = x1v[i];
	Data[i].y[0] = y1v[i];
	Data[i].x[1] = x2v[i];
	Data[i].y[1] = y2v[i];
	Data[i].color = 0;
    }

    //////////// convert shape to graph ////////////

    int group[in_count],i,j, gcnt = 0;
    int index_group[in_count];
    for (i=0; i< in_count; i++) {
	group[i] = -1;
	index_group[i] = 0;
    } 

    vector<int> each_group_size;
    int is_first_time = 0;
    for (i=0; i< in_count; i++) {
	for (j=0; j< in_count-1; j++) {
	    if (i!=j) {
		if (Data[i].x[1] > Data[j].x[0] && Data[i].x[0] <= Data[j].x[0]) {
		    if (Data[i].y[1] < Data[j].y[0] && Data[j].y[0]-Data[i].y[1] < beta) { 
			if (group[i] < 0 && group[j] < 0) { group[i] = ++gcnt; group[j] = gcnt; }
			else {
			    if (group[i] > 0) 	group[j] = group[i];
			    else		group[i] = group[j];
			}
			//  write in right group
			if (index_group[group[i]-1] == 0) {
			    help_you_class(group[i], i, j, 1);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			    index_group[group[i]-1] = 1;
			}
			else {
			    help_you_class(group[i], i, j, 0);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			}
		    }
		    if (Data[i].y[0] > Data[j].y[1] && Data[i].y[0]-Data[j].y[1] < beta) {
			if (group[i] < 0 && group[j] < 0) { group[i] = ++gcnt; group[j] = gcnt; }
			else {
			    if (group[i] > 0) 	group[j] = group[i];
			    else		group[i] = group[j];
			}
			//  write in right group
			if (index_group[group[i]-1] == 0) {
			    help_you_class(group[i], i, j, 1);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			    index_group[group[i]-1] = 1;
			}
			else {
			    help_you_class(group[i], i, j, 0);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			}
		    }
		}
		if (Data[i].y[1] > Data[j].y[0] && Data[i].y[0] <= Data[j].y[0]) {
		    if (Data[i].x[1] < Data[j].x[0] && Data[j].x[0]-Data[i].x[1] < alpha) {
			if (group[i] < 0 && group[j] < 0) { group[i] = ++gcnt; group[j] = gcnt; }
			else {
			    if (group[i] > 0)	group[j] = group[i];
			    else                group[i] = group[j];
			}
			//  write in right group
			if (index_group[group[i]-1] == 0) {
			    help_you_class(group[i], i, j, 1);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			    index_group[group[i]-1] = 1;
			}
			else {
			    help_you_class(group[i], i, j, 0);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			}
		    }   
		    if (Data[i].x[0] > Data[j].x[1] && Data[i].x[0]-Data[j].x[1] < alpha) {
			if (group[i] < 0 && group[j] < 0) { group[i] = ++gcnt; group[j] = gcnt; }
			else {
			    if (group[i] > 0)	group[j] = group[i];
			    else                group[i] = group[j];
			}
			//  write in right group
			if (index_group[group[i]-1] == 0) {
			    help_you_class(group[i], i, j, 1);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			    index_group[group[i]-1] = 1;
			}
			else {
			    help_you_class(group[i], i, j, 0);
			    Data[i].group_index = group[i];
			    Data[j].group_index = group[i];
			}
		    }
		}
	    }
	}
    }
    for (int i; i < in_count; i++) { 
	if (group[i] < 0) { 
	    group[i] = ++gcnt;
	    help_you_class(group[i], i, i, 1);
	}
	Data[i].group_index = group[i];
    }

    ProtocolAndGroup our_protocol_and_group;
    
    //  collect each group has how many elements
    for(i = 0;i<gcnt + 1; i++)
	each_group_size.push_back(0);
    for(i= 0;i<in_count; i++)
	each_group_size[Data[i].group_index] += 1;

    //  initializing returning Group; 
    our_protocol_and_group.Group = new int* [gcnt + 1];
    for(i = 1;i<gcnt + 1;i++)
	our_protocol_and_group.Group[i] = new int[each_group_size[i] + 1];

    //  assigning each group's (from 1 to gcnt) member numbers, first member, second member, ..., last member
    int* counter = new int[gcnt + 1]; // count where to put in each group
    for(i = 1; i< gcnt + 1; i++) {
	counter[i] = 1;
	our_protocol_and_group.Group[i][0] = each_group_size[i];
    }
    for(i = 0;i< in_count; i++) {
	our_protocol_and_group.Group[ Data[i].group_index ][ counter[ Data[i].group_index ] ] = i;
	counter[ Data[i].group_index ] += 1;
    }

    //  end each file
    help_you_ending(gcnt);

    fin.close();
    fout.close();

    our_protocol_and_group.protocal_list = new int[_sameParsingNum_];
    our_protocol_and_group.protocal_list[0] = gcnt;
    our_protocol_and_group.protocal_list[1] = in_count;
    our_protocol_and_group.protocal_list[2] = omega;

    return our_protocol_and_group;
}

