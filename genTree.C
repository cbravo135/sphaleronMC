#include <iostream>
#include <cmath>
#include <vector>

#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"

#include "sphalerons.h"

#include "genTree.h"
#define NPART 12

using namespace std;


int main()
{
    int Nevt = 10000;
    int NF = 0;
    double maxwt = 0.0;
    vector<double> m2;
    TRandom3 rand;
    rand.SetSeed(0);
    vector<TLorentzVector> daughters;
    TLorentzVector mom(0.0,0.0,0.0,9100.0);
    TLorentzVector u1(0.0,0.0,0.0,0.0);
    TLorentzVector u2(0.0,0.0,0.0,0.0);
    fillSphal();
    double masses[NPART];
    double weight = 0.0;

    TGenPhaseSpace gen;
    double x1 = 0.0;
    double x2 = 0.0;
    double Q2 = 0.0;
    double momM = 0.0;
    double pz = 0.0;

    TF1 pdfu("pdfu","5.1072*(x^(0.8))*(1-x)^3/x",0.0,1.0);


    for(int i = 0; i < NPART; i++)
    {
        masses[i] = sphal[0][i];
        cout << "masses[" << i << "] = " << masses[i] << endl;
    }


    TFile *myF = new TFile("mcTree.root","RECREATE","Holds daughters from sphaleron decay");

    TH1D *x1_h = new TH1D("x1_h","x1 inclusive",1000,0.0,1.0);

    TTree *myT = new TTree("mcTree","mcTree");
    myT->Branch("daughters",&daughters); 
    myT->Branch("weight",&weight); 
    myT->Branch("m2",&m2); 
    myT->Branch("pz",&pz); 
    myT->Branch("x1",&x1); 
    myT->Branch("x2",&x2); 
    myT->Branch("Q2",&Q2); 
    myT->Branch("momM",&momM); 

    while(NF < Nevt)
    {
        daughters.clear();
        m2.clear();

        Q2 = 0.0;
        while(Q2 < 9100*9100)
        {
            x1 = pdfu.GetRandom();
            x2 = pdfu.GetRandom();
            Q2 = x1*x2*13000*13000;
            x1_h->Fill(x1);
        }

        u1.SetXYZM(0.0,0.0,x1*6500,0.0);
        u2.SetXYZM(0.0,0.0,x2*(-6500),0.0);
        mom = u1 + u2;
        momM = mom.M();
        pz = mom.Pz();

        gen.SetDecay(mom, NPART, masses);

        weight = gen.Generate();

        for(int ii = 0; ii < NPART; ii++)
        {
            TLorentzVector prod = *gen.GetDecay(ii);
            daughters.push_back(prod);
        }

        TLorentzVector p[NPART];

        for(int i = 0; i < NPART; i++)
        {
            for(int ii = 0; ii < NPART; ii++)
            {
                if(i != ii)p[i] = p[i] + daughters.at(ii);
            }
            m2.push_back(p[i].M2());
        }

        if(weight > maxwt) 
        {
            maxwt = weight;
            cout << "MaxWt: " << maxwt << endl;
        }


        //if(0.146556*rand.Uniform() < weight && m2[3] < 1100 && m2[3] > 900) 
        if(1.19751e-09*rand.Uniform() < weight) 
        {
            myT->Fill();
            NF++;
            if(NF%100 == 0) cout << "Produced Event " << NF << endl;
        }
    }

    cout << "Max Weight: " << maxwt << endl;

    x1_h->Write();
    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
