#include "asmmodel.h"
#include <cstdio>

namespace StatModel {

void ASMModel::findParamForShapeBTSM(const ShapeVec &Y, const ShapeVec &Y_old,
                               ASMFitResult & fitResult, ASMFitResult &b_old, int l)
{
    const double c[3] = {0.0005, 0.0005, 0.0005};

    double rho2, delta2, x2;
    double p;
    ShapeVec y_r, y_rpr, xFromParam, x;

    ShapeVec yt = Y_old;
    yt -= Y;
    rho2 = c[l] * yt.dot(yt);

    SimilarityTrans curTrans = b_old.transformation;
    Mat_< double > curParam, tmpFullParam, lastParam;

    curParam.create(pcaPyr[l].eigenvalues.rows, 1);
    for (int i=0; i<pcaPyr[l].eigenvalues.rows; i++)
        if (i<b_old.params.rows)
            curParam(i, 0) = b_old.params(i, 0);
        else
            curParam(i, 0) = 0;

    int ii=0;
    do{
        double s = curTrans.getS();
        lastParam = curParam.clone();

        // Expectation Step
        curTrans.invTransform(Y, y_r);
        p = sigma2Pyr[l]/(sigma2Pyr[l] + rho2/(s * s));
        delta2 = 1/(1/sigma2Pyr[l] + s*s / rho2);

        this->pcaPyr[l].backProject(curParam, xFromParam);

        pcaFullShape->project(y_r, tmpFullParam);
        pcaFullShape->backProject(tmpFullParam, y_rpr);
        x = p*y_rpr + (1-p) * xFromParam;
        x2 = x.dot(x) + (x.rows - 4) * delta2;

        // Maximization Step
        pcaPyr[l].project(x, curParam);

        for (int i=0; i<pcaPyr[l].eigenvalues.rows; i++)
            curParam(i, 0) *= (pcaShape->eigenvalues.at<double>(i, 0))/
                            (pcaShape->eigenvalues.at<double>(i, 0)+sigma2Pyr[l]);
        int nP = x.rows / 2;
        curTrans.a = Y.dot(x) / x2;
        curTrans.b = 0;
        for (int i=0; i<nP; i++)
            curTrans.b += x.X(i) * Y.Y(i) - x.Y(i)*Y.X(i);
        curTrans.b /= x2;
        curTrans.Xt = Y.getXMean();
        curTrans.Yt = Y.getYMean();

        ii++;
    } while (norm(lastParam-curParam)>1e-4 && ii<20);
    fitResult.params = curParam;
    fitResult.transformation = curTrans;
}

vector< ASMFitResult > ASMModel::fitAll(
        const Mat & img,
        const vector< cv::Rect > & detectedObjs,
        int verbose) {
    vector< ASMFitResult > fitResultV;
    for (uint i=0; i<detectedObjs.size(); i++){
        cv::Rect r = detectedObjs[i];
        r.y -= r.height*searchYOffset;
        r.x -= r.width*searchXOffset;
        if (r.x<0) r.x = 0;
        if (r.y<0) r.y = 0;
        r.width *= searchWScale;
        r.height *= searchHScale;
        if (r.x+r.width>img.cols) r.width = img.cols-r.x;
        if (r.y+r.height>img.rows) r.height = img.rows-r.y;

        // Fit it!
        ASMFitResult fitResult = fit(img(r), verbose);
        SimilarityTrans s2;
        s2.Xt = r.x;
        s2.Yt = r.y;
        s2.a = 1;
        fitResult.transformation = s2 * fitResult.transformation;
        fitResultV.push_back(fitResult);
    }
    return fitResultV;
}

void ASMModel::showResult(Mat& img, const vector< ASMFitResult >& res)
{
    Mat mb;
    if (img.channels()==1)
        cv::cvtColor(img, mb, CV_GRAY2RGB);
    else
        mb = img.clone();

    ShapeVec sv;
    for (uint i=0; i<res.size(); i++){
        vector< Point_<int> > V;
        res[i].toPointList(V);
        shapeInfo.drawMarkPointsOnImg(mb, V, true);
    }

    if (!img.empty())
        imshow("hoho", mb);
}

ASMFitResult ASMModel::fit(const cv::Mat& img, int verbose)
{
    ASMFitResult fitResult(this);
    // Step 2: Ensure it is a grayscale image
    Mat grayImg;
    if (img.channels() == 3){
        cv::cvtColor(img, grayImg, CV_BGR2GRAY);
    }
    else
        grayImg = img;

    // Step 3: Resize each face image
    Mat resizedImg;
    // Resize the image to proper size
    double ratio;
    ratio = sqrt( double(40000) / (grayImg.rows * grayImg.cols));
    cv::resize(grayImg, resizedImg, Size(grayImg.cols*ratio, grayImg.rows*ratio));

    ModelImage curSearch;
    curSearch.setShapeInfo( &shapeInfo );
    curSearch.loadTrainImage(resizedImg);

    fitResult.params = Mat_<double>::zeros(nShapeParams, 1);

    ShapeVec &sv = curSearch.shapeVec;
    ShapeVec shape_old;

    projectParamToShape(fitResult.params, sv);
    SimilarityTrans st = sv.getShapeTransformFitingSize(resizedImg.size(),
                                                      searchScaleRatio,
                                                      searchInitXOffset,
                                                      searchInitYOffset);
    fitResult.transformation = st;
    curSearch.buildFromShapeVec(st);

    pyramidLevel = 2;
    int k=localFeatureRad;

    ns=4;

    // sum of offsets of current iteration.
    int totalOffset;
    if (verbose >= ASM_FIT_VERBOSE_AT_LEVEL)
        curSearch.show();
    // Update each point
    vector< Point_< int > > V;
    for (int l=this->pyramidLevel; l>=0; l--){
        if (verbose >= ASM_FIT_VERBOSE_AT_LEVEL)
            printf("Level %d\n", l);
        Mat_<double> img=curSearch.getDerivImage(l);

        // at most 5 iterations for each level
        int runT;
        double avgMov;
        for (runT=0; runT<10; runT++){
            // Backup current shape
            shape_old.fromPointList(curSearch.points);

            totalOffset = 0;
            vector< Point_< int > > bestEP(nMarkPoints);
            for (int i=0; i<this->nMarkPoints; i++){
                if (verbose >= ASM_FIT_VERBOSE_AT_POINT)
                    printf("Dealing point %d...\n", i);

                Mat_< double > nrmV(2*k+1, 1);
                double curBest=-1, ct;
                int bestI = 0;
                double absSum;
                for (int e=ns; e>=-ns; e--){
                    curSearch.getPointsOnNorm(i, k, l, V, 2*searchStepAreaRatio, e);

                    absSum = 0;
                    for (int j=-k;j<=k;j++){
                        nrmV(j+k, 0) = img(V[j+k]);
                        absSum += fabs(nrmV(j+k, 0));
                    }
                    nrmV *= 1/absSum;
                    ct = cv::Mahalanobis(nrmV, this->meanG[l][i], this->iCovarG[l][i]);

                    if (verbose >= ASM_FIT_VERBOSE_AT_POINT)
                        curSearch.show(l, i, true, e);

                    if (ct<curBest || curBest<0){
                        curBest = ct;
                        bestI = e;
                        bestEP[i] = V[k];
                    }
                }

                totalOffset += abs(bestI);

                if (verbose >= ASM_FIT_VERBOSE_AT_POINT)
                    curSearch.show(l, i, true, bestI);
            }
            for (int i=0;i<nMarkPoints;i++){
                curSearch.points[i] = bestEP[i];
                curSearch.points[i].x <<= l;
                if (l>0) curSearch.points[i].x += (1<<(l-1));
                curSearch.points[i].y <<= l;
                if (l>0) curSearch.points[i].y += (1<<(l-1));
            }
            curSearch.shapeVec.fromPointList(curSearch.points);

            if (verbose >= ASM_FIT_VERBOSE_AT_ITERATION)
                curSearch.show(l);

            // Project to PCA model and then back
            //findParamForShape(curSearch.shapeVec,  fitResult);
            findParamForShapeBTSM(curSearch.shapeVec, shape_old, fitResult, fitResult, l);

            pcaPyr[l].backProject(fitResult.params, sv);

            // Reconstruct new shape
            curSearch.buildFromShapeVec(fitResult.transformation);

            avgMov = (double)totalOffset/nMarkPoints;
            if (verbose >= ASM_FIT_VERBOSE_AT_ITERATION){
                printf("Iter %d:  Average offset: %.3f\n", runT+1, avgMov);
                curSearch.show(l);
            }

            if (avgMov < 1.3){
                runT++;
                break;
            }
        }
        if (verbose == ASM_FIT_VERBOSE_AT_LEVEL){
            printf("%d iterations. average offset for last iter: %.3f\n", runT, avgMov);
            curSearch.show(l);
        }
    }

    SimilarityTrans s2;
    s2.a = 1/ratio;
    fitResult.transformation = s2 * fitResult.transformation;
    return fitResult;
}

void ASMModel::loadFromFile(ModelFile& file)
{
    ShapeModel::loadFromFile(file);
    printf("Loading ASM model from file...\n");

    file.readInt(localFeatureRad);
    file.readInt(ns);

    int i,j;
    int rows, cols;
    file.readInt(rows);
    file.readInt(cols);
    iCovarG.resize(pyramidLevel+1);
    for (i=0;i<=pyramidLevel;i++){
        iCovarG[i].resize(nMarkPoints);
        for (j=0;j<nMarkPoints;j++){
            iCovarG[i][j].create(rows, cols);
            for (int ii=0;ii<rows;ii++)
                for (int jj=0;jj<cols;jj++)
                    file.readReal(iCovarG[i][j](ii, jj));
        }
    }

    file.readInt(rows);
    file.readInt(cols);
    meanG.resize(pyramidLevel+1);
    for (i=0;i<=pyramidLevel;i++){
        meanG[i].resize(nMarkPoints);
        for (j=0;j<nMarkPoints;j++){
            meanG[i][j].create(rows, cols);
            for (int ii=0;ii<rows;ii++)
                for (int jj=0;jj<cols;jj++)
                    file.readReal(meanG[i][j](ii, jj));
        }
    }

    // Prepare BTSM pyramid
    double curSigma2 = 0;
    for (i=0; i<pcaFullShape->eigenvalues.rows; i++){
        curSigma2 += pcaFullShape->eigenvalues.at<double>(i, 0);
    }

    // Layer 2, 5 parameter
    for (i=0; i<5 && i<pcaFullShape->eigenvalues.rows; i++){
        curSigma2 -= pcaFullShape->eigenvalues.at<double>(i, 0);
    }
    sigma2Pyr[2] = curSigma2 / (nMarkPoints*2-4);
    pcaPyr[2].eigenvalues = pcaFullShape->eigenvalues.rowRange(0, i);
    pcaPyr[2].eigenvectors = pcaFullShape->eigenvectors.rowRange(0, i);
    pcaPyr[2].mean = pcaFullShape->mean;

    // Layer 1, 20 parameter
    for (; i<20 && i<pcaFullShape->eigenvalues.rows; i++){
        curSigma2 -= pcaFullShape->eigenvalues.at<double>(i, 0);
    }
    sigma2Pyr[1] = curSigma2 / (nMarkPoints*2-4);
    pcaPyr[1].eigenvalues = pcaFullShape->eigenvalues.rowRange(0, i);
    pcaPyr[1].eigenvectors = pcaFullShape->eigenvectors.rowRange(0, i);
    pcaPyr[1].mean = pcaFullShape->mean;

    sigma2Pyr[0] = sigma2;
    pcaPyr[0] = *pcaShape;
}

void ASMModel::loadFromFile(const string& filename)
{
    ModelFileAscii mf;
    mf.openFile(filename.c_str(), "rb");
    loadFromFile(mf);
    mf.closeFile();
}


void ASMFitResult::toPointList(vector< Point_<int> > &pV) const {
    ShapeVec sv;
    asmModel->projectParamToShape(params, sv);
    sv.restoreToPointList(pV, transformation);
}

} // Namespace
