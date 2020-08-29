#include "include/MotionDetector.hpp"
#include "include/scanner.hpp"
#include <opencv2/opencv.hpp>

cv::Rect2d resizeBox(cv::Rect2d box, float scale)
{
    cv::Rect2d resbox;
    resbox.x = box.x / scale;
    resbox.y = box.y / scale;
    resbox.width = box.width / scale;
    resbox.height = box.height / scale;
    return resbox;
} 
cv::Mat genMovementFrame(boost::circular_buffer<cv::Mat>& frames, cv::Size size)
{
    cv::Mat acc = cv::Mat::zeros(size, CV_32FC3);
    double i = 0.0;
    for(auto f = frames.begin(); f != frames.end(); f++)
    {
        i++;
        acc += (*f) * i;
    }
    acc = acc / ((1.0 + i) / 2.0 * i);

    cv::Mat mask = acc > 254;
    acc.setTo(255, mask);
    
    return acc.clone();
}

MotionDetector::MotionDetector(int backgroundSkipFrames,
                                float backgroundSubsScalePercent,
                                int brightnessDiscardLevel,
                                float pixelCompressionRatio,
                                int expansionStep,
                                std::size_t bgBufferSize,
                                std::size_t movementBufferSize,
                                bool groupBoxes) {
    backgroundSkipFrames_ = backgroundSkipFrames;
    backgroundSubsScalePercent_ = backgroundSubsScalePercent;
    brightnessDiscardLevel_ = brightnessDiscardLevel;
    pixelCompressionRatio_ = pixelCompressionRatio;
    expansionStep_ = expansionStep;
    bgBufferSize_ = bgBufferSize;
    movementBufferSize_ = movementBufferSize;

    count_ = 0;
    groupBoxes_ = groupBoxes;
    bgFrames_ = boost::circular_buffer<cv::Mat>(bgBufferSize_);
    movementFrames_ = boost::circular_buffer<cv::Mat>(movementBufferSize_);
}

std::list<cv::Rect2d>& MotionDetector::detect(cv::Mat& frame)
{
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
    
    boxes_ = getMovementZones(detection_);
    count_++;
    
    return boxes_;   
}
cv::Mat MotionDetector::prepare(cv::Mat& f, int width, int height)
{
    cv::Mat dst = f.clone();
    cv::resize(dst, dst, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
    cv::GaussianBlur(dst, dst, cv::Size(5, 5), 0);
    return dst;
}

cv::Mat MotionDetector::detectionBoxes() const
{
    return detectionBoxes_.clone();
}

cv::Mat MotionDetector::colorMovement() const
{
    return colorMovement_.clone();
}

void MotionDetector::updateBackgraund(cv::Mat& frame_fp32)
{
    if(!(count_ % backgroundSkipFrames_ == 0))
        return;

    cv::Mat currentFrame = frame_fp32.clone();
    if(backgroundAcc_.empty())
        backgroundAcc_ = frame_fp32.clone();
    else
    {
        backgroundAcc_ += currentFrame;
        if(bgFrames_.size() == bgBufferSize_)
        {
            backgroundAcc_ -= bgFrames_.front();
        }
    }

    bgFrames_.push_back(currentFrame);
}

cv::Mat MotionDetector::detectMovement(cv::Mat& frame_fp32)
{
    movementFrames_.push_back(frame_fp32);
    cv::Mat moveFrame = genMovementFrame(movementFrames_, frame_fp32.size());
    backgroundFrame_ = backgroundAcc_ / bgFrames_.size();

    cv::Mat mask = backgroundFrame_ > 254;
    backgroundFrame_.setTo(255, mask);

    cv::Mat movement;
    if(!bgFrames_.empty())
        cv::absdiff(moveFrame, backgroundFrame_, movement);
    else
        movement = cv::Mat::zeros(moveFrame.size(), CV_32FC3);
    colorMovement_ = movement;

    mask = movement < brightnessDiscardLevel_;
    movement.setTo(0, mask);
    mask = movement > 0;
    movement.setTo(254, mask);

    movement_.convertTo(movement_, CV_8U);
    cv::cvtColor(movement, movement, cv::COLOR_BGR2GRAY);

    return movement;
}

std::list<cv::Rect2d>& MotionDetector::getMovementZones(cv::Mat& frame)
{
    boxes_ = std::list<cv::Rect2d>();
    if(bgFrames_.size() >= bgBufferSize_ / 2)
    {
        boxes_ = scan(frame, expansionStep_);
        /*if(groupBoxes_)
        {
            boxes_ = optimizeBoundingBoxes(boxes_);
        }*/
    }

    detectionBoxes_ = detection_.clone();
    for(auto i = boxes_.begin(); i != boxes_.end(); ++i)
    {
        cv::rectangle(detectionBoxes_, *i, cv::Scalar(0, 0, 250));
    }

    std::list<cv::Rect2d> origBoxes;
    for(auto i = boxes_.begin(); i != boxes_.end(); ++i)
    {
        origBoxes.push_back(resizeBox(*i, pixelCompressionRatio_));
    }

    boxes_ = origBoxes;
    return boxes_;
} 
