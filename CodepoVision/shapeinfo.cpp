#include "shapeinfo.h"

namespace StatModel {

void ShapeInfo::readFromFile(ModelFile &f){
    f.readInt(nContours);
    int t;
    contourStartInd.resize(nContours+1);
    for (int i=0;i<nContours+1;i++)
        contourStartInd[i] = f.readInt(t);
    contourIsClosed.resize(nContours);
    for (int i=0;i<nContours;i++)
        contourIsClosed[i] = f.readInt(t);
    pointInfo.resize(f.readInt(t));
    for (size_t i=0;i<pointInfo.size();i++){
        f.readInt(pointInfo[i].connectFrom);
        f.readInt(pointInfo[i].connectTo);
        f.readInt(pointInfo[i].type);
        f.readInt(pointInfo[i].pathId);
    }
}

Mat ShapeInfo::drawMarkPointsOnImg(
  Mat& img, const vector< Point >& vP, bool drawInPlace) const
{
    Mat mb;
    if (drawInPlace)
        mb = img;
    else
        mb = img.clone();

    for (uint i=0;i<vP.size();i++){
        cv::circle(mb, vP[i], 3, cv::Scalar(25, 50, 255), 1, CV_AA);
    }
    for (int i=0;i<nContours;i++){
        for (int j=contourStartInd[i]; j<contourStartInd[i+1]; j++){
            cv::line(mb, vP[pointInfo[j].connectFrom],
                     vP[j],
                     cv::Scalar(200, 30, 80), 1, CV_AA);
        }
    }
    return mb;
}

} // Namespace
