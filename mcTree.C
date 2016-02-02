#define mcTree_cxx
#include "mcTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>

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

   TH1D * eta_4550_h = new TH1D("eta_4550_h","eta_4550_h",455,0.0,4550.0);
   TH1D * eta_2275_h = new TH1D("eta_2275_h","eta_2275_h",455,0.0,4550.0);
   TH1D * eta_1137_h = new TH1D("eta_1137_h","eta_1137_h",455,0.0,4550.0);
   TH1D * eta_568_h = new TH1D("eta_568_h","eta_568_h",455,0.0,4550.0);
   TH1D * eta_284_h = new TH1D("eta_284_h","P_{T} of Different Legs",455,0.0,4550.0);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      for(int i = 0; i < 12; i++)
      {
          if(momMass->at(i) == 4550.0) eta_4550_h->Fill(daughters->at(i).Pt());
          if(momMass->at(i) == 2275.0) eta_2275_h->Fill(daughters->at(i).Pt());
          if(momMass->at(i) == 1137.5) eta_1137_h->Fill(daughters->at(i).Pt());
          if(momMass->at(i) == 568.75) eta_568_h->Fill(daughters->at(i).Pt());
          if(momMass->at(i) == 284.375) eta_284_h->Fill(daughters->at(i).Pt());
      }
   }

   eta_4550_h->Scale(1.0/eta_4550_h->Integral(0,455));
   eta_2275_h->Scale(1.0/eta_2275_h->Integral(0,455));
   eta_1137_h->Scale(1.0/eta_1137_h->Integral(0,455));
   eta_568_h->Scale(1.0/eta_568_h->Integral(0,455));
   eta_284_h->Scale(1.0/eta_284_h->Integral(0,455));

   eta_4550_h->SetLineColor(1);
   eta_2275_h->SetLineColor(2);
   eta_1137_h->SetLineColor(4);
   eta_568_h->SetLineColor(6); 
   eta_284_h->SetLineColor(8); 

   eta_4550_h->SetLineWidth(2);
   eta_2275_h->SetLineWidth(2);
   eta_1137_h->SetLineWidth(2);
   eta_568_h->SetLineWidth(2); 
   eta_284_h->SetLineWidth(2); 

   TLegend *leg = new TLegend(0.1,0.7,0.48,0.9);
   leg->SetHeader("Mother Mass");
   leg->AddEntry(eta_284_h,"284.375 GeV","l");
   leg->AddEntry(eta_568_h,"568.75 GeV","l");
   leg->AddEntry(eta_1137_h,"1137.5 GeV","l");
   leg->AddEntry(eta_2275_h,"2275 GeV","l");
   leg->AddEntry(eta_4550_h,"4550 GeV","l");

   TCanvas *canv = new TCanvas();
   canv->cd();

   eta_284_h->GetYaxis()->SetTitle("1 / 10.0 GeV");
   eta_284_h->GetXaxis()->SetTitle("P_{T} [GeV]");
   eta_284_h->GetYaxis()->SetTitleOffset(1.3);

   eta_284_h->Draw();
   eta_568_h->Draw("same");
   eta_4550_h->Draw("same");
   eta_2275_h->Draw("same");
   eta_1137_h->Draw("same");
   leg->Draw();

   canv->SaveAs("Pz_h.pdf");


}
