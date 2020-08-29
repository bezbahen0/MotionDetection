#include "include/scanner.hpp"

#define MAX(x, y) (x > y ? (x) : (y))
#define MIN(x, y) (x < y ? (x) : (y))

#define ABS(x) (x < 0 ? (-(x)) : (x))

std::vector<cv::Rect2d> scan(const cv::Mat& image, const int expansionStep)
{
    std::vector<cv::Rect2d> boxes;

    int height = image.size().height;
    int width = image.size().width;

    std::vector<Coord> scanPoitns;
    cv::Mat avoidPoints = cv::Mat::zeros(image.size(), image.type());

    for(int r = 0; r < height; ++r)
    {
        for(int c = 0; c < width; ++c)
        {
            if(0 < image.at<int>(r, c))
            {
                scanPoitns.push_back(Coord(r, c)); 
            }
        }
    }

    for(auto coord = scanPoitns.begin(); coord != scanPoitns.end(); coord++)
    {
        if(!avoidPoints.at<unsigned char>(coord -> r, coord -> c))
        {
            int r_min = height;
            int c_min = width;
            int r_max = 0;
            int c_max = 0;

            std::vector<Coord> temp;
            temp.push_back(Coord(coord -> r, coord -> c));
            
            for(auto temp_cord = temp.begin(); temp_cord != temp.end(); temp_cord++)
            {
                r_min = MIN(r_min, temp_cord -> r);
                c_min = MIN(c_min, temp_cord -> c);
                r_max = MAX(r_max, temp_cord -> r);
                c_max = MAX(c_max, temp_cord -> c);

                for(int i = -expansionStep; i < expansionStep + 1; ++i)
                {
                    for(int j = -expansionStep; j < expansionStep + 1; ++j)
                    {
                        int r = i + temp_cord -> r;
                        int c = j + temp_cord -> c;

                        if(r >= 0 && c >= 0 && r < height && c < width && !avoidPoints.at<unsigned char>(r, c))
                        {
                            avoidPoints.at<int>(r, c) = 1;
                            /*if(ABS(i) == expansionStep || ABS(j) == expansionStep)
                            {
                                temp.push_back(Coord(r, c));
                            }*/
                        }
                    }
                }

                r_min = MAX(0, r_min - expansionStep);
                c_min = MAX(0, c_min - expansionStep);

                r_max = MIN(height - 1, r_max + expansionStep);
                c_max = MIN(width - 1, c_max + expansionStep);

                boxes.push_back(cv::Rect2d(cv::Point(c_min, r_min), cv::Point(c_max, r_max)));
            }
        }
    }


    return boxes;
}
