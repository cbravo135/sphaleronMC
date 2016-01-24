#include <iostream>
#include <cmath>

#include "TLorentzVector.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TFile.h"

#pragma link C++ class std::vector < std::vector<TLorentzVector> >+;

int makeMC()
{
    int Nevt = 100;
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
        for(int p = 0; p < 12; p++)
        {
            ct = 2*rand.Uniform() - 1;
            t = acos(ct);
            eta = -1.0*log(tan(t/2.0));
            phi = 2.0*M_PI*rand.Uniform();
            pt = 80.0*rand.Uniform()+20;
            
            TLorentzVector dghtr;
            dghtr.SetPtEtaPhiM(pt,eta,phi,0.0);
            daughters.push_back(dghtr);
        }
        myT->Fill();
    }

    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
