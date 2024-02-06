//
// Created by kalwalt on 06/02/24.
//

#ifndef MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H
#define MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H

// File includes:
#include "Markerless/include/GeometryTypes.hpp"
#include "Markerless/include/CameraCalibration.hpp"

#include <igl/ShaderCreator.h>
#include <igl/opengl/Device.h>
#include <igl/opengl/TextureBufferExternal.h>
#include <igl/opengl/webgl/Context.h>
#include <igl/opengl/webgl/Device.h>
#include <igl/opengl/webgl/PlatformDevice.h>

class ARDrawingContextIGL {
public:
    ARDrawingContextIGL(std::string windowName, cv::Size frameSize, const CameraCalibration& c);
    ~ARDrawingContextIGL();


    bool                isPatternPresent;
    Transformation      patternPose;
private:
    bool               m_isTextureInitialized;
    unsigned int       m_backgroundTextureId;
    CameraCalibration  m_calibration;
    cv::Mat            m_backgroundImage;
    std::string        m_windowName;
};


#endif //MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H
