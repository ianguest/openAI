#ifndef PCA_H
#define PCA_H


#include "opencv2/opencv.hpp"
#include <math.h>

using namespace cv;
using namespace std;

typedef struct sPCAobj
{
    float angle;
    float majoreigenvalue;
    float minoreigenvalue;
    float ratio;
    float meanrow;
    float meancol;
    float objsize;
    float sinang;
    Rect box;
    int type;
} PCAOBJ;

class pca
{
    public:
        pca();
        virtual ~pca();
        void getOrientation(vector<Point> &pts);
        void draw(Mat &imag);
        void drawCentroid(Mat &imag);
        void drawText(Mat &imag);
        void reset();

    protected:

    private:
        void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2);

    private:
        double m_angle;
        Point m_cntr;
        vector<Point2d> m_eigen_vecs; //(2);
        vector<double> m_eigen_val; //(2);
        PCAOBJ m_pcapoint;

    public:
        vector<PCAOBJ> pcaobj;


};

#endif // PCA_H
