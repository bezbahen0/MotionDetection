#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

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
        "{ expansion_step           | 5    |                             }"
        ;

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    
    cv::VideoCaputure cap;

    if(argc == 1)
    {
        cap.open(0);
    }
    if(parser.has("help") || argc == 1)
    {
        parser.printMessage();
        return EXIT_SUCCESS;
    }
    if(parser.has("video"))
    {
        cap.open(parser.get<cv::String>("video");
    }
    else if(parser.has("camera"))
    {
        cap.open(parser.get<int>("camera"))
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, parser.get<int>("width"));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, parser.get<int>("height"));


    return EXIT_SUCCESS;
}
