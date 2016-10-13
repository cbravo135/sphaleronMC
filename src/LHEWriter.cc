#include "../include/LHEWriter.h"

LHEWriter::LHEWriter(string fName)
{
    oF.open((fName+".lhe").c_str());
    oF << std::scientific;
    oF << "<LesHouchesEvents version=\"1.0\">" << endl;
    oF << "<header>" << endl;
    oF << "</header>" << endl;
    oF << "<init>" << endl;
    oF << "\t2212 2212 0.65000000000e+04 0.65000000000e+04 0 0 263000 263000 3 1" << endl;
    oF << "\t0.1e+01 0.1e-01 0.1e-02 1" << endl;
    oF << "</init>" << endl;
}

LHEWriter::~LHEWriter()
{
}

int LHEWriter::writeEvent(vector<TLorentzVector> decayK, vector<int> decayPIDs, vector<int> decayColz)
{
    oF << "<event>" << endl;
    oF << "\t" << decayK.size() << " 1 1 -1 -1 -1" << endl;
    for(int i = 0; i < int(decayK.size()); i++)
    {
        if(i < 2) oF << "\t" << decayPIDs.at(i) << " -1 0 0 ";
        else oF << "\t" << decayPIDs.at(i) << " 1 1 2 ";
        //else if (i == 2) oF << "\t" << decayPIDs.at(i) << " 2 1 2 ";
        if(decayPIDs[i] > 0) oF << decayColz[i] << " 0 ";
        else oF << "0 "  << decayColz[i] << " ";
        oF << decayK[i].Px() << " ";
        oF << decayK[i].Py() << " ";
        oF << decayK[i].Pz() << " ";
        oF << decayK[i].E() << " ";
        oF << decayK[i].M() << " ";
        oF << "0 9" << endl;
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
