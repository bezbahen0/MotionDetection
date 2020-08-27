#include "include/MotionDetector.hpp"
#include "include/scanner.hpp"

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
    int i = 0;
    for(; i != frames.size(); ++i)
    {
        acc = acc + frames[i] * i;
    }
    acc = acc / ((1 + i) / 2 *i);
    //acc[acc > 254] = 255;
    return acc;
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
    origFrames_ = boost::circular_buffer<cv::Mat>(movementBufferSize_);
}

std::vector<cv::Rect2d>& MotionDetector::detect(cv::Mat& frame)
{
    origFrames_.push_back(frame.clone());

    int width = int(frame.size().width * backgroundSubsScalePercent_);
    int height = int(frame.size().height * backgroundSubsScalePercent_);

    int detectWidth = int(frame.size().width * pixelCompressionRatio_);
    int detectHeight = int(frame.size().height * pixelCompressionRatio_);

    frame_ = prepare(frame, width, height);

    cv::Mat frame_fp32;
    frame_.convertTo(frame_fp32, CV_32FC3);
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
    movementFrames_.push_back(frame_fp32.clone());
    cv::Mat moveFrame = genMovementFrame(movementFrames_, frame_fp32.size());
    backgroundFrame_ = backgroundAcc_ / bgFrames_.size();
    // self.background_frame[self.background+fram > 254] = 0
    if(!bgFrames_.empty())
        cv::absdiff(moveFrame, backgroundFrame_, movement_);
    else
        movement_ = cv::Mat::zeros(moveFrame.size(), CV_32FC3); 
    colorMovement_ = movement_.clone();
    //movement_[movement_ < brightnessDiscardLevel_] = 0;
    //movement_[movement_ > 0] = 254;
    movement_.convertTo(movement_, CV_8UC3);
    cv::cvtColor(movement_, movement_, cv::COLOR_BGR2GRAY);
    //movement_[movement_ > 0] = 254;
   
    return movement_;
}

std::vector<cv::Rect2d>& MotionDetector::getMovementZones(cv::Mat& frame)
{
    boxes_ = std::vector<cv::Rect2d>();
    if(bgFrames_.size() >= bgBufferSize_ / 2)
    {
        //boxes = scan(frame, expansionStep_);
        if(groupBoxes_)
        {
            //boxes_ = optimizeBoundingBoxes(boxes_);
        }
    }

    detectionBoxes_ = detection_.clone();
    for(unsigned i = 0; i != boxes_.size(); ++i)
    {
        cv::rectangle(detectionBoxes_, boxes_[i], cv::Scalar(0, 0, 250));
    }

    std::vector<cv::Rect2d> origBoxes;
    for(unsigned i = 0; i != boxes_.size(); ++i)
    {
        origBoxes.push_back(resizeBox(boxes_[i], pixelCompressionRatio_));
    }

    boxes_ = origBoxes;
    return boxes_;
} 
