#include "include/MotionDetector.hpp"

#include <opencv2/imgproc.hpp>

MotionDetector::MotionDetector(int backgroundSkipFrames,
                                float backgroundSubsScalePercent,
                                int brightnessDiscardLevel,
                                float pixelCompressionRatio,
                                int expansionStep,
                                std::size_t bgBufferSize,
                                std::size_t movementBufferSize)
{
    backgroundSkipFrames_ = backgroundSkipFrames;
    backgroundSubsScalePercent_ = backgroundSubsScalePercent;
    brightnessDiscardLevel_ = brightnessDiscardLevel;
    pixelCompressionRatio_ = pixelCompressionRatio;
    expansionStep_ = expansionStep;
    bgBufferSize_ = bgBufferSize;
    movementBufferSize_ = movementBufferSize;

    count_ = 0;
    bgFrames_ = boost::circular_buffer<cv::Mat>(bgBufferSize_);
    movementFrames_ = boost::circular_buffer<cv::Mat>(movementBufferSize_);
    origFrames_ = boost::circular_buffer<cv::Mat>(movementBufferSize_);
}

void MotionDetector::detect(cv::Mat& frame)
{
    origFrames_.push_back(frame);

    int width = int(frame.size().width * backgroundSubsScalePercent_);
    int height = int(frame.size().height * backgroundSubsScalePercent_);

    int detectWidth = int(frame.size().width * pixelCompressionRatio_);
    int detectHeight = int(frame.size().height * pixelCompressionRatio_);

    frame_ = prepare(frame, width, height);
    cv::Mat frame_fp32;
    frame_.convertTo(frame_fp32, CV_32F);
    updateBackgraund(frame_fp32);
    
    movement_ = detectMovement(frame_fp32);
    cv::resize(movement_, detection_, cv::Size(detectWidth, detectHeight), 0, 0, cv::INTER_CUBIC);
    
    //boxes = getMovementZones(detection_);
    count_++;
    //return boxes, origFrames_[0]   
}
cv::Mat MotionDetector::prepare(cv::Mat& f, int width, int height)
{
    cv::Mat dst = f.clone();
    cv::resize(dst, dst, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
    cv::GaussianBlur(dst, dst, cv::Size(5, 5), 0);
    return dst;
}

void MotionDetector::updateBackgraund(cv::Mat& frame_fp32)
{
    if(!(count_ % backgroundSkipFrames_ == 0))
        return;

    cv::Mat currentFrame;
    if(movementFrames_.size() == movementBufferSize_)
        currentFrame = movementFrames_.front();
    else
        currentFrame = frame_fp32.clone();

    if(backgroundAcc_.empty())
        backgroundAcc_ = frame_fp32;
    else
    {
        backgroundAcc_ += currentFrame;
        if(bgFrames_.size() == bgBufferSize_)
        {
            cv::Mat subsFrame = bgFrames_.front();
            backgroundAcc_ -= subsFrame;
        }
    }

    bgFrames_.push_back(currentFrame);
}

cv::Mat MotionDetector::detectMovement(cv::Mat& frame_fp32)
{
    movementFrames_.push_back(frame_fp32);
    cv::Mat moveFrame;
    backgroundFrame_ = backgroundAcc_ / bgFrames_.size();
    if(!bgFrames_.empty())
        cv::absdiff(moveFrame, backgroundFrame_, movement_);
    else
        movement_ = cv::Mat::zeros(moveFrame.size(), CV_32F); 
    colorMovement_ = movement_.clone();
    //movement_[movement_ < brightnessDiscardLevel_] = 0;
    //movement_[movement_ > 0] = 254;
    movement_.convertTo(movement_, CV_8U);
    cv::cvtColor(movement_, movement_, cv::COLOR_BGR2GRAY);
    //movement_[movement_ > 0] = 254;
   
    return movement_;
}

void MotionDetector::getMovementZones()
{
} 
