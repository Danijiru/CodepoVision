#ifndef SHAPEMODEL_H
#define SHAPEMODEL_H

#include "modelimage.h"
#include "opencv/cv.h"
#include "modelfile.h"
#include <vector>
using std::vector;
using cv::PCA;

namespace StatModel {

//! Base class for ASM/AAM fitting result
struct FitResult{
    //! Parameters for the model
    Mat_< double > params;

    //! The similarity transformation needed to recover shape
    SimilarityTrans transformation;
};

//! The Statistical Shape model for training sets.
/**
 * After all the training shapes are aligned, a PCA model is built so that a
 * shape can be roughly described by a few parameters (PCA coefficients).
 *
 * This class is responsible for building the statistical shape model and
 * provides some helper functions.
 */
class ShapeModel
{
public:
    ShapeModel();

    //! Load the model from a file
    virtual void loadFromFile(ModelFile &file);

    //! Set the level for the image pyramid
    /*!
        \param l Image from level 0 to l will be considered during training
                    and searching.
    */
    void setPyramidLevel(int l){ pyramidLevel = l; }

    //! Project a parameter vector to a shape
    /*!
        \param paramVec parameter vector.
        \param shapeVec the shape corresponding to the parameter vector
    */
    void projectParamToShape(const Mat_<double> & paramVec, ShapeVec &shapeVec);

    //! Project a shape to a parameter vector
    /*!
        \param shapeVec the shape corresponding to the parameter vector
        \param paramVec parameter vector.
    */
    void projectShapeToParam(const ShapeVec & shapeVec, Mat_<double> &paramVec);

    ShapeInfo & getShapeInfo(){ return shapeInfo;}

    //! Normalize an parameter vector(0..1)
    Mat_< double > normalizeParam(const Mat_<double> &p){
        Mat_<double> n = p.clone();
        for (int i=0; i<p.rows; i++)
            n(i, 0)/= 3*sqrt(pcaShape->eigenvalues.at<double>(i, 0));
        return n;
    }

    //! Reconstruct parameter vector from normalized vector
    Mat_< double > reConFromNorm(const Mat_<double> &p){
        Mat_<double> n = p.clone();
        for (int i=0; i<p.rows; i++)
            n(i, 0)*= 3*sqrt(pcaShape->eigenvalues.at<double>(i, 0));
        return n;
    }


    // For viewing the model
    //! Used for viewing model
    struct ModelViewInfo
    {
        vector< int > vList;
        int curParam;
        void *pModel;
    };

protected:
    //! level for the image pyramid.
    int pyramidLevel;

    //! Number of landmark points in a image.
    int nMarkPoints;

    //! Number of training images.
    int nTrain;

    //! All the images, with labelled markpoints.
    vector<ModelImage> imageSet;

    //! Path info for shapes;
    ShapeInfo shapeInfo;

    //! Mean shape after aligning
    ShapeVec meanShape;

    //! PCA model for shapes.
    PCA *pcaShape;
    //! Number of eigen vectors reserved for shape model.
    int nShapeParams;

    //! Data for BTSM: \f$\sigma^2\f$
    double sigma2;

    //! Data for BTSM: Full \f$\phi\f$
    PCA *pcaFullShape;

protected:
    //! r.y -= r.height*?
    double searchYOffset;

    //! r.x -= r.width*?
    double searchXOffset;

    //! r.width *= ?
    double searchWScale;
    //! r.height *= ?
    double searchHScale;

    //! step: ?*100/sqrt(area)
    double searchStepAreaRatio;

    //! init scale ratio when searching
    double searchScaleRatio;
    //! init X offset when searching
    double searchInitXOffset;
    //! init Y offset when searching
    double searchInitYOffset;
};
} // Namespace
#endif // SHAPEMODEL_H
