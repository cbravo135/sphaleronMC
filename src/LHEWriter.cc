#include "../include/LHEWriter.h"
#include <iostream>
using namespace std;

LHEWriter::LHEWriter(string fName)
{
    oF.open((fName+".lhe").c_str());
    oF << std::scientific;
    oF << "<LesHouchesEvents version=\"1.0\">" << endl;
    oF << "<header>" << endl;
    oF << "</header>" << endl;
    oF << "<init>" << endl;
    oF << "\t2212 2212 0.65000000000e+04 0.65000000000e+04 -1 -1 -1 -1 3 1" << endl;
    oF << "\t0.1e+01 0.1e-01 0.1e-02 7000" << endl;
    oF << "</init>" << endl;
}

LHEWriter::~LHEWriter()
{
}

int LHEWriter::writeEvent(vector<particle> outParts)
{
    //cout << "size check: " << decayK.size() << "\t" << decayPIDs.size() << "\t" << decayColz.size() << endl;
    oF << "<event>" << endl;
    oF << "\t" << outParts.size() << " 7000 1 -1 -1 -1" << endl;
    for(int i = 0; i < int(outParts.size()); i++)
    {
        if(i < 2) oF << "\t" << outParts[i].pid << "\t-1\t0\t0\t";
        else 
        {
            oF << "\t" << outParts[i].pid;
            if(fabs(outParts[i].pid) > 1000) oF << "\t2\t";
            else oF << "\t1\t";
            if(i > 2 && i < 6) oF << "3\t3\t";
            else if(i > 6 && i < 10) oF << "7\t7\t";
            else oF << "1\t2\t";
        }
        
        if(outParts[i].pid > 0) oF << outParts[i].color << "\t0\t";
        else oF << "0\t"  << outParts[i].color << "\t";

        oF << outParts[i].p4v.Px() << "\t";
        oF << outParts[i].p4v.Py() << "\t";
        oF << outParts[i].p4v.Pz() << "\t";
        oF << outParts[i].p4v.E() << "\t";
        oF << outParts[i].p4v.M() << "\t";
        oF << "0\t9" << endl;
    }
    oF << "</event>" << endl;
    return 1;
}

int LHEWriter::close()
{
    oF << "</LesHouchesEvents>" << endl;
    oF.close();
    return 1;
}
