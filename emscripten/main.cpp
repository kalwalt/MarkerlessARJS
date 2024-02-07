//
// Created by @kalwalt on 06/12/2023.
//

#include <emscripten/html5.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <igl/IGL.h>
#include <igl/opengl/webgl/Context.h>
#include <igl/opengl/webgl/Device.h>
#include <Common.h>

#include <ARPipeline.hpp>
#include <CameraCalibration.hpp>
#include <GeometryTypes.hpp>
#include <Pattern.hpp>
#include <PatternDetector.hpp>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

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

GLFWwindow *window_ = nullptr;
int width_ = 0;
int height_ = 0;

std::unique_ptr <igl::IDevice> device_;
std::shared_ptr <igl::ICommandQueue> commandQueue_;
igl::RenderPassDesc renderPass_;
std::shared_ptr <igl::IFramebuffer> framebuffer_;
std::shared_ptr <igl::IRenderPipelineState> renderPipelineState_Triangle_;

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

void render(const std::shared_ptr <igl::ITexture> &nativeDrawable) {
    const auto size = framebuffer_->getColorAttachment(0)->getSize();
    if (size.width != width_ || size.height != height_) {
        createFramebuffer(nativeDrawable);
    } else {
        framebuffer_->updateDrawable(nativeDrawable);
    }

    // Command buffers (1-N per thread): create, submit and forget
    igl::CommandBufferDesc cbDesc;
    std::shared_ptr <igl::ICommandBuffer> buffer = commandQueue_->createCommandBuffer(cbDesc, nullptr);

    const igl::Viewport viewport = {0.0f, 0.0f, (float) width_, (float) height_, 0.0f, +1.0f};
    const igl::ScissorRect scissor = {0, 0, (uint32_t) width_, (uint32_t) height_};

    // This will clear the framebuffer
    auto commands = buffer->createRenderCommandEncoder(renderPass_, framebuffer_);

    commands->bindRenderPipelineState(renderPipelineState_Triangle_);
    commands->bindViewport(viewport);
    commands->bindScissorRect(scissor);
    commands->pushDebugGroupLabel("Render Triangle", igl::Color(1, 0, 0));
    commands->draw(igl::PrimitiveType::Triangle, 0, 6);
    commands->popDebugGroupLabel();
    commands->endEncoding();

    buffer->present(nativeDrawable);

    commandQueue_->submit(*buffer);
}


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

bool initWindow(GLFWwindow **outWindow) {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, true);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);
    glfwWindowHint(GLFW_SRGB_CAPABLE, true);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    getRenderingBufferSize(width_, height_);
    GLFWwindow *window = glfwCreateWindow(width_, height_, "MarrkerlessARJS Example", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return false;
    }

    glfwSetErrorCallback([](int error, const char *description) {
        printf("GLFW Error (%i): %s\n", error, description);
    });

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int, int action, int) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });

    // @lint-ignore CLANGTIDY
    glfwSetWindowSizeCallback(window, [](GLFWwindow * /*window*/, int width, int height) {
        printf("Window resized! width=%d, height=%d\n", width, height);
    });

    if (outWindow) {
        *outWindow = window;
    }

    return true;
}

void initGL() {
    // create a device

    {
        auto ctx =
                std::make_unique<igl::opengl::webgl::Context>(igl::opengl::RenderingAPI::GLES3, "#canvas");
        device_ = std::make_unique<igl::opengl::webgl::Device>(std::move(ctx));

        IGL_ASSERT(device_);
    }

    // Command queue: backed by different types of GPU HW queues
    igl::CommandQueueDesc desc{igl::CommandQueueType::Graphics};
    commandQueue_ = device_->createCommandQueue(desc, nullptr);

    // Color attachment
    renderPass_.colorAttachments[0] = igl::RenderPassDesc::ColorAttachmentDesc{};
    renderPass_.colorAttachments[0].loadAction = igl::LoadAction::Clear;
    renderPass_.colorAttachments[0].storeAction = igl::StoreAction::Store;
    renderPass_.colorAttachments[0].clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
    renderPass_.depthAttachment.loadAction = igl::LoadAction::DontCare;
}


//! Load an image from file
uint8_t *loadImage(std::string filename, int width, int height) {
    int comp;

    const uint8_t *img = stbi_load(filename.c_str(), &width, &height, &comp, 3);
    return (uint8_t *) img;
}

int main(int argc, char *argv[]) {
    // Change this calibration to yours:
    CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);
    int width = 640;
    int height = 480;
    cv::Size frameSize(width, height);

    renderPass_.colorAttachments.resize(1);
    initWindow(&window_);
    initGL();

    printf("webGL initialized\n");

    createFramebuffer(getNativeDrawable());
    createRenderPipeline();

    auto u8 = loadImage("data/pinball.jpg", 640, 480);

    printf("patternImage converted\n");

    cv::Mat grayImage(height, width, CV_8UC1);
    cv::Mat patternImageMat = cv::Mat(height, width, CV_8UC4, u8);
    cv::cvtColor(patternImageMat, grayImage, cv::COLOR_RGBA2GRAY);
    ARPipeline pipeline(grayImage, calibration);

    printf("pipeline ok\n");


    // Main loop
    emscripten_set_main_loop(&emscriptenMainLoopCallback, 0, 1);

    renderPipelineState_Triangle_.reset();
    framebuffer_.reset();
    commandQueue_.reset();
    device_.reset(nullptr);

    glfwDestroyWindow(window_);
    glfwTerminate();

    return 0;
}