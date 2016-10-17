#include "../include/configBuilder.h"

configBuilder::configBuilder()
{
    particle partBuf;
    partBuf.mass = ELE_MASS;
    partBuf.pid = ELE_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);

    partBuf.mass = UQ_MASS;
    partBuf.pid = TQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);

    partBuf.mass = DQ_MASS;
    partBuf.pid = BQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);

    partBuf.mass = MU_MASS;
    partBuf.pid = MU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);

    partBuf.mass = CQ_MASS;
    partBuf.pid = TQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);

    partBuf.mass = SQ_MASS;
    partBuf.pid = BQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);

    partBuf.mass = TAU_MASS;
    partBuf.pid = TAU_PID;
    partBuf.color = 0;
    parts.push_back(partBuf);

    partBuf.mass = TQ_MASS;
    partBuf.pid = TQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);

    partBuf.mass = BQ_MASS;
    partBuf.pid = BQ_PID;
    partBuf.color = 1;
    parts.push_back(partBuf);
}

configBuilder::~configBuilder()
{
}

vector<particle> configBuilder::build(int dCS)
{
    vector<particle> conf;
    for(int i = 0; i < parts.size(); i++)
    {
        particle partBuf;
        partBuf.mass = parts[i].mass;
        partBuf.pid = dCS*parts[i].pid;
        partBuf.color = parts[i].color;
        conf.push_back(partBuf);
        if(i == 1 || i == 4 || i == 7) conf.push_back(partBuf);
    }
    return conf;
}









