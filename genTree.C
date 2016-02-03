#include <iostream>
#include <cmath>

#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"

#include "decay.h"
#include "sphalerons.h"

#pragma link C++ class std::vector < std::vector<TLorentzVector> >+;

int makeMC()
{
    int Nevt = 1000000;
    int NF = 0;
    int Npart = 3;
    double maxwt = 0.0;
    double m12 = 0.0;
    double m13 = 0.0;
    TRandom3 rand;
    rand.SetSeed(0);
    vector<TLorentzVector> daughters;
    TLorentzVector mom(0.0,0.0,0.0,9100.0);
    TGenPhaseSpace gen;
    fillSphal();
    double masses[3] = {0.0,0.0,0.0};
    double weight = 0.0;

    /*for(int i = 0; i < 12; i++)
    {
        masses[i] = sphal[0][i];
        cout << "masses[" << i << "] = " << masses[i] << endl;
    }*/

    gen.SetDecay(mom, Npart, masses);

    TFile *myF = new TFile("mcTree.root","RECREATE","Holds daughters from sphaleron decay");

    TTree *myT = new TTree("mcTree","mcTree");
    myT->Branch("daughters",&daughters); 
    myT->Branch("weight",&weight); 
    myT->Branch("m12",&m12); 
    myT->Branch("m13",&m13); 

    while(1)
    {
        daughters.clear();

        weight = gen.Generate();

        for(int ii = 0; ii < Npart; ii++)
        {
            TLorentzVector prod = *gen.GetDecay(ii);
            daughters.push_back(prod);
        }

        TLorentzVector p1 = daughters.at(0);
        TLorentzVector p2 = daughters.at(1);
        TLorentzVector p3 = daughters.at(2);

        TLorentzVector p12 = p1 + p2;
        TLorentzVector p13 = p1 + p3;

        m12 = p12.M2();
        m13 = p13.M2();

        if(weight > maxwt) 
        {
            maxwt = weight;
            cout << "MaxWt: " << maxwt << endl;
        }

        //cout << gen.GetWtMax() << ": " << weight << endl;


        if(0.3849*rand.Uniform() < weight) 
        {
            myT->Fill();
            NF++;
            if(NF%10000 == 0) cout << "Produced Event " << NF << endl;
        }
        if(NF >= Nevt) break;
    }

    cout << "Max Weight: " << maxwt << endl;

    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
