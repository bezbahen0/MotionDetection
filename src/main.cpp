#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

#include "include/MotionDetector.hpp"

cv::String keys =
        "{ help h                   |      | print this message          }"
        "{ video v                  |      | video capturing             }"
        "{ camera c                 | 0    | camera capturing            }"
        "{ height                   | 1980 | height of frame             }"
        "{ width                    | 1080 | width of frame              }"
        "{ bg_buffer_size           | 10   | backgraund buffer size      }"
        "{ move_buffer_size         | 2    | movement buffer size        }"
        "{ bg_skip_frames           | 1    | skip frames for processing  }"
        "{ bg_subs_scale_percent    | 0.2  |                             }"
        "{ brightness_discard_level | 20   |                             }"
        "{ pixel_compression_ratio  | 0.1  |                             }"
        "{ expansion_step           | 5    |                             }" ;

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    
    cv::VideoCapture cap;

    if(argc == 1)
    {
        cap.open(0, cv::CAP_V4L);
    }
    if(parser.has("help"))
    {
        parser.printMessage();
        return EXIT_SUCCESS;
    }
    if(parser.has("video"))
    {
        cap.open(parser.get<cv::String>("video"));
    }
    else if(parser.has("camera"))
    {
        cap.open(parser.get<int>("camera"), cv::CAP_V4L);
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, parser.get<int>("width"));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, parser.get<int>("height"));

    MotionDetector detector(
            parser.get<int>("bg_skip_frames"),
            parser.get<float>("bg_subs_scale_percent"),
            parser.get<int>("brightness_discard_level"),
            parser.get<float>("pixel_compression_ratio"),
            parser.get<int>("expansion_step"),
            parser.get<std::size_t>("bg_buffer_size"),
            parser.get<std::size_t>("move_buffer_size"));

    cv::Mat frame;
    std::list<cv::Rect2d> boxes;
    while(true)
    {
//        int64 t0 = cv::getTickCount();
        cap >> frame;
        
        boxes = detector.detect(frame);
        for(auto i = boxes.begin(); i != boxes.end(); ++i)
        {
            cv::rectangle(frame, *i, cv::Scalar(0, 0, 255));
        }  
        
        cv::imshow("frame", frame);
        cv::imshow("detect_boxes", detector.detectionBoxes());
        cv::imshow("color_movement", detector.colorMovement());

//        int64 t1 = cv::getTickCount();
//        double secs = (t1-t0)/cv::getTickFrequency();
//        std::cout << "Time taken : " << secs << " seconds" << std::endl;

        if((char)cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}
