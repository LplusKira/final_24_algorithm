// **************************************************************************
// File       [ main.cpp ]
// Author     [ Po-Kai Chang ]
// Synopsis   [  ]
// Date       [ 2015/06/27 modified ]
// **************************************************************************

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
#include "../lib/tm_usage.h"
#include "sys_cmd.h"
#include "user_cmd.h"
#include "graph.h"
#include "shape.h"
using namespace CommonNs;

void initOpts(OptMgr *mgr);
void initCmds(CmdMgr *mgr);

int main(int argc, char **argv) {

    //  TODO: creating files
    Classifier ourClassifier;
    ProtocolAndGroup our_protocol_and_group = ourClassifier.classify(argc, argv);
    int i, j;

    TmUsage tmusg;
    TmStat stat;
    tmusg.periodStart();

    //  TODO: finding who are 2colorable
    char parsingBuf[_ParsingNum_][_BUFFSIZE_];
    Color_2coloring our2Coloring;
    sprintf(parsingBuf[1], "%d", our_protocol_and_group.protocal_list[0]);

    int* checkingGP = our2Coloring.exec(2, parsingBuf, ourClassifier.Data, our_protocol_and_group.protocal_list[2], our_protocol_and_group.protocal_list[1]);

    tmusg.getPeriodUsage(stat);
    printf("runtime = %lf sec\n", (stat.uTime + stat.sTime) / 1000000.0);
    printf("memory = %lf MB\n", stat.vmPeak / 1000.0);

    return 0;
}





