#include "framebuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(OpenGLContext* context, unsigned int width, unsigned int height, unsigned int devicePixelRatio)
    : mp_context(context), m_width(width), m_height(height),
    m_devicePixelRatio(devicePixelRatio), m_created(false),
    m_currentFrame{0,0,0}
{}

FrameBuffer::~FrameBuffer() {
    destroy();
}

void FrameBuffer::createFrameBufferData(FrameBufferData& data) {
    mp_context->glGenFramebuffers(1, &data.frameBuffer);
    mp_context->glGenTextures(1, &data.outputTexture);
    mp_context->glGenRenderbuffers(1, &data.depthRenderBuffer);

    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, data.frameBuffer);
    mp_context->glBindTexture(GL_TEXTURE_2D, data.outputTexture);

    mp_context->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
                             m_width * m_devicePixelRatio,
                             m_height * m_devicePixelRatio,
                             0, GL_RGB, GL_FLOAT, nullptr);

    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mp_context->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    mp_context->glBindRenderbuffer(GL_RENDERBUFFER, data.depthRenderBuffer);
    mp_context->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                      m_width * m_devicePixelRatio,
                                      m_height * m_devicePixelRatio);
    mp_context->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                          GL_RENDERBUFFER, data.depthRenderBuffer);

    mp_context->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                     data.outputTexture, 0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    mp_context->glDrawBuffers(1, drawBuffers);
}

void FrameBuffer::create() {
    createFrameBufferData(m_currentFrame);

    for(int i = 0; i < NUM_HISTORY_FRAMES; ++i) {
        FrameBufferData data{0,0,0};
        createFrameBufferData(data);
        m_frameHistory.push(data);
    }

    m_created = true;

    if(mp_context->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        m_created = false;
        std::cout << "Frame buffer did not initialize correctly..." << std::endl;
        mp_context->printGLErrorLog();
    }
}

void FrameBuffer::destroyFrameBufferData(FrameBufferData& data) {
    if(data.frameBuffer != 0) {
        mp_context->glDeleteFramebuffers(1, &data.frameBuffer);
        mp_context->glDeleteTextures(1, &data.outputTexture);
        mp_context->glDeleteRenderbuffers(1, &data.depthRenderBuffer);
        data = {0, 0, 0};
    }
}

void FrameBuffer::destroy() {
    if(m_created) {
        destroyFrameBufferData(m_currentFrame);

        while(!m_frameHistory.empty()) {
            destroyFrameBufferData(m_frameHistory.front());
            m_frameHistory.pop();
        }

        m_created = false;
    }
}

void FrameBuffer::resize(unsigned int width, unsigned int height, unsigned int devicePixelRatio) {
    m_width = width;
    m_height = height;
    m_devicePixelRatio = devicePixelRatio;

    if(m_created) {
        destroy();
        create();
    }
}

void FrameBuffer::bindFrameBuffer() {
    mp_context->glBindFramebuffer(GL_FRAMEBUFFER, m_currentFrame.frameBuffer);
}

void FrameBuffer::bindToTextureSlot(unsigned int slot) {
    m_textureSlot = slot;
    mp_context->glActiveTexture(GL_TEXTURE0 + slot);
    mp_context->glBindTexture(GL_TEXTURE_2D, m_currentFrame.outputTexture);
}

void FrameBuffer::bindHistoryTextures(unsigned int startSlot) {
    int slot = startSlot;
    std::queue<FrameBufferData> tempQueue = m_frameHistory;

    while(!tempQueue.empty()) {
        mp_context->glActiveTexture(GL_TEXTURE0 + slot);
        mp_context->glBindTexture(GL_TEXTURE_2D, tempQueue.front().outputTexture);
        tempQueue.pop();
        slot++;
    }
}

void FrameBuffer::cycleFrameBuffers() {
    if (m_frameHistory.size() >= NUM_HISTORY_FRAMES) {
        destroyFrameBufferData(m_frameHistory.front());
        m_frameHistory.pop();
    }

    FrameBufferData oldCurrent = m_currentFrame;
    createFrameBufferData(m_currentFrame);

    m_frameHistory.push(oldCurrent);
}

unsigned int FrameBuffer::getTextureSlot() const {
    return m_textureSlot;
}
