#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "include/MotionDetector.hpp"

cv::String keys =
        "{ help h                   |      | print this message          }"
        "{ video v                  |      | video capturing             }"
        "{ camera c                 |      | camera capturing            }"
        "{ b_height                 | 320  | height of compression frame }"
        "{ b_width                  | 320  | width of compression frame  }"
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
        cap.open(0);
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
        cap.open(parser.get<int>("camera"));
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, parser.get<int>("width"));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, parser.get<int>("height"));

    int b_height = parser.get<int>("b_height");
    int b_width = parser.get<int>("b_width");

    MotionDetector detector(
            parser.get<int>("bg_skip_frames"),
            parser.get<float>("bg_subs_scale_percent"),
            parser.get<int>("brightness_discard_level"),
            parser.get<float>("pixel_compression_ratio"),
            parser.get<int>("expansion_step"),
            parser.get<std::size_t>("bg_buffer_size"),
            parser.get<std::size_t>("move_buffer_size"));

    cv::Mat frame;
    std::vector<cv::Rect2d> boxes;
    while(true)
    {
        cap >> frame;
        
        boxes = detector.detect(frame);
        for(int i = 0; i != boxes.size(); ++i)
        {
            cv::rectangle(frame, boxes[i], cv::Scalar(0, 0, 255));
        }  
        
        if((char)cv::waitKey(1) == 'q')
        {
            break;
        }
        cv::imshow("frame", frame);
        cv::imshow("detect_boxes", detector.detectionBoxes());
    }

    return EXIT_SUCCESS;
}
