#pragma once
#include "openglcontext.h"
#include "glm_includes.h"
#include <queue>

class FrameBuffer {
private:
    OpenGLContext* mp_context;
    static const int NUM_HISTORY_FRAMES = 5;

    struct FrameBufferData {
        GLuint frameBuffer;
        GLuint outputTexture;
        GLuint depthRenderBuffer;
    };

    std::queue<FrameBufferData> m_frameHistory;
    FrameBufferData m_currentFrame;

    unsigned int m_width, m_height, m_devicePixelRatio;
    bool m_created;
    unsigned int m_textureSlot;

    void createFrameBufferData(FrameBufferData& data);
    void destroyFrameBufferData(FrameBufferData& data);

public:
    FrameBuffer(OpenGLContext* context, unsigned int width, unsigned int height, unsigned int devicePixelRatio);
    ~FrameBuffer();

    void resize(unsigned int width, unsigned int height, unsigned int devicePixelRatio);
    void create();
    void destroy();
    void bindFrameBuffer();
    void bindToTextureSlot(unsigned int slot);
    unsigned int getTextureSlot() const;

    void cycleFrameBuffers();
    void bindHistoryTextures(unsigned int startSlot);
};
