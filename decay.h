#include <iostream>
#include <cmath>

#include "TRandom3.h"
#include "TLorentzVector.h"
#include "TVector3.h"

struct biP 
{
    TLorentzVector p1;
    TLorentzVector p2;
};

biP decayTwo(TLorentzVector mother,double m1, double m2)
{
    TRandom3 rand;
    rand.SetSeed();

    //Get boost vector
    TVector3 boost;
    boost = mother.BoostVector();

    //Decay in random direction
    double mass = mother.M();
    double th1 = acos(2.0*rand.Uniform()-1.0);
    double phi1 = 2.0*M_PI*rand.Uniform() - M_PI;

    //Solve for everything else
    double E1 = (mass*mass - m2*m2 + m1*m1)/(2.0*mass);
    double E2 = mass - E1;
    double mp1 = sqrt(E1*E1 - m1*m1);

    TVector3 p1(mp1*cos(phi1)*sin(th1),mp1*sin(phi1)*sin(th1),mp1*cos(th1));
    TVector3 p2 = -1.0*p1;

    biP output;
    output.p1.SetPxPyPzE(p1.Px(),p1.Py(),p1.Pz(),E1);
    output.p2.SetPxPyPzE(p2.Px(),p2.Py(),p2.Pz(),E2);

    //Boost into lab frame
    output.p1.Boost(boost);
    output.p2.Boost(boost);

    return output;


};
