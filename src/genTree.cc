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

#include "../include/configBuilder.h"
#include "../include/genTree.h"
#include "../include/LHEWriter.h"

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
    double masses[15];

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

    TH1D *x1_h = new TH1D("x1_h","x1 inclusive",1000,0.0,1.0);
    TH1D *sumInterQ3_h = new TH1D("sumInterQ3_h","Intermidiate particle charges",21,-10.5,10.5);

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

    configBuilder confBuild;

    while(NF < Nevt)
    {
        daughters.clear();
        m2.clear();

        vector<int> decayPID;
        vector<int> decayColz;
        //For now both incoming quarks are ups
        decayPID.push_back(UQ_PID);
        decayColz.push_back(501);
        decayPID.push_back(UQ_PID);
        decayColz.push_back(502);
        int colNow = 503;
        //Now the sphaleron mediator
        //decayPID.push_back(-10000099);
        //decayColz.push_back(503);

        vector<particle> confBuf = confBuild.build(UQ_PID,UQ_PID);

        int Nline = 0;
        for(int i = 0; i < confBuf.size(); i++)
        {
            masses[i] = confBuf[i].mass;
        }

        Q2 = 0.0;
        while(Q2 < thr*thr)
        {
            x1 = pdfu.GetRandom();
            x2 = pdfu.GetRandom();
            Q2 = x1*x2*13000*13000;
            pdfN++;
            x1_h->Fill(x1);
        }

        u1.SetXYZM(0.0,0.0,x1*6500,UQ_MASS);
        u2.SetXYZM(0.0,0.0,x2*(-6500),UQ_MASS);
        daughters.push_back(u1);
        daughters.push_back(u2);
        mom = u1 + u2;
        //daughters.push_back(mom);
        momM = mom.M();
        pz = mom.Pz();

        weight = 1.0;
        while(weight > maxW*rand.Uniform())
        {
            gen.SetDecay(mom, confBuf.size(), masses);

            weight = gen.Generate();
        }

        vector<TLorentzVector> decayBuf;
        for(int ii = 0; ii < confBuf.size(); ii++)
        {
            TLorentzVector prod = *gen.GetDecay(ii);
            decayBuf.push_back(prod);
        }

        int Nq = 0;
        vector<int> qI;
        int Nlep = 0;
        vector<int> lepI;
        int Ninter = 0;
        for(int i = 0; i < confBuf.size(); i++)
        {
            if(fabs(confBuf[i].pid) > 7) //lepton
            {
                lepI.push_back(i);
                Nlep++;
            }
            else //quark
            {
                qI.push_back(i);
                Nq++;
                if(qI.size() == 3 && Ninter < 2)
                {
                    TLorentzVector interP(0.0,0.0,0.0,0.0);
                    int interQ3 = 0;
                    for(int ii = 0; ii < qI.size(); ii++)
                    {
                        interP = interP + decayBuf[qI[ii]];
                        interQ3 += confBuf[qI[ii]].q3;
                    }
                    sumInterQ3_h->Fill(interQ3);
                    daughters.push_back(interP);
                    decayColz.push_back(0);
                    if(interQ3 == 0) decayPID.push_back(1000022);
                    if(fabs(interQ3) == 3) decayPID.push_back(interQ3*1006213/fabs(interQ3));
                    if(fabs(interQ3) == 6) decayPID.push_back(interQ3*1006223/fabs(interQ3));
                    for(int ii = 0; ii < qI.size(); ii++)
                    {
                        daughters.push_back(decayBuf[qI[ii]]);
                        decayColz.push_back(colNow++);
                        decayPID.push_back(confBuf[qI[ii]].pid);
                    }
                    Ninter++;
                    qI.clear();
                }
            }
        }
        for(int i = 0; i < qI.size(); i++)
        {
            daughters.push_back(decayBuf[qI[i]]);
            if(confBuf.size() == 12 && qI.size() - (i+1) == 0) decayColz.push_back(501);
            else if(confBuf.size() == 14 && qI.size() - (i+1) < 2) decayColz.push_back(501 + qI.size() - (i+1));
            else decayColz.push_back(colNow++);
            decayPID.push_back(confBuf[qI[i]].pid);
        }
        for(int i = 0; i < lepI.size(); i++)
        {
            daughters.push_back(decayBuf[lepI[i]]);
            decayColz.push_back(0);
            decayPID.push_back(confBuf[lepI[i]].pid);
        }

        TLorentzVector p[20];

        for(int i = 0; i < confBuf.size(); i++)
        {
            for(int ii = 0; ii < confBuf.size(); ii++)
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


        lheF.writeEvent(daughters,decayPID,decayColz);
        myT->Fill();
        NF++;
        if(NF%(Nevt/10) == 0) cout << "Produced Event " << NF << "  pdfN : " << pdfN << endl;
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














