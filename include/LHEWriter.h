#include <fstream>
#include <vector>
#include <string>

#include "TLorentzVector.h"

using namespace std;

class LHEWriter
{
    private:
        ofstream oF;

    public:
        LHEWriter(string fName);
        ~LHEWriter();

        int writeEvent(vector<TLorentzVector> decayK, vector<int> decayPIDs);
        int close();

};
