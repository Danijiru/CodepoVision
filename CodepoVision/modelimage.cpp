#include "modelimage.h"
//#include "afreader.h"
using std::cerr;

namespace StatModel {

ModelImage::ModelImage()
{
    imgLoaded = false;
    this->shapeInfo = NULL;
}

void ModelImage::buildFromShapeVec(SimilarityTrans& trans)
{
    nMarkPoints = shapeVec.nPoints();
    shapeVec.restoreToPointList(points, trans);
}

bool ModelImage::loadTrainImage(const Mat &img)
{
    imgdata = img;
    cv::buildPyramid(imgdata, this->imgPyramid, 3);
    if (imgdata.channels() == 3){
        this->imgPyrGray.resize(4);
        for (int i=0; i<=3; i++)
            cv::cvtColor(imgPyramid[i], imgPyrGray[i], CV_BGR2GRAY);
    }
    else {
        this->imgPyrGray.resize(4);
        for (int i=0; i<=3; i++)
            imgPyrGray[i] = imgPyramid[i];
    }
    imgPyrGrayDeriv.resize(4);
    for (int i=0; i<=3; i++){
        imgPyrGray[i].convertTo(imgPyrGrayDeriv[i], CV_64F);
        cv::Sobel(imgPyrGrayDeriv[i], imgPyrGrayDeriv[i], CV_64F, 1, 1);
    }
    this->imgLoaded = true;
    return imgLoaded;
}

void ModelImage::getPointsOnNorm(int pId, int k, int level,
                            vector< Point_< int > > &V, double step,
                            int pOffset)
{
    // Find the norm direction
    Point_< double > va, vb, vDirection;
    va = points[shapeInfo->pointInfo[pId].connectFrom] - points[pId];
    vb = points[shapeInfo->pointInfo[pId].connectTo] - points[pId];
    double td;
    td = norm(va);
    if (td>1e-10) va *= 1 / td;
    td = norm(vb);
    if (td>1e-10) vb *= 1 / td;
    vDirection.x = - va.y + vb.y;
    vDirection.y = va.x - vb.x;
    if (norm(vDirection)<1e-10){
        if (norm(va)>1e-10)
            vDirection = - va;
        else
            vDirection.x = 1, vDirection.y = 0;
    }
    else
        vDirection *= 1 / norm(vDirection);

    int i, j;
    int nx, ny;
    int offsetX, offsetY;
    int prevX, prevY;

    // Find the center point, here step===1
    prevX = 0;
    prevY = 0;
    nx = ny = 0;
    j = 1;
    for (i=1; i<=abs(pOffset);i++){
        do {
            nx = cvRound(j*vDirection.x);
            ny = cvRound(j*vDirection.y);
            j++;
        } while (nx == prevX && ny == prevY);

        prevX = nx;
        prevY = ny;
    }
    j--;
//     printf("%d, %d\n", nx, ny);
    if (pOffset>0)
        offsetX = nx, offsetY = ny;
    else
        offsetX = -nx, offsetY = -ny;
//     offsetX = offsetY = 0;

    // Apply the "step", and find points
    vDirection *= step;
    prevX = 0;
    prevY = 0;
    nx = ny = 0;

    // Test best j
    j = 1;
    for (i=1; i<=k;i++){
        do {
            nx = cvRound(j*vDirection.x);
            ny = cvRound(j*vDirection.y);
            j++;
        } while (nx == prevX && ny == prevY);

        prevX = nx;
        prevY = ny;
    }
    j--;

    V.resize(2*k+1);
    int rX, rY;
    for (i=k;i>=-k;i--){
        rX = (points[pId].x>>level) + nx + offsetX;
        rY = (points[pId].y>>level) + ny + offsetY;
        if (rX<0) rX = 0;
        if (rY<0) rY = 0;
        if (rX >= (imgdata.cols>>level)) rX = (imgdata.cols>>level) - 1;
        if (rY >= (imgdata.rows>>level)) rY = (imgdata.rows>>level) - 1;
        V[i+k] = Point_< int >(rX, rY);
        //v(i+(k+1), 0) = this->imgdata.at< char >(nx, ny);
        do {
            nx = cvRound(j*vDirection.x);
            ny = cvRound(j*vDirection.y);
            j--;
        } while (nx == prevX && ny == prevY);
        prevX = nx;
        prevY = ny;
    }
}

Mat ModelImage::show(int l, int pId, bool showInWin, int highLight)
{
    Mat mb;
    if (imgPyramid[0].channels()==1)
        cv::cvtColor(imgPyramid[0], mb, CV_GRAY2RGB);
    else
        mb = imgPyramid[0].clone();

    shapeInfo->drawMarkPointsOnImg(mb, points, true);

    for (int i=0;i<nMarkPoints;i++){
        if (pId==-1 || pId!=i)
            continue;
        vector< Point_< int > > pV;
        getPointsOnNorm(i, 4, l, pV, 2, highLight);
        for (int j=0; j<9; j++)
            if (highLight==100/*j-3*/){
                cv::circle(mb, Point_< int >(pV[j].x<<l, pV[j].y<<l), 3, CV_RGB(230, 100, 50),
                       1, CV_AA);
            }
            else
                cv::circle(mb, Point_< int >(pV[j].x<<l, pV[j].y<<l), 3, CV_RGB(50, 230, 100),
                       1, CV_AA);
    }
    if (showInWin){
        cvNamedWindow("hoho", CV_WINDOW_AUTOSIZE);
        cv::imshow("hoho", mb);

        printf("Press any key to continue...\n");
        cv::waitKey();
    }
    return mb;
}
} // Namespace
