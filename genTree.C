#include <iostream>
#include <cmath>

#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TFile.h"

#include "decay.h"

#pragma link C++ class std::vector < std::vector<TLorentzVector> >+;

int makeMC()
{
    int Nevt = 10000;
    vector<TLorentzVector> daughters;
    vector<double> momMass;
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
    myT->Branch("momMass",&momMass); 

    for(int i = 0; i < Nevt; i++ )
    {
        daughters.clear();
        momMass.clear();

        TLorentzVector dghtr;
        dghtr.SetPxPyPzE(0.0,0.0,0.0,9100.0);
        biP dd = decayTwo(dghtr,9100.0/2.0,9100.0/2.0);
        daughters.push_back(dd.p1);
        daughters.push_back(dd.p2);
        for(int d = 2; d < 6;d++)
        {
            int di = rand.Integer(d);
            TLorentzVector preDecay = daughters.at(di);
            biP postDecay = decayTwo(preDecay, preDecay.M()/2.0, preDecay.M()/2.0);
            daughters.at(di) = postDecay.p1;
            daughters.push_back(postDecay.p2);
        }
        for(int d = 0; d < 6; d++)
        {
            momMass.push_back(daughters.at(d).M());
        }
        for(int d = 0; d < 6; d++)
        {
            TLorentzVector preDecay = daughters.at(d);
            biP postDecay = decayTwo(preDecay,0.0,0.0);
            daughters.at(d) = postDecay.p1;
            daughters.push_back(postDecay.p2);
            momMass.push_back(preDecay.M());
        }
        myT->Fill();
    }

    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
