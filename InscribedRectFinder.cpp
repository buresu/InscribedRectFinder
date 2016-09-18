#include "InscribedRectFinder.h"

cv::Rect InscribedRectFinder::findRectangle(const cv::Mat &binary) const
{
    // Rows and Cols
    const int rows = binary.rows;
    const int cols = binary.cols;

    // Create the Square Map
    cv::Mat squareMap;

    binary.convertTo(squareMap, CV_32SC1);

    squareMap.forEach<int>([](int &i, const int *) -> void {
        i = (i + 1) >> 8;
    });

    int maxSquare = 0;

    for (int r(rows - 2); r >= 0; --r) {

        const int step = int(squareMap.step1());
        int *p = reinterpret_cast<int*>(squareMap.data) + step * r + squareMap.cols - 1;

        for (int c(cols - 2); c >= 0; --c, p--) {
            if (*p) {
                const int rv = *(p + 1);
                const int bv = *(p + step);
                const int rbv = *(p + step + 1);
                *p = std::min(rbv, std::min(rv, bv)) + 1;
                maxSquare = std::max(maxSquare, *p);
            }
        }
    }

    // Create the Width, Height and Area Map
    cv::Mat areaMap = squareMap.clone();

    areaMap.forEach<int>([](int &i, const int *) -> void {
        i = i << 1;
    });

    cv::Mat widthMap = cv::Mat(squareMap.rows, squareMap.cols, squareMap.type());
    cv::Mat heightMap = cv::Mat(squareMap.rows, squareMap.cols, squareMap.type());
    std::vector<int> height2width(std::size_t(maxSquare + 1));

    for (int r(0); r < rows; ++r) {

        const int index = int(squareMap.step1()) * r + cols - 1;
        int *sp = reinterpret_cast<int*>(squareMap.data) + index;
        int *ap = reinterpret_cast<int*>(areaMap.data) + index;
        int *wp = reinterpret_cast<int*>(widthMap.data) + index;
        int *hp = reinterpret_cast<int*>(heightMap.data) + index;

        std::fill(height2width.begin(), height2width.end(), 0);

        for (int c(cols - 1); c >= 0; --c, --sp, --ap, --wp, --hp) {

            if (*sp) {

                int max = *ap;

                for (int h = *sp; h > 0; --h) {

                    const int w = std::max(height2width[std::size_t(h)] + 1, *sp);
                    const int size = w * h; // w + h?

                    if (size >= max) {
                        *wp = w;
                        *hp = h;
                        max = size;
                    }

                    height2width[std::size_t(h)] = w;
                }

                *ap = max;
            }

            std::fill(height2width.begin() + *sp, height2width.end(), 0);
        }
    }

    std::vector<int> width2height(std::size_t(maxSquare + 1));

    for (int c(0); c < cols; ++c) {

        const int step = int(squareMap.step1());
        const int index = step * (squareMap.rows - 1) + c;
        int *sp = reinterpret_cast<int*>(squareMap.data) + index;
        int *ap = reinterpret_cast<int*>(areaMap.data) + index;
        int *wp = reinterpret_cast<int*>(widthMap.data) + index;
        int *hp = reinterpret_cast<int*>(heightMap.data) + index;

        std::fill(width2height.begin(), width2height.end(), 0);

        for (int r(rows - 1); r >= 0; --r, sp -= step, ap -= step, wp -= step, hp -= step) {

            if (*sp) {

                int max = *ap;

                for (int w = *sp; w > 0; --w) {

                    const int h = std::max(width2height[std::size_t(w)] + 1, *sp);
                    const int size = w * h; // w + h?

                    if (size > max) {
                        *wp = w;
                        *hp = h;
                        max = size;
                    }

                    width2height[std::size_t(w)] = h;
                }

                *ap = max;
            }

            std::fill(width2height.begin() + *sp, width2height.end(), 0);
        }
    }

    // Find the Largest Rectangle
    int x = 0, y = 0, width = 0, height = 0, max = 0;
    const bool useAspect = mUseAspectRatio ? (mAspectRatio > 0.0 ? true : false) : false;
    const double aspect = mAspectRatio;
    const int minArea = mMinArea;
    const int maxArea = mMaxArea;

    for (int r(0); r < rows; ++r) {

        if ((rows - r) * cols <= max)
            break;

        const int index = int(squareMap.step1()) * r;
        int *wp = reinterpret_cast<int*>(widthMap.data) + index;
        int *hp = reinterpret_cast<int*>(heightMap.data) + index;

        for (int c(0); c < cols; ++c, wp++, hp++) {

            if ((rows - r) * (cols - c) <= max)
                break;

            const double a = useAspect ? *wp / double(*hp) : 0;
            const int w = useAspect ? int(a > aspect ? *hp * aspect : *wp) : *wp;
            const int h = useAspect ? int(a > aspect ? *hp : *wp / aspect) : *hp;
            const int size = w * h;

            if (size > max && size >= minArea && size <= maxArea) {
                x = c;
                y = r;
                width = w;
                height = h;
                max = size;
            }
        }
    }

    return cv::Rect(x, y, width, height);
}
