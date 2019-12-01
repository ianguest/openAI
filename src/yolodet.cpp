#include "yolodet.h"

yolodet::yolodet()
{
    // Initialize the parameters
    m_confThreshold = 0.1; // Confidence threshold
    m_nmsThreshold = 0.2;  // Non-maximum suppression threshold
    m_inpWidth = 416;        // Width of network's input image
    m_inpHeight = 416;       // Height of network's input image

    // Load names of classes
    string classesFile = "coco.names";
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line))
    {
        m_classes.push_back(line);
    }

    // Give the configuration and weight files for the model
    String modelConfiguration = "yolov3-tiny.cfg";
    String modelWeights = "yolov3-tiny.weights";

    // Load the network
    m_net = readNetFromDarknet(modelConfiguration, modelWeights);
    m_net.setPreferableBackend(DNN_BACKEND_OPENCV);
    m_net.setPreferableTarget(DNN_TARGET_CPU);

}

int yolodet::step(InputArray image, double *confidence)
{
    Mat tempmat = image.getMat();
    Mat blob;
    blobFromImage(tempmat, blob, 1/255.0, Size(m_inpWidth, m_inpHeight), Scalar(0,0,0), true, false);
    //Sets the input to the network
    m_net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    vector<Mat> outs;
    m_net.forward(outs, getOutputsNames(m_net));

    // Remove the bounding boxes with low confidence
    return postprocess(tempmat, outs,confidence);


}

int yolodet::postprocess(Mat& frame, const vector<Mat>& outs, double *confidence)
{
    m_classIds.clear();
    m_confidences.clear();
    m_boxes.clear();
    double highconfidence = 0;
    int type = 0;  // 0 is nothing, 1 is human, 2 is vehicle

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > m_confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                m_classIds.push_back(classIdPoint.x);
                m_confidences.push_back((float)confidence);
                m_boxes.push_back(Rect(left, top, width, height));
                if (confidence > highconfidence)
                {

                    if (m_classes[classIdPoint.x] == "person")
                    {
                        type = 1;
                        highconfidence = confidence;
                    }
                    if (m_classes[classIdPoint.x] == "bicycle")
                    {
                        type = 1;
                        highconfidence = confidence;
                    }
                    if (m_classes[classIdPoint.x] == "car")
                    {
                        type = 2;
                        highconfidence = confidence;
                    }
                    if (m_classes[classIdPoint.x] == "motorbike")
                    {
                        type = 2;
                        highconfidence = confidence;
                    }
                    if (m_classes[classIdPoint.x] == "bus")
                    {
                        type = 2;
                        highconfidence = confidence;
                    }
                    if (m_classes[classIdPoint.x] == "truck")
                    {
                        type = 2;
                        highconfidence = confidence;
                    }
                }
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    m_indices.clear();
    NMSBoxes(m_boxes, m_confidences, m_confThreshold, m_nmsThreshold, m_indices);
    // fill in detections
    m_detobj.clear();
    for (size_t i = 0; i < m_indices.size(); ++i)
    {
        int idx = m_indices[i];
        Rect box = m_boxes[idx];
        if (m_classes[m_classIds[idx]] == "person")
        {
            m_obj.box = box;
            m_obj.meancol = box.x + box.width/2;
            m_obj.meanrow = box.y + box.height/2;
            m_obj.type = 1;
            m_detobj.push_back(m_obj);
        }
    }

    *confidence = highconfidence;
    return type;

}

void yolodet::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!m_classes.empty())
    {
        CV_Assert(classId < (int)m_classes.size());
        label = m_classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}

vector<String> yolodet::getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

void yolodet::draw(InputOutputArray image)
{
    Mat tempmat = image.getMat();
    for (size_t i = 0; i < m_indices.size(); ++i)
    {
        int idx = m_indices[i];
        Rect box = m_boxes[idx];
        if (m_classes[m_classIds[idx]] == "person")
        {
         drawPred(m_classIds[idx], m_confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, tempmat);
        }
        if (m_classes[m_classIds[idx]] == "car")
        {
         drawPred(m_classIds[idx], m_confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, tempmat);
        }
        if (m_classes[m_classIds[idx]] == "bus")
        {
         drawPred(m_classIds[idx], m_confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, tempmat);
        }
        if (m_classes[m_classIds[idx]] == "truck")
        {
         drawPred(m_classIds[idx], m_confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, tempmat);
        }




    }
}

yolodet::~yolodet()
{
    //dtor
}
