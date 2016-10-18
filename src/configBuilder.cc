#include "../include/configBuilder.h"

configBuilder::configBuilder()
{
    particle partBuf;
    partBuf.mass = ELE_MASS;
    partBuf.pid = ELE_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 0

    partBuf.mass = 0;
    partBuf.pid = ENU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 1

    partBuf.mass = UQ_MASS;
    partBuf.pid = UQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 2

    partBuf.mass = DQ_MASS;
    partBuf.pid = DQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 3

    partBuf.mass = MU_MASS;
    partBuf.pid = MU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 4

    partBuf.mass = 0;
    partBuf.pid = MNU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 5

    partBuf.mass = CQ_MASS;
    partBuf.pid = CQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 6

    partBuf.mass = SQ_MASS;
    partBuf.pid = SQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 7

    partBuf.mass = TAU_MASS;
    partBuf.pid = TAU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 8

    partBuf.mass = 0;
    partBuf.pid = TNU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);  // 9

    partBuf.mass = TQ_MASS;
    partBuf.pid = TQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 10

    partBuf.mass = BQ_MASS;
    partBuf.pid = BQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);  // 11
}

configBuilder::~configBuilder()
{
}

vector<particle> configBuilder::build(int dCS)
{
    vector<particle> conf;
    for(int i = 0; i < parts.size(); i++)
    {
        if(i==1 || i==5 || i==9) continue;
        particle partBuf;
        partBuf.mass = parts[i].mass;
        partBuf.pid = dCS*parts[i].pid;
        partBuf.color = parts[i].color;
        conf.push_back(partBuf);
        if(i == 2 || i == 6 || i == 10) conf.push_back(partBuf);
    }
    return conf;
}









