#include "utils.h"
#include "Transform.h"

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

void progress_bar(double progress, int bar_length) {
    std::cout << "[";
    int pos = bar_length * progress;
    for (int i = 0; i < bar_length; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();    
}

void flip_axis(float *block, unsigned to_flip, unsigned flat_size, Point4D shape, Point4D stride) {
    float _block[flat_size];
    for (int v = 0; v < shape.v; v++) {
        for (int u = 0; u < shape.u; u++) {
            for (int y = 0; y < shape.y; y++) {
                for (int x = 0; x < shape.x; x++) {
                    auto dx = to_flip & Transform::AXIS_X ? shape.x - 1 - 2 * x : 0;
                    auto dy = to_flip & Transform::AXIS_Y ? shape.y - 1 - 2 * y : 0;
                    auto du = to_flip & Transform::AXIS_U ? shape.u - 1 - 2 * u : 0;
                    auto dv = to_flip & Transform::AXIS_V ? shape.v - 1 - 2 * v : 0;
                    auto f_offset = offset(x, y, u, v, stride);
                    auto r_offset = offset(x + dx, y + dy, u + du, v + dv, stride);
                    _block[r_offset] = block[f_offset];
                }
            }
        }
    }
    for (int v = 0; v < shape.v; v++) {
        for (int u = 0; u < shape.u; u++) {
            for (int y = 0; y < shape.y; y++) {
                for (int x = 0; x < shape.x; x++) {
                    auto index = offset(x, y, u, v, stride);
                    block[index] = _block[index];
                }
            }
        }
    }
}

inline void extend_borders(float *block, const Point4D &shape, const Point4D &stride) {
    for (int y = 0; y < shape.y; y++)
        for (int x = 0; x < shape.x; x++)
            for (int v = 0; v < shape.v; v++) {
                int u = 0;
                int value;
                // Left extension
                while ((value = block[offset(x, y, u, v, stride)]) == 0)
                    u++;
                for (int i = 0; i < u; i++)
                    block[offset(x, y, i, v, stride)] = value;

                // Right extension
                u = shape.u - 1;
                while ((value = block[offset(x, y, u, v, stride)]) == 0)
                    u--;
                for (int i = shape.u - 1; i > u; i--)
                    block[offset(x, y, i, v, stride)] = value;
            }
}