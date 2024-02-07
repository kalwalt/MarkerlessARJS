//
// Created by kalwalt on 06/02/24.
//

#ifndef MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H
#define MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H

// File includes:
#include "Markerless/include/GeometryTypes.hpp"
#include "Markerless/include/CameraCalibration.hpp"

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <igl/IGL.h>
#include <igl/opengl/webgl/Context.h>
#include <igl/opengl/webgl/Device.h>
#include <Common.h>

class ARDrawingContextIGL {
public:
    ARDrawingContextIGL(std::string windowName, cv::Size frameSize, const CameraCalibration &c);

    ~ARDrawingContextIGL();

    bool initialize();

    bool isPatternPresent;
    Transformation patternPose;
    const char *canvas = "#canvas";

    static constexpr const char *codeVS = R"(#version 300 es

precision highp float;

out vec3 vColor;
const vec2 pos[6] = vec2[6](
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0)
);
const vec3 col[6] = vec3[6](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
    vec3(0.0, 0.0, 1.0),
	vec3(0.0, 1.0, 0.0),
	vec3(1.0, 0.0, 0.0)
);
void main() {
	gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
	vColor = col[gl_VertexID];
}
)";

    static constexpr const char *codeFS = R"(#version 300 es

precision highp float;

in vec3 vColor;
layout (location=0) out vec4 oColor;
void main() {
	oColor = vec4(vColor, 1.0);
}
)";


    void emscriptenMainLoopCallback() {

        render(getNativeDrawable());
        glfwPollEvents();

    }

    void destroyWindow() {
        renderPipelineState_Triangle_.reset();
        framebuffer_.reset();
        commandQueue_.reset();
        device_.reset(nullptr);
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

private:
    bool initWindow(GLFWwindow **outWindow);

    void initGL();

    void render(const std::shared_ptr <igl::ITexture> &nativeDrawable);

    std::shared_ptr <igl::ITexture> getNativeDrawable() {
        const auto &platformDevice = device_->getPlatformDevice<igl::opengl::webgl::PlatformDevice>();
        IGL_ASSERT(platformDevice != nullptr);

        getRenderingBufferSize(width_, height_);

        igl::Result ret;
        std::shared_ptr <igl::ITexture> drawable =
                platformDevice->createTextureFromNativeDrawable(width_, height_, &ret);

        IGL_ASSERT_MSG(ret.isOk(), ret.message.c_str());
        IGL_ASSERT(drawable != nullptr);
        return drawable;
    }

    void createFramebuffer(const std::shared_ptr <igl::ITexture> &nativeDrawable) {
        igl::FramebufferDesc framebufferDesc;
        framebufferDesc.colorAttachments[0].texture = nativeDrawable;

        const igl::TextureDesc desc = igl::TextureDesc::new2D(nativeDrawable->getFormat(),
                                                              nativeDrawable->getDimensions().width,
                                                              nativeDrawable->getDimensions().height,
                                                              igl::TextureDesc::TextureUsageBits::Attachment |
                                                              igl::TextureDesc::TextureUsageBits::Sampled,
                                                              framebufferDesc.debugName.c_str());

        framebufferDesc.colorAttachments[1].texture = device_->createTexture(desc, nullptr);
        framebuffer_ = device_->createFramebuffer(framebufferDesc, nullptr);
        IGL_ASSERT(framebuffer_);
    }

    void createRenderPipeline() {
        if (renderPipelineState_Triangle_) {
            return;
        }

        IGL_ASSERT(framebuffer_);

        igl::RenderPipelineDesc desc;

        desc.targetDesc.colorAttachments.resize(1);

        // @fb-only
        if (framebuffer_->getColorAttachment(0)) {
            desc.targetDesc.colorAttachments[0].textureFormat =
                    framebuffer_->getColorAttachment(0)->getFormat();
        }

        if (framebuffer_->getDepthAttachment()) {
            desc.targetDesc.depthAttachmentFormat = framebuffer_->getDepthAttachment()->getFormat();
        }

        desc.shaderStages = igl::ShaderStagesCreator::fromModuleStringInput(
                *device_, codeVS, "main", "", codeFS, "main", "", nullptr);
        renderPipelineState_Triangle_ = device_->createRenderPipeline(desc, nullptr);
        IGL_ASSERT(renderPipelineState_Triangle_);
    }

    bool m_isTextureInitialized;
    unsigned int m_backgroundTextureId;
    CameraCalibration m_calibration;
    cv::Mat m_backgroundImage;
    std::string m_windowName;
    // igl private properties
    GLFWwindow *window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    std::unique_ptr <igl::IDevice> device_;
    std::shared_ptr <igl::ICommandQueue> commandQueue_;
    igl::RenderPassDesc renderPass_;
    std::shared_ptr <igl::IFramebuffer> framebuffer_;
    std::shared_ptr <igl::IRenderPipelineState> renderPipelineState_Triangle_;
};


#endif //MARKERLESSARJS_ARDRAWINGCONTEXTIGL_H
