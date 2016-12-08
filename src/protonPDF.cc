#include "../include/protonPDF.h"

protonPDF::protonPDF(string fName, double qq, double xxmin)
{
    q = qq;
    xmin = xxmin;
    grid = new c_mstwpdf(fName.c_str());
    pdf = new TF1("pdf",this,&protonPDF::pdfLU,xmin,1.0,0);
}

protonPDF::~protonPDF()
{
    delete grid;
    delete pdf;
}

double protonPDF::sample()
{
    double xx = pdf->GetRandom();
    return xx;
}

