#pragma once

#include <opencv2/opencv.hpp>

class InscribedRectFinder
{
public:
    InscribedRectFinder() {}
    ~InscribedRectFinder() {}

    /* Find Rectangle */
    cv::Rect findRectangle(const cv::Mat &binary) const;

    /* Use Aspect Ratio */
    bool useAspectRatio() const { return mUseAspectRatio; }

    /* Set Use Aspect Ratio */
    void setUseAspectRatio(bool b) { mUseAspectRatio = b; }

    /* Aspect Ratio */
    double aspectRatio() const { return mAspectRatio; }

    /* Set Aspect Ratio */
    void setAspectRatio(double aspect) { mAspectRatio = aspect; }

    /* Min Area */
    int minArea() const { return mMinArea; }

    /* Set Min Area */
    void setMinArea(int area)  { mMinArea = area; }

    /* Max Area */
    int maxArea() const { return mMaxArea; }

    /* Set Max Area */
    void setMaxArea(int area)  { mMaxArea = area; }

private:
    /* Use Aspect Ratio */
    bool mUseAspectRatio = false;

    /* Aspect Ratio */
    double mAspectRatio = 1.0;

    /* Min Area Size */
    int mMinArea = 10 * 10;

    /* Max Area Sizs */
    int mMaxArea = 10000 * 10000;
};
