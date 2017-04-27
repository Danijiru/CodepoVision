#include "similaritytrans.h"
#include "shapevec.h"

namespace StatModel {
void SimilarityTrans::invTransform(const ShapeVec &src, ShapeVec &dst) const{
    int nP = src.nPoints();
    double x11, x12, x21, x22;
    x11 = a/(a*a+b*b);
    x12 = b/(a*a+b*b);
    x21 = -b/(a*a+b*b);
    x22 = a/(a*a+b*b);

    dst.create(nP<<1, 1);
    double xt, yt;
    for (int i=0; i < nP; i++) {
        xt = src.X(i) - Xt;
        yt = src.Y(i) - Yt;
        dst.X(i) = x11 * xt + x12 * yt;
        dst.Y(i) = x21 * xt + x22 * yt;
    }
}

void SimilarityTrans::transform(const ShapeVec &src, ShapeVec &dst) const{
    int nP = src.nPoints();
    dst.create(nP<<1, 1);
    double xt, yt;
    for (int i=0; i < nP; i++) {
        xt = src.X(i);
        yt = src.Y(i);
        dst.X(i) = a * xt - b * yt + Xt;
        dst.Y(i) = b * xt + a * yt + Yt;
    }
}

} // Namespace
