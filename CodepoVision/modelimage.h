#ifndef MODELIMAGE_H
#define MODELIMAGE_H
#include <string>
#include <vector>
#include "opencv/highgui.h"
#include "opencv/cv.h"
#include "shapevec.h"
#include "shapeinfo.h"
using std::string;
using std::vector;
using cv::Point_;
using cv::Mat_;
using cv::Mat;
using cv::imread;
using cv::Size;

namespace StatModel {

//! Image and image related operations.
class ModelImage
{
protected:
    /// The number of landmark points.
    int nMarkPoints;

    /// Optional 'host image' filename including full path.
    string hostImageName;

    /// the training image
    Mat imgdata;

    /// Image pyramid
    vector< Mat > imgPyramid;
    vector< Mat_<double> > imgPyrGrayDeriv;
    vector< Mat_<uchar> > imgPyrGray;

    /// Is the image loaded?
    bool imgLoaded;

    /// Information about shapes and paths.
    ShapeInfo *shapeInfo;

public:
    /// Landmark points
    vector< Point_< int > > points;

    /// Shape vectors
    ShapeVec shapeVec;

    /// Load training image
    bool loadTrainImage(const Mat &img);

    void setShapeInfo(ShapeInfo *si){ shapeInfo = si; }

    /// Get the coordinates of points at normal direction of a landmark point
    /*!
        \param pId id of the point
        \param k how many points to get on either direction
        \param level position in the pyramid level
        \param V the vector that save results.
        \param step VERY IMPORTANT, for a image with area of 10000, 1.0 may be a good choice
        \param pOffset when searching for best points, use the p'th  point along the profile as the center
    */
    void getPointsOnNorm(int pId, int k, int level,
                         vector< Point_< int > > &V,
                         double step, int pOffset=0);

    /// Return the derivative image at specified level;
    Mat & getDerivImage(int level){ return imgPyrGrayDeriv[level]; }

    /// Host image (if any).
    inline const string &HostImage() const { return hostImageName; }

    void buildFromShapeVec(SimilarityTrans &trans);

    /// Set the host image.
    void setHostImage( const char * hostImageFilename ) {
        hostImageName = hostImageFilename;
    }

    /// The number of shape points.
    inline int NPoints() const { return nMarkPoints; }

    //! Show the image interactively
    Mat show(int level = 0, int pId = -1, bool showInWin = true, int highLight = 0);

    ModelImage();
};

} // Namespace
#endif // MODELIMAGE_H
