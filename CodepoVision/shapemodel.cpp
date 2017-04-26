#include <cstdio>
#include <opencv/highgui.h>
#include "shapemodel.h"
#include "asmmodel.h"

namespace StatModel {

ShapeModel::ShapeModel()
{
    /// \todo Make the "3" here a constant or a configurable variable.
    pyramidLevel = 3;
}

void ShapeModel::loadFromFile(ModelFile &file)
{
    printf("Loading Shape model from file...\n");
    file.readInt(pyramidLevel);
    file.readInt(nMarkPoints);
    file.readInt(nTrain);
    file.readInt(nShapeParams);

    file.readReal(searchYOffset);
    file.readReal(searchXOffset);
    file.readReal(searchWScale);
    file.readReal(searchHScale);
    file.readReal(searchStepAreaRatio);

    file.readReal(searchScaleRatio);
    file.readReal(searchInitXOffset);
    file.readReal(searchInitYOffset);

    // PCA shape model
    file.readPCA(pcaShape);

    meanShape.create(nMarkPoints*2, 1);
    for (int i=0;i<nMarkPoints*2;i++)
        file.readReal(meanShape(i, 0));

    // Info for BTSM
    file.readReal(sigma2);
    file.readPCA(pcaFullShape);

    shapeInfo.readFromFile(file);
}

void ShapeModel::projectParamToShape(const Mat_<double> & paramVec,
                                     ShapeVec & shapeVec)
{
    this->pcaShape->backProject(paramVec, shapeVec);
}

using cv::namedWindow;
using cv::createTrackbar;
using cv::setTrackbarPos;

} // Namespace
