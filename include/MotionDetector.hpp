#ifndef MOTIONDETECTOR_HPP
#define MOTIONDETECTOR_HPP

#include <boost/circular_buffer.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <list>

class MotionDetector
{
public:
    MotionDetector(int backgroundSkipFrames,
                    float backgroundSubsScalePercent,
                    int brightnessDiscardLevel, 
                    float pixelCompressionRatio, 
                    int expansionStep, 
                    std::size_t bgBufferSize, 
                    std::size_t movementBufferSize,
                    bool groupBoxes = true);


    std::list<cv::Rect2d>& detect(cv::Mat& frame);
    cv::Mat prepare(cv::Mat& f, int width, int height);
    cv::Mat detectionBoxes() const;
    cv::Mat colorMovement() const;

private:
    void updateBackgraund(cv::Mat& frame_fp32);
    cv::Mat detectMovement(cv::Mat& frame_fp32);
    std::list<cv::Rect2d>& getMovementZones(cv::Mat& frame);

    int count_;
    bool groupBoxes_;
    int backgroundSkipFrames_;
    float backgroundSubsScalePercent_;
    int brightnessDiscardLevel_;
    float pixelCompressionRatio_;
    int expansionStep_;
    std::size_t bgBufferSize_;
    std::size_t movementBufferSize_;

    cv::Mat movement_;
    cv::Mat colorMovement_;
    cv::Mat frame_;
    cv::Mat backgroundFrame_;
    cv::Mat detection_;
    cv::Mat backgroundAcc_;
    cv::Mat detectionBoxes_;

    std::list<cv::Rect2d> boxes_;
    boost::circular_buffer<cv::Mat> bgFrames_;
    boost::circular_buffer<cv::Mat> movementFrames_;
};

#endif /* MOTIONDETECTOR_HPP */
