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

#include "../include/configBuilder.h"
#include "../include/genTree.h"
#include "../include/LHEWriter.h"
#include "../include/mstwpdf.h"
//#include "../include/protonPDF.h"

#define ARGS 4
#define SQRTS 13000.0
#define MCW 3.6e-5

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
    vector<double> m2;
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

    //TF1 pdfu("pdfu","5.1072*(x^(0.8))*(1-x)^3/x",thr*thr/13000.0*13000.0,1.0);
    TF1 pdfu("pdfu","(x^(-1.16))*(1-x)^(1.76)/(2.19*x)",minx,1.0);

    c_mstwpdf *pdf = new c_mstwpdf("/afs/cern.ch/user/b/bravo/work/sphaleron/mc/toy/mstw2008/Grids/mstw2008nnlo.00.dat");
    double maxMCtot = 0.0;

    TH1D *x1_h = new TH1D("x1_h","x1 inclusive",1000,0.0,1.0);
    TH1D *mcTot_h = new TH1D("mcTot_h","Monte Carlo Probabilities",100,0.0,MCW);
    TH1D *sumInterQ3_h = new TH1D("sumInterQ3_h","Intermidiate particle charges",21,-10.5,10.5);
    //protonPDF proton("../mstw2008/Grids/mstw2008nnlo.00.dat",SQRTS,minx);

    TTree *myT = new TTree("mcTree","mcTree");
    myT->Branch("daughters",&daughters); 
    myT->Branch("weight",&weight); 
    myT->Branch("m2",&m2); 
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
        m2.clear();

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
                    //cout << "MC Total: " << mcTot << endl;
                    //cout << "iq1: " << iq1 << endl;
                    //cout << "iq2: " << iq2 << endl;
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
        vector<int> decayPID;
        vector<int> decayColz;

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

        decayPID.push_back(iq1);
        decayColz.push_back(501);
        decayPID.push_back(iq2);
        decayColz.push_back(502);
        int colNow = 503;
        int colNowNew = 503;

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
        vector<TLorentzVector> decayBuf;
        for(int ii = 0; ii < confBuf.size(); ii++)
        {
            TLorentzVector prod = *gen.GetDecay(ii);
            confBuf[ii].p4v = prod;
            if(confBuf[ii].color != 501 && confBuf[ii].color != 502 && fabs(confBuf[ii].pid) < 7) confBuf[ii].color = colNowNew++;
            if(fabs(confBuf[ii].pid) > 7) confBuf[ii].color = 0;
            decayBuf.push_back(prod);
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
            if(confBuf[i].color == 501 || confBuf[i].color == 502) { specParts.push_back(confBuf[i]); continue; }
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
        decayColz.push_back(0);
        particle interBuf;
        interBuf.mass = interP.M();
        interBuf.color = 0;
        interBuf.p4v = interP;
        interBuf.q3 = interQ3;
        if(interQ3 == 0) { decayPID.push_back(1000022); interBuf.pid = 1000022; }
        if(fabs(interQ3) == 3) { decayPID.push_back(interQ3*1006213/fabs(interQ3)); interBuf.pid = interQ3*1006213/fabs(interQ3); }
        if(fabs(interQ3) == 6) { decayPID.push_back(interQ3*1006223/fabs(interQ3)); interBuf.pid = interQ3*1006223/fabs(interQ3); }
        fileParts.push_back(interBuf);
        for(int ii = 0; ii < g3q.size(); ii++)
        {
            fileParts.push_back(g3q[ii]);
            daughters.push_back(decayBuf[qIg3[ii]]);
            decayColz.push_back(colNow++);
            decayPID.push_back(confBuf[qIg3[ii]].pid);
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
        decayColz.push_back(0);
        interBuf.mass = interP.M();
        interBuf.color = 0;
        interBuf.p4v = interP;
        interBuf.q3 = interQ3;
        if(interQ3 == 0) { decayPID.push_back(1000022); interBuf.pid = 1000022; }
        if(fabs(interQ3) == 3) { decayPID.push_back(interQ3*1006213/fabs(interQ3)); interBuf.pid = interQ3*1006213/fabs(interQ3); }
        if(fabs(interQ3) == 6) { decayPID.push_back(interQ3*1006223/fabs(interQ3)); interBuf.pid = interQ3*1006223/fabs(interQ3); }
        fileParts.push_back(interBuf);
        for(int ii = 0; ii < g2q.size(); ii++)
        {
            fileParts.push_back(g2q[ii]);
            daughters.push_back(decayBuf[qIg2[ii]]);
            decayColz.push_back(colNow++);
            decayPID.push_back(confBuf[qIg2[ii]].pid);
        }

        //Push first Gen quarks onto output stack
        for(int i = 0; i < g1q.size(); i++)
        {
            if(g1q[i].color == 501 || g3q[i].color == 502) continue;
            fileParts.push_back(g1q[i]);
            daughters.push_back(decayBuf[qIg1[i]]);
            if(confBuf.size() == 12 && qIg1.size() - (i+1) == 0) decayColz.push_back(501);
            else if(confBuf.size() == 14 && qIg1.size() - (i+1) < 2) decayColz.push_back(501 + qIg1.size() - (i+1));
            else decayColz.push_back(colNow++);
            decayPID.push_back(confBuf[qIg1[i]].pid);
        }

        //Push leptons onto output stack
        for(int i = 0; i < leps.size(); i++)
        {
            fileParts.push_back(leps[i]);
            daughters.push_back(decayBuf[lepI[i]]);
            decayColz.push_back(0);
            decayPID.push_back(confBuf[lepI[i]].pid);
        }

        //Push spectator Quarks if there are any
        for(int i = 0; i < specParts.size(); i++) fileParts.push_back(specParts[i]);

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


        lheF.writeEvent(fileParts);
        myT->Fill();
        NF++;
        if(NF%(Nevt/10) == 0) cout << "Produced Event " << NF << "  pdfN : " << pdfN << endl;
    }

    cout << "Max Weight: " << maxwt << endl;

    lheF.close();
    myF->cd();
    x1_h->Write();
    mcTot_h->Write();
    pdfu.Write();
    myT->Write();
    myF->Close();


    return 0;
};














