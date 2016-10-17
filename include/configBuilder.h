#include "particles.h"

#include <vector>

using namespace std;

class configBuilder
{
    private:
        vector<particle> parts;

    public:
        configBuilder();
        ~configBuilder();

        vector<particle> build(int dCS);

};

