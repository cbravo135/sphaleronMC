#define mcTree_cxx
#include "mcTree.h"
#include <TH2.h>
#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>

#include <iostream>

using namespace std;

void mcTree::Loop()
{
//   In a ROOT session, you can do:
//      root> .L mcTree.C
//      root> mcTree t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   TFile *myF = new TFile("dalitz.root","RECREATE");

   TH2D *dalitz = new TH2D("dalitz","dalitz",910,0.0,9100.0,910,0.0,9100.0);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      double m12 = 0.0;
      double m13 = 0.0;

      TLorentzVector p1 = daughters->at(0);
      TLorentzVector p2 = daughters->at(1);
      TLorentzVector p3 = daughters->at(2);

      TLorentzVector p12 = p1 + p2;
      TLorentzVector p13 = p1 + p3;

      m12 = p12.M();
      m13 = p13.M();

      if(jentry%1000 == 0) cout << "m12: " << m12 << "   m13: " << m13 << endl;

      dalitz->Fill(m12,m13);

   }

   dalitz->Write();
   myF->Write();
   myF->Close();
}
