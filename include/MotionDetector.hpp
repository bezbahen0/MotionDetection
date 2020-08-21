#ifndef MOTIONDETECTOR_HPP
#define MOTIONDETECTOR_HPP

class MotionDetector
{
public:
    MotionDetector(int backgroundSkipFrames,
                    float backgroundSubsScalePercent,
                    int brightnessDiscardLevel, 
                    float pixelCompressionRatio, 
                    int expansionStep, 
                    std::size_t bgBufferSize, 
                    std::size_t movementBufferSize);

    void detection(cv::Mat& frame);
    void prepare();

private:
    void updateBackgraund();
    void detectionMovement();
    void getMovementZones();

    int backgroundSkipFrames_;
    float backgroundSubsScalePercent_;
    int brightnessDiscardLevel_;
    float pixelCompressionRatio_;
    int expansionStep_;
    std::size_t bgBufferSize_;
    std::size_t movementBufferSize_;


};

#endif /* MOTIONDETECTOR_HPP */
