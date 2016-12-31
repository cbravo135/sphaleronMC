#include <iostream>
#include <stdlib.h> 
#include <cmath>
#include <vector>
#include <string>

#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TString.h"

#include "../include/configBuilder.h"
#include "../include/genTree.h"
#include "../include/LHEWriter.h"
#include "../include/mstwpdf.h"
//#include "../include/protonPDF.h"

#define ARGS 4
#define SQRTS 13000.0
#define MCW 3.8e-5

using namespace std;

int main(int argc, char* argv[])
{

    if(argc != ARGS+1)
    {
        cout << "./genTree threshold maxweight Nevents Filename" << endl;
        return -99;
    }

    float thr = atof(argv[1]);
    float maxW = atof(argv[2]);
    int Nevt = atoi(argv[3]);
    string ofName = string(argv[4]);
    double maxwt = 0;

    int NF = 0;
    TRandom3 rand;
    rand.SetSeed(0);
    particleBase *partBase = new particleBase();
    vector<TLorentzVector> daughters;
    TLorentzVector mom(0.0,0.0,0.0,9100.0);
    TLorentzVector u1(0.0,0.0,0.0,0.0);
    TLorentzVector u2(0.0,0.0,0.0,0.0);
    double masses[15];

    double weight = 0.0;

    TGenPhaseSpace gen;
    double x1 = 0.0;
    double x2 = 0.0;
    int iq1 = 0;
    int iq2 = 0;
    double Q2 = 0.0;
    double momM = 0.0;
    double pz = 0.0;

    TFile *myF = new TFile((ofName+".root").c_str(),"RECREATE","Holds daughters from sphaleron decay");
    LHEWriter lheF(ofName);

    double minx = thr*thr/(SQRTS*SQRTS);

    //TF1 pdfu("pdfu","(x^(-1.16))*(1-x)^(1.76)/(2.19*x)",minx,1.0);

    c_mstwpdf *pdf = new c_mstwpdf("/afs/cern.ch/user/b/bravo/work/sphaleron/mc/toy/mstw2008/Grids/mstw2008nnlo.00.dat");
    double maxMCtot = 0.0;

    TH1D *x1_h = new TH1D("x1_h","x1 inclusive",1000,0.0,1.0);
    TH1D *mcTot_h = new TH1D("mcTot_h","Monte Carlo Probabilities",100,0.0,MCW);
    TH1D *sumInterQ3_h = new TH1D("sumInterQ3_h","Intermidiate particle charges",21,-10.5,10.5);
    vector<TH1D> pt_hv;
    for(int iq = -6; iq < 7; iq++)
    {
        string nameBuf;
        string titBuf;
        titBuf = Form("Quark %i p_{T};p_{T} [GeV];Entries / 10 GeV",iq);
        if(iq < 0) nameBuf = Form("iqm%i_h",int(fabs(iq)));
        else nameBuf = Form("iqp%i_h",int(fabs(iq)));
        TH1D hBuf(nameBuf.c_str(),titBuf.c_str(),200,0.0,2000.0);
        pt_hv.push_back(hBuf);
    }

    TTree *myT = new TTree("mcTree","mcTree");
    myT->Branch("daughters",&daughters); 
    myT->Branch("weight",&weight); 
    myT->Branch("pz",&pz); 
    myT->Branch("x1",&x1); 
    myT->Branch("x2",&x2); 
    myT->Branch("iq1",&iq1); 
    myT->Branch("iq2",&iq2); 
    myT->Branch("Q2",&Q2); 
    myT->Branch("momM",&momM); 

    int pdfN = 0;

    configBuilder confBuild;

    while(NF < Nevt)
    {
        daughters.clear();

        Q2 = 0.0;
        double mcP = 1.1;
        bool mcPass = false;
        //Collide protons
        while(!mcPass)
        {
            //Choose x1 and x2 in proper range
            Q2 = 0.0;
            while(Q2 < thr*thr)
            {
                //x1 = proton.sample();
                //x2 = proton.sample();
                x1 = (1.0-minx)*rand.Uniform()+minx;
                x2 = (1.0-minx)*rand.Uniform()+minx;
                Q2 = x1*x2*SQRTS*SQRTS;
            }
            mcP = MCW*rand.Uniform();
            double mcTot = 0.0;
            for(int i1 = -5; i1 < 6; i1++)
            {
                if(i1 == 0) continue;
                iq1 = i1;
                double x1p = pdf->parton(iq1,x1,SQRTS);
                for(int i2 = -5; i2 < 6; i2++)
                {
                    if(i2 == 0) continue;
                    iq2 = i2;
                    double x2p = pdf->parton(iq2,x2,SQRTS);
                    mcTot += x1p*x2p;
                    if(mcTot > mcP) {mcPass = true; break;}
                }
                if(mcPass) break;
            }
            pdfN++;
            x1_h->Fill(x1);
            mcTot_h->Fill(mcTot);
            if(maxMCtot < mcTot) {maxMCtot = mcTot; cout << "Max MC Total: " << maxMCtot << endl;}
        }

        vector<particle> inParts;

        //Build incoming particles, sphaleron, and prepare to decay
        particle partBuf = partBase->getParticle(iq1);
        if(iq1 != partBuf.pid) cout << "iq1 = " << iq1 << " != partBuf.pid = " << partBuf.pid << endl;
        partBuf.color = 501;
        partBuf.p4v.SetXYZM(0.0,0.0,x1*6500,partBuf.mass);
        inParts.push_back(partBuf); //Push first incoming quark

        partBuf = partBase->getParticle(iq2);
        partBuf.color = 502;
        partBuf.p4v.SetXYZM(0.0,0.0,x2*(-6500),partBuf.mass);
        inParts.push_back(partBuf); //Push second incoming quark

        int colNow = 503;

        particle mother;
        mother.p4v = inParts[0].p4v + inParts[1].p4v;
        mother.mass = mother.p4v.M();

        vector<particle> confBuf = confBuild.build(iq1,iq2);
        int Nline = 0;
        for(int i = 0; i < confBuf.size(); i++)
        {
            masses[i] = confBuf[i].mass;
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

        //Extract Decay 4-vectors and assign colors to non-spectator quarks
        for(int ii = 0; ii < confBuf.size(); ii++)
        {
            TLorentzVector prod = *gen.GetDecay(ii);
            confBuf[ii].p4v = prod;
            confBuf[ii].m1 = 0;
            confBuf[ii].m2 = 0;
            if(confBuf[ii].color != 501 && confBuf[ii].color != 502 && fabs(confBuf[ii].pid) < 7) confBuf[ii].color = colNow++;
            if(fabs(confBuf[ii].pid) > 7) confBuf[ii].color = 0;
            int kinI = confBuf[ii].pid + 6;
            if(confBuf[ii].color != 501 && confBuf[ii].color != 502 && fabs(confBuf[ii].pid) < 7) pt_hv[kinI].Fill(confBuf[ii].p4v.Pt());
        }

        vector<particle> g1q;
        vector<particle> g2q;
        vector<particle> g3q;
        vector<particle> leps;
        vector<particle> specParts;

        vector<int> qIg1;
        vector<int> qIg2;
        vector<int> qIg3;
        int Nlep = 0;
        vector<int> lepI;
        //Seperate leptons and quarks
        for(int i = 0; i < confBuf.size(); i++)
        {
            if(confBuf[i].color == 501 || confBuf[i].color == 502) 
            { 
                confBuf[i].m1 = 1;
                confBuf[i].m2 = 2;
                specParts.push_back(confBuf[i]); 
                continue; 
            }
            if(fabs(confBuf[i].pid) > 7) //lepton
            {
                leps.push_back(confBuf[i]);
                lepI.push_back(i);
                Nlep++;
            }
            else //quark
            {
                if(fabs(confBuf[i].pid) > 4) { qIg3.push_back(i); g3q.push_back(confBuf[i]); }
                else if(fabs(confBuf[i].pid) > 2) { qIg2.push_back(i); g2q.push_back(confBuf[i]); }
                else { qIg1.push_back(i); g1q.push_back(confBuf[i]); }
            }
        }
        int interCol = 0;
        int icolS = 0;
        if(specParts.size() == 1)
        {
            if(specParts[0].color == 501) 
            {
                interCol = 502;
                if(iq2 > 0) icolS = 1;
                else icolS = -1;
            }
            if(specParts[0].color == 502) 
            {
                interCol = 501;
                if(iq1 > 0) icolS = 1;
                else icolS = -1;
            }
        }

        vector<particle> fileParts;
        fileParts.push_back(inParts[0]);
        fileParts.push_back(inParts[1]);

        //Push third Gen quarks and fake susy mediator onto output stack
        TLorentzVector interP(0.0,0.0,0.0,0.0);
        int interQ3 = 0;
        for(int ii = 0; ii < g3q.size(); ii++)
        {
            if(g3q[ii].color == 501 || g3q[ii].color == 502) continue;
            interP = interP + g3q[ii].p4v;
            interQ3 += g3q[ii].q3;
        }
        sumInterQ3_h->Fill(interQ3);
        daughters.push_back(interP);
        particle interBuf;
        interBuf.mass = interP.M();
        interBuf.color = 0;
        if(g3q.size() == 2) 
        {
            interBuf.color = interCol;
            interBuf.ic = icolS;
        }
        interBuf.p4v = interP;
        interBuf.q3 = interQ3;
        interBuf.m1 = 1;
        interBuf.m2 = 2;
        if(interQ3 == 0) interBuf.pid = 1000022; 
        if(fabs(interQ3) == 3) interBuf.pid = interQ3*1006213/fabs(interQ3); 
        if(fabs(interQ3) == 6) interBuf.pid = interQ3*1006223/fabs(interQ3); 
        if(g3q.size() > 1) fileParts.push_back(interBuf);
        int interI = fileParts.size();
        for(int ii = 0; ii < g3q.size(); ii++)
        {
            g3q[ii].m1 = interI;
            g3q[ii].m2 = interI;
            if(g3q.size() == 1) 
            {
                g3q[ii].m1 = 1;
                g3q[ii].m2 = 2;
                g3q[ii].ic = icolS;
            }
            fileParts.push_back(g3q[ii]);
            daughters.push_back(g3q[ii].p4v);
        }

        //Push second Gen quarks onto output stack
        interP.SetPxPyPzE(0.0,0.0,0.0,0.0);
        interQ3 = 0;
        for(int ii = 0; ii < g2q.size(); ii++)
        {
            if(g2q[ii].color == 501 || g2q[ii].color == 502) continue;
            interP = interP + g2q[ii].p4v;
            interQ3 += g2q[ii].q3;
        }
        sumInterQ3_h->Fill(interQ3);
        daughters.push_back(interP);
        interBuf.mass = interP.M();
        interBuf.color = 0;
        if(g2q.size() == 2) 
        {
            interBuf.color = interCol;
            interBuf.ic = icolS;
        }
        interBuf.p4v = interP;
        interBuf.q3 = interQ3;
        if(interQ3 == 0) interBuf.pid = 1000022; 
        if(fabs(interQ3) == 3) interBuf.pid = interQ3*1006213/fabs(interQ3);
        if(fabs(interQ3) == 6) interBuf.pid = interQ3*1006223/fabs(interQ3);
        if(g2q.size() > 1) fileParts.push_back(interBuf);
        interI = fileParts.size();
        for(int ii = 0; ii < g2q.size(); ii++)
        {
            g2q[ii].m1 = interI;
            g2q[ii].m2 = interI;
            if(g2q.size() == 1) 
            {
                g2q[ii].m1 = 1;
                g2q[ii].m2 = 2;
                g2q[ii].ic = icolS;
            }
            fileParts.push_back(g2q[ii]);
            daughters.push_back(g2q[ii].p4v);
        }

        //Push first Gen quarks onto output stack
        interP.SetPxPyPzE(0.0,0.0,0.0,0.0);
        interQ3 = 0;
        for(int ii = 0; ii < g1q.size(); ii++)
        {
            if(g1q[ii].color == 501 || g1q[ii].color == 502) continue;
            interP = interP + g1q[ii].p4v;
            interQ3 += g1q[ii].q3;
        }
        sumInterQ3_h->Fill(interQ3);
        daughters.push_back(interP);
        interBuf.mass = interP.M();
        interBuf.color = 0;
        if(g1q.size() == 2) 
        {
            interBuf.color = interCol;
            interBuf.ic = icolS;
        }
        interBuf.p4v = interP;
        interBuf.q3 = interQ3;
        if(interQ3 == 0) interBuf.pid = 1000022; 
        if(fabs(interQ3) == 3) interBuf.pid = interQ3*1006213/fabs(interQ3);
        if(fabs(interQ3) == 6) interBuf.pid = interQ3*1006223/fabs(interQ3);
        if(g1q.size() > 1) fileParts.push_back(interBuf);
        interI = fileParts.size();
        for(int ii = 0; ii < g1q.size(); ii++)
        {
            g1q[ii].m1 = interI;
            g1q[ii].m2 = interI;
            if(g1q.size() == 1) 
            {
                g1q[ii].m1 = 1;
                g1q[ii].m2 = 2;
            }
            fileParts.push_back(g1q[ii]);
            daughters.push_back(g1q[ii].p4v);
        }

        //Push leptons onto output stack
        for(int i = 0; i < leps.size(); i++)
        {
            leps[i].m1 = 1;
            leps[i].m2 = 2;
            fileParts.push_back(leps[i]);
            daughters.push_back(leps[i].p4v);
        }

        //Push spectator Quarks if there are any
        for(int i = 0; i < specParts.size(); i++) fileParts.push_back(specParts[i]);

        if(weight > maxwt) 
        {
            maxwt = weight;
            cout << "MaxWt: " << maxwt << endl;
        }


        lheF.writeEvent(fileParts);
        myT->Fill();
        NF++;
        if(NF%(Nevt/10) == 0) cout << "Produced Event " << NF << "  pdfN : " << pdfN << endl;
        //cout << "Produced Event " << NF << "  pdfN : " << pdfN << endl;
    }

    cout << "Max Weight: " << maxwt << endl;

    lheF.close();
    myF->cd();
    x1_h->Write();
    mcTot_h->Write();
    for(int i = 0; i < pt_hv.size(); i++)
    {
        pt_hv[i].Write();
    }
    myT->Write();
    myF->Close();


    return 0;
};



















