#include <fstream>
#include <vector>
#include <string>

#include "particles.h"
#include "TLorentzVector.h"

using namespace std;

class LHEWriter
{
    private:
        ofstream oF;

    public:
        LHEWriter(string fName);
        ~LHEWriter();

        int writeEvent(vector<particle> outParts);
        int close();

};
