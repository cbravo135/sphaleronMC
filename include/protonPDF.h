#include <string>

#include "TF1.h"

#include "mstwpdf.h"

using namespace std;

class protonPDF
{
    private:
        c_mstwpdf *grid;
        TF1 *pdf;
        double q;
        double xmin;
        double upv;


    public:
        protonPDF(string fName, double qq, double xxmin);
        ~protonPDF();

        double sample();
        double pdfLU(double *x, double *p) {
            double xx = x[0];
            upv = grid->parton(8,xx,q);
            return upv;
        }

};
