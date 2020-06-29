#include "utils.h"

void show_block(int channel, float *block, const Point4D &shape, const Point4D &stride, const char *window) {
#if !!LFCODEC_USE_OPENCV
    int img[shape.v * shape.y][shape.u * shape.x];
    const char *channel_name;
    cv::String title = "Block Viewer ";
    auto size = cv::Size(13*15, 13*15);
    cv::Mat mat = cv::Mat::zeros(size, CV_32F);

    for (int v = 0; v < shape.v; v++)
        for (int u = 0; u < shape.u; u++)
            for (int y = 0; y < shape.y; y++)
                for (int x = 0; x < shape.x; x++) {
                    int index = x * stride.x + y * stride.y + u * stride.u + v * stride.v;
                    auto mat_x = x * shape.u + u;
                    auto mat_y = y * shape.v + v;
                    mat.at<float>(mat_y, mat_x) = (block[index] + 512.0F) / 1024.0F;
                }
    

    switch (channel) {
        case 0: channel_name = "Y"; break;
        case 1: channel_name = "Cb"; break;
        case 2: channel_name = "Cr"; break;
    }
    title += channel_name;
    title += " ";
    title += window;
    cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
    cv::imshow(title, mat);
    cv::waitKey(0);
#endif
}