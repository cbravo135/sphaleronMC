#include <iostream>
#include <stdlib.h> 
#include <cmath>
#include <vector>

#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"

#include "../include/particles.h"
#include "../include/genTree.h"
#include "../include/LHEWriter.h"

#define NPART 12
#define ARGS 3

using namespace std;

int main(int argc, char* argv[])
{

    if(argc != ARGS+1)
    {
        cout << "./genTree threshold maxweight Nevents" << endl;
        return -99;
    }

    float thr = atof(argv[1]);
    float maxW = atof(argv[2]);
    int Nevt = atoi(argv[3]);
    double maxwt = 0;

    int NF = 0;
    vector<double> m2;
    TRandom3 rand;
    rand.SetSeed(0);
    vector<TLorentzVector> daughters;
    TLorentzVector mom(0.0,0.0,0.0,9100.0);
    TLorentzVector u1(0.0,0.0,0.0,0.0);
    TLorentzVector u2(0.0,0.0,0.0,0.0);
    double masses[NPART];
    double sphal[2][12];


    sphal[0][0] = TQ_MASS;
    sphal[1][0] = -1*TQ_PID;

    sphal[0][1] = TQ_MASS;
    sphal[1][1] = -1*TQ_PID;

    sphal[0][2] = BQ_MASS;
    sphal[1][2] = -1*BQ_PID;

    sphal[0][3] = TAU_MASS;
    sphal[1][3] = -1*TAU_PID;

    sphal[0][4] = CQ_MASS;
    sphal[1][4] = -1*CQ_PID;

    sphal[0][5] = CQ_MASS;
    sphal[1][5] = -1*CQ_PID;

    sphal[0][6] = SQ_MASS;
    sphal[1][6] = -1*SQ_PID;

    sphal[0][7] = MU_MASS;
    sphal[1][7] = -1*MU_PID;

    sphal[0][8] = DQ_MASS;
    sphal[1][8] = -1*DQ_PID;

    sphal[0][9] = UQ_MASS;
    sphal[1][9] = -1*UQ_PID;

    sphal[0][10] = UQ_MASS;
    sphal[1][10] = -1*UQ_PID;

    sphal[0][11] = ELE_MASS;
    sphal[1][11] = -1*ELE_PID;

    double weight = 0.0;

    TGenPhaseSpace gen;
    double x1 = 0.0;
    double x2 = 0.0;
    double Q2 = 0.0;
    double momM = 0.0;
    double pz = 0.0;

    TFile *myF = new TFile("mcTree.root","RECREATE","Holds daughters from sphaleron decay");
    LHEWriter lheF("sphaleronTest");

    //TF1 pdfu("pdfu","5.1072*(x^(0.8))*(1-x)^3/x",thr*thr/13000.0*13000.0,1.0);
    TF1 pdfu("pdfu","(x^(-1.16))*(1-x)^(1.76)/(2.19*x)",thr*thr/(13000.0*13000.0),1.0);

    vector<int> decayPID;
    vector<int> decayColz;
    //For now both incoming quarks are ups
    decayPID.push_back(UQ_PID);
    decayColz.push_back(501);
    decayPID.push_back(UQ_PID);
    decayColz.push_back(502);
    //Now the sphaleron mediator
    //decayPID.push_back(-10000099);
    //decayColz.push_back(503);
    int Nline = 0;
    for(int i = 0; i < NPART; i++)
    {
        masses[i] = sphal[0][i];
        decayPID.push_back(sphal[1][i]);
        if(fabs(sphal[1][i]) > 10) decayColz.push_back(0);
        else decayColz.push_back(503 + Nline++); 
        cout << "masses[" << i << "] = " << masses[i] << endl;
    }

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

    int pdfN = 0;

    while(NF < Nevt)
    {
        daughters.clear();
        m2.clear();

        Q2 = 0.0;
        while(Q2 < thr*thr)
        {
            x1 = pdfu.GetRandom();
            x2 = pdfu.GetRandom();
            Q2 = x1*x2*13000*13000;
            pdfN++;
            x1_h->Fill(x1);
        }

        u1.SetXYZM(0.0,0.0,x1*6500,0.0);
        u2.SetXYZM(0.0,0.0,x2*(-6500),0.0);
        daughters.push_back(u1);
        daughters.push_back(u2);
        mom = u1 + u2;
        //daughters.push_back(mom);
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
                if(i != ii) p[i] = p[i] + daughters[ii];
            }
            m2.push_back(p[i].M2());
        }

        if(weight > maxwt) 
        {
            maxwt = weight;
            cout << "MaxWt: " << maxwt << endl;
        }


        //if(0.146556*rand.Uniform() < weight && m2[3] < 1100 && m2[3] > 900) 
        //if(1.59751e-09*rand.Uniform() < weight) 
        if(maxW*rand.Uniform() < weight) 
        {
            lheF.writeEvent(daughters,decayPID,decayColz);
            myT->Fill();
            NF++;
            if(NF%(Nevt/10) == 0) cout << "Produced Event " << NF << "  pdfN : " << pdfN << endl;
        }
    }

    cout << "Max Weight: " << maxwt << endl;

    lheF.close();
    x1_h->Write();
    pdfu.Write();
    myT->Write();
    myF->Write();
    myF->Close();


    return 0;
};
