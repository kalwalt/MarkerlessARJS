//
// Created by kalwalt on 06/02/24.
//

#include "ARDrawingContextIGL.hpp"

ARDrawingContextIGL::ARDrawingContextIGL(std::string windowName, cv::Size frameSize, const CameraCalibration &c)
        : m_isTextureInitialized(false), m_calibration(c), m_windowName(windowName) {

}


ARDrawingContextIGL::~ARDrawingContextIGL() {}

bool ARDrawingContextIGL::initialize() {
    renderPass_.colorAttachments.resize(1);
    initWindow(&window_);
    initGL();
    createFramebuffer(getNativeDrawable());
    createRenderPipeline();
    return true;
}

bool ARDrawingContextIGL::initWindow(GLFWwindow **outWindow) {
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
    GLFWwindow *window = glfwCreateWindow(width_, height_, m_windowName.c_str(), nullptr, nullptr);

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

void ARDrawingContextIGL::initGL() {
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

void ARDrawingContextIGL::render(const std::shared_ptr <igl::ITexture> &nativeDrawable) {
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
