//
// Created by kalwalt on 06/02/24.
//

#include "ARDrawingContextIGL.hpp"

ARDrawingContextIGL::ARDrawingContextIGL(std::string windowName, cv::Size frameSize, const CameraCalibration& c)
        : m_isTextureInitialized(false)
        , m_calibration(c)
        , m_windowName(windowName)
        {}


ARDrawingContextIGL::~ARDrawingContextIGL() {}
