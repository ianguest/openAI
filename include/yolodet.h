#ifndef YOLODET_H
#define YOLODET_H

#include "opencv2/opencv.hpp"

#include "pca.h"



using namespace std;
using namespace cv;
using namespace dnn;

class yolodet
{
    public:
        yolodet();
        virtual ~yolodet();
        int step(InputArray image, double *confidence);
        void draw(InputOutputArray image);

    protected:

    private:
        float m_confThreshold; // Confidence threshold
        float m_nmsThreshold;  // Non-maximum suppression threshold
        int m_inpWidth;        // Width of network's input image
        int m_inpHeight;       // Height of network's input image
        vector<string> m_classes;
        Net m_net;
        vector<int> m_classIds;
        vector<float> m_confidences;
        vector<Rect> m_boxes;
        vector<int> m_indices;
        PCAOBJ m_obj;

    private:
        // Remove the bounding boxes with low confidence using non-maxima suppression
        int postprocess(Mat& frame, const vector<Mat>& out, double *confidence);

        // Draw the predicted bounding box
        void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

        // Get the names of the output layers
        vector<String> getOutputsNames(const Net& net);

    public:
        vector<PCAOBJ> m_detobj;

};

#endif // YOLODET_H
