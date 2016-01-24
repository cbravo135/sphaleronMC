#include <iostream>
#include <cmath>

#include "TLorentzVector.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TFile.h"

#include "decay.h"

#pragma link C++ class std::vector < std::vector<TLorentzVector> >+;

int makeMC()
{
    int Nevt = 10000;
    vector<TLorentzVector> daughters;
    TRandom3 rand;
    rand.SetSeed(0);
    double ct = 0.0;
    double t = 0.0;
    double pt = 0.0;
    double phi = 0.0;
    double eta = 0.0;

    TFile *myF = new TFile("mcTree.root","RECREATE","Holds daughters from sphaleron decay");

    TTree *myT = new TTree("mcTree","mcTree");
    myT->Branch("daughters",&daughters); 

    for(int i = 0; i < Nevt; i++ )
    {
        daughters.clear();

        TLorentzVector dghtr;
        double M = rand.BreitWigner(91.2,2.5);
        while (M < 10.0)
        {
            M = rand.BreitWigner(91.2,2.5);
        }
        double pz = rand.Gaus(50.0,1.0);
        dghtr.SetPxPyPzE(0.0,0.0,pz,sqrt(M*M+pz*pz));
        if(abs(dghtr.M()) == 0 ) cout << "M: " << M << endl;

        biP dp;

        dp = decayTwo(dghtr,3.0,7.0);
        daughters.push_back(dp.p1);
        daughters.push_back(dp.p2);

        myT->Fill();
    }

    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
