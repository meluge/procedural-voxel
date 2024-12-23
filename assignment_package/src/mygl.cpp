#include "mygl.h"
#include <glm_includes.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include "inventory.h"

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
    m_worldAxes(this),
    m_crosshair(this),
    m_progLambert(this),
    m_progFlat(this),
    m_progInstanced(this),
    m_progCross(this),
    m_progPostProcess(this),
    m_progInventory(this),
    m_progRain(this),
    m_progSnow(this),
    m_progTornado(this),
    m_progSky(this),
    postProcessFrameBuffer(this, this->width(), this->height(), this->devicePixelRatio()),
    m_terrain(this),
    m_player(glm::vec3(890.f, 230.f, 1000.f), m_terrain),
    quadDrawable(this),
    sky(this),
    lightDir(glm::vec4(0.5, 1, 0.75,0)),
    m_inventoryOpen(false),
    m_rainSystem(this),
    isRaining(true),
    m_snowSystem(this),
    isSnowing(true),
    m_selectedBlock(GRASS),
    invRect(this),
    walk(this),
    m_tornadoSystem(this, &m_terrain),
    m_tornadoCenter(1000.f, 30.f, 1000.f),
    m_craftingGrid(9, EMPTY),
    m_craftingResult(EMPTY),
    m_selectedSlot(0),
    m_motionBlurEnabled(false),
    m_isSpherical(false),
    invCube(this)
{

    walk.setSource(QUrl::fromLocalFile(":/sound/walk.wav"));
    walk.setLoopCount(QSoundEffect::Infinite);
    walk.setVolume(1.f);

    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible

    initializeCraftingRecipes();



}

MyGL::~MyGL() {
    makeCurrent();
    postProcessFrameBuffer.destroy();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {

    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    glGenVertexArrays(1, &vao);

    m_progLambert.addUniform("u_IsSpherical");

    m_progPostProcess.useMe();
    m_progPostProcess.addAttrib("vs_Pos");
    m_progPostProcess.addAttrib("vs_UV");
    m_progPostProcess.addUniform("u_RenderedTexture");
    m_progPostProcess.addUniform("u_InWater");
    m_progPostProcess.addUniform("u_InLava");

    m_progPostProcess.addUniform("u_History1");
    m_progPostProcess.addUniform("u_History2");
    m_progPostProcess.addUniform("u_History3");
    m_progPostProcess.addUniform("u_History4");
    m_progPostProcess.addUniform("u_MotionBlurEnabled");

    m_progRain.addAttrib("vs_Pos");
    m_progRain.addAttrib("vs_ID");
    m_progRain.addUniform("u_Model");
    m_progRain.addUniform("u_ViewProj");
    m_progRain.addUniform("u_Time");

    m_progSnow.addAttrib("vs_Pos");
    m_progSnow.addAttrib("vs_ID");
    m_progSnow.addUniform("u_Model");
    m_progSnow.addUniform("u_ViewProj");
    m_progSnow.addUniform("u_Time");

    m_progTornado.addAttrib("vs_Pos");
    m_progTornado.addAttrib("vs_ID");
    m_progTornado.addUniform("u_Model");
    m_progTornado.addUniform("u_ViewProj");
    m_progTornado.addUniform("u_Time");
    m_progTornado.addUniform("u_Center");

    //setup transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_progCross.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progPostProcess.create(":/glsl/postprocess.vert.glsl", ":/glsl/postprocess.frag.glsl");
    m_progInventory.create(":/glsl/inventory.vert.glsl", ":/glsl/inventory.frag.glsl");
    m_progRain.create(":/glsl/rain.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progTornado.create(":/glsl/tornado.vert.glsl", ":/glsl/tornado.frag.glsl");
    m_progSky.create(":glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");
    m_progSnow.create(":/glsl/snow.vert.glsl", ":/glsl/flat.frag.glsl");

    postProcessFrameBuffer.resize(width() * this->devicePixelRatio(), height() * this->devicePixelRatio(), 1);
    postProcessFrameBuffer.create();

    quadDrawable.createVBOdata();
    sky.createVBOdata();
    m_rainSystem.setPlayerPosition(glm::vec3(1000.f, 100.f, 1000.f));
    m_snowSystem.setPlayerPosition(glm::vec3(1000.f, 100.f, 1000.f));

    m_tornadoSystem.loadMoonMesh(QString(":/halfmoon/halfmoon.obj"));
    m_tornadoSystem.setCenter(glm::vec3(1000.f, 120.f, 1000.f));
    m_tornadoSystem.createVBOdata();

    // m_crosshair.setScreenDimensions(w, h);
    m_crosshair.createVBOdata();


    //Create texture
    glGenTextures(1, &m_textureHandle);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    QImage img(":/textures/minecraft_textures_all.png");
    if (img.isNull()) {
        qWarning("Failed to load image");
    }
    img.convertTo(QImage::Format_RGBA8888);
    img = img.mirrored();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 img.width(), img.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    invRect.createVBOdata();
    invCube.createVBOdata();

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);


}


void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
    m_crosshair.setScreenDimensions(w, h);

    postProcessFrameBuffer.destroy();
    postProcessFrameBuffer.resize(width() * this->devicePixelRatio(), height() * this->devicePixelRatio(), 1);
    postProcessFrameBuffer.create();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progInstanced.setUnifMat4("u_ViewProj", viewproj);

    m_progInventory.addAttrib("vs_Pos");
    m_progInventory.addAttrib("vs_UV");
    m_progInventory.addUniform("u_Model");
    m_progInventory.addUniform("u_ViewProj");
    m_progInventory.addUniform("u_Texture");
    m_progInventory.addUniform("u_BlockType");
    m_progInventory.addUniform("u_IsSelected");
    m_progInventory.addUniform("u_IsEmpty");
    m_progInventory.addUniform("u_IsCrafting");
    m_progInventory.addUniform("u_Time");
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {

    u_Time++;


    //light direction update
    float angle = 0.01 * u_Time * (2.0 * 3.14159 / 60.0);
    lightDir = glm::normalize(glm::vec4(-1.0,  sin(angle),cos(angle), 0));
    // lightDir = glm::normalize(glm::vec4(0.5, 1, 0.75, 0));

    glm::vec3 playerPos = m_player.mcr_position;

    m_progLambert.setUnifVec3("u_PlayerPos", playerPos);
    m_progTornado.setUnifVec3("u_PlayerPos", playerPos);

    m_terrain.checkForTerrainExpansion(playerPos, m_player.prev_pos);


    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline

    int nextMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    int dt = nextMSecsSinceEpoch - currentMSecsSinceEpoch;
    currentMSecsSinceEpoch = nextMSecsSinceEpoch;


    m_player.tick(dt , m_inputs);

    if (m_terrain.getGlobalBiomeAt(playerPos.x, playerPos.z) == RAINFOREST ) {
        isRaining = true;
    } else {
        isRaining = false;
    }

    if (m_terrain.getGlobalBiomeAt(playerPos.x, playerPos.z) == SNOWY ) {
        isSnowing = true;
    } else {
        isSnowing = false;
    }




    m_rainSystem.setPlayerPosition(playerPos);



    m_snowSystem.setPlayerPosition(playerPos);



    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    m_player.prev_pos = playerPos;

    if (glm::length(m_player.getVelocity()) > 0.01f) {
        if (!walk.isPlaying()) {
            walk.play();
        }
    } else {
        if (walk.isPlaying()) {
            walk.stop();
        }
    }

    float radius = 32.f;
    m_tornadoCenter.x = 1000.f + cos(u_Time * 0.01f) * radius;
    m_tornadoCenter.z = 1000.f + sin(u_Time * 0.01f) * radius;
    m_tornadoSystem.setCenter(m_tornadoCenter);

   // std::cout << "Tornado position: " << m_tornadoCenter.x << ", " << m_tornadoCenter.y << ", " << m_tornadoCenter.z << std::endl;

}



void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so () called at a rate of 60 frames per second.




void MyGL::paintGL() {
    postProcessFrameBuffer.bindFrameBuffer();

    glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);


    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
    m_progLambert.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifVec4("lightDir", lightDir);
    m_progFlat.setUnifMat4("u_ViewProj", viewproj);
    m_progInstanced.setUnifMat4("u_ViewProj", viewproj);
    m_progLambert.setUnifInt("u_IsSpherical", m_isSpherical ? 1 : 0);

    m_progSky.setUnifVec3("R", m_player.getR());
    m_progSky.setUnifVec3("F", m_player.getF());
    m_progSky.setUnifVec3("U", m_player.getU());
    m_progSky.setUnifVec3("eye", m_player.mcr_camera.mcr_position);
    m_progSky.setUnifFloat("aspect", this->width() / this->height());
    m_progSky.setUnifFloat("u_Time", u_Time);
    m_progSky.setUnifVec4("lightDir", lightDir);
    m_progSky.draw(sky);

    renderTerrain();

    if (isRaining) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progRain.useMe();
    m_progRain.setUnifMat4("u_ViewProj", viewproj);
    m_progRain.setUnifMat4("u_Model", glm::mat4(1.f));
    m_progRain.setUnifFloat("u_Time", u_Time);

    m_progRain.drawRain(m_rainSystem);
    }

    glDisable(GL_BLEND);

    if (isSnowing) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progSnow.useMe();
    m_progSnow.setUnifMat4("u_ViewProj", viewproj);
    m_progSnow.setUnifMat4("u_Model", glm::mat4(1.f));
    m_progSnow.setUnifFloat("u_Time", u_Time);

    m_progSnow.drawRain(m_snowSystem);

    glDisable(GL_BLEND);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progTornado.useMe();
    m_progTornado.setUnifMat4("u_ViewProj", m_player.mcr_camera.getViewProj());
    m_progTornado.setUnifMat4("u_Model", glm::mat4(1.f));
    m_progTornado.setUnifFloat("u_Time", u_Time);
    m_progTornado.setUnifVec3("u_Center", m_tornadoCenter);
    m_progTornado.drawRain(m_tornadoSystem);

    glDisable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setUnifMat4("u_Model", glm::mat4());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);

    m_crosshair.updateColor(this);
    glDisable(GL_DEPTH_TEST);
    m_progCross.setUnifMat4("u_ViewProj", glm::mat4(1.f));
    m_progCross.setUnifMat4("u_Model", glm::mat4(1.f));
    m_progCross.draw(m_crosshair);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setUnifMat4("u_Model", glm::mat4());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);

    postProcessFrameBuffer.bindToTextureSlot(1);
    postProcessFrameBuffer.bindHistoryTextures(2); // This will bind history textures to slots 2-5

    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Always bind the current frame
    postProcessFrameBuffer.bindToTextureSlot(1);
    m_progPostProcess.useMe();
    checkPlayerEnvironment();

    // Always bind history textures, even if we're not using them
    postProcessFrameBuffer.bindHistoryTextures(2);
    m_progPostProcess.setUnifInt("u_RenderedTexture", 1);
    m_progPostProcess.setUnifInt("u_History1", 2);
    m_progPostProcess.setUnifInt("u_History2", 3);
    m_progPostProcess.setUnifInt("u_History3", 4);
    m_progPostProcess.setUnifInt("u_History4", 5);

    m_progPostProcess.setUnifInt("u_InWater", isPlayerInWater ? 1 : 0);
    m_progPostProcess.setUnifInt("u_InLava", isPlayerInLava ? 1 : 0);
    m_progPostProcess.setUnifInt("u_Raining", isRaining ? 1 : 0);
    m_progPostProcess.setUnifInt("u_MotionBlurEnabled", m_motionBlurEnabled ? 1 : 0);

    glDisable(GL_DEPTH_TEST);
    m_progPostProcess.draw(quadDrawable);
    glEnable(GL_DEPTH_TEST);

    // Always cycle frame buffers
    postProcessFrameBuffer.cycleFrameBuffers();

    if (m_inventoryOpen) {

        renderInventory();

    }
    glEnable(GL_DEPTH_TEST);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    glm::vec3 playerPos = m_player.mcr_position;
    int chunkRadius = 8;

    int playerChunkX = static_cast<int>(glm::floor(playerPos.x / 16.f)) * 16;
    int playerChunkZ = static_cast<int>(glm::floor(playerPos.z / 16.f)) * 16;

    for (int x = -chunkRadius; x <= chunkRadius; x++) {
        for (int z = -chunkRadius; z <= chunkRadius; z++) {
            int currentX = playerChunkX + (x * 16);
            int currentZ = playerChunkZ + (z * 16);

            if (m_terrain.hasChunkAt(currentX, currentZ)) {
                glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
                m_progLambert.setUnifMat4("u_ViewProj", viewproj);;
                m_progLambert.setUnifMat4("u_Model", glm::mat4(1.f));
                m_progLambert.setUnifMat4("u_ModelInvTr", glm::mat4(1.f));
                m_progLambert.setUnifInt("u_Time", u_Time);
                //setup textures
                m_progLambert.useMe();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_textureHandle);
                m_progLambert.setUnifInt("u_Texture", 0);
                //
                m_terrain.getChunkAt(currentX, currentZ)->bindBuffer(INDEX);
                m_terrain.getChunkAt(currentX, currentZ)->bindBuffer(INTERLEAVED);
                m_progLambert.drawWeaved(*m_terrain.getChunkAt(currentX, currentZ), INDEX, INTERLEAVED);


            }
        }
    }

    for (int x = -chunkRadius; x <= chunkRadius; x++) {
        for (int z = -chunkRadius; z <= chunkRadius; z++) {
            int currentX = playerChunkX + (x * 16);
            int currentZ = playerChunkZ + (z * 16);

            if (m_terrain.hasChunkAt(currentX, currentZ)) {
                glm::mat4 viewproj = m_player.mcr_camera.getViewProj();
                m_progLambert.setUnifMat4("u_ViewProj", viewproj);;
                m_progLambert.setUnifMat4("u_Model", glm::mat4(1.f));
                m_progLambert.setUnifMat4("u_ModelInvTr", glm::mat4(1.f));
                m_progLambert.setUnifInt("u_Time", u_Time);

                //setup textures
                m_progLambert.useMe();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_textureHandle);
                m_progLambert.setUnifInt("u_Texture", 0);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                m_terrain.getChunkAt(currentX, currentZ)->bindBuffer(INDEX_TRANSPARENT);
                m_terrain.getChunkAt(currentX, currentZ)->bindBuffer(INTERLEAVED_TRANSPARENT);

                m_progLambert.drawWeaved(*m_terrain.getChunkAt(currentX, currentZ), INDEX_TRANSPARENT, INTERLEAVED_TRANSPARENT);

                glDisable(GL_BLEND);
            }
        }
    }
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }

    if (m_inventoryOpen) {
        const int TOTAL_SLOTS = 27;

        if (e->key() == Qt::Key_Right) {
            m_selectedSlot = (m_selectedSlot + 1) % TOTAL_SLOTS;
            m_selectedBlock = m_player.getBlockAtSlot(m_selectedSlot);
            return;
        }
        else if (e->key() == Qt::Key_Left) {
            m_selectedSlot = (m_selectedSlot - 1 + TOTAL_SLOTS) % TOTAL_SLOTS;
            m_selectedBlock = m_player.getBlockAtSlot(m_selectedSlot);
            return;
        }
    }

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_I) {
        m_inventoryOpen = !m_inventoryOpen;
        if (m_inventoryOpen) {
            setCursor(Qt::ArrowCursor);

        } else {
            setCursor(Qt::BlankCursor);
            moveMouseToCenter();
        }
    }
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = true;
    } else if (e->key() == Qt::Key_F) {
        m_player.toggleFlightMode();
        moveMouseToCenter();
    } else if (e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = true;
    } else if (e->key() == Qt::Key_M) {
        m_motionBlurEnabled = !m_motionBlurEnabled;
    } else if (e->key() == Qt::Key_P) {
        m_isSpherical = !m_isSpherical;
    } else if (e->key() == Qt::Key_C && m_inventoryOpen && m_selectedBlock != EMPTY) {
        m_player.addBlockToCraftingSlot(m_selectedBlock);
        m_player.removeBlockFromInventoryBlock(m_selectedBlock);
        m_selectedSlot = 0;

        auto craftResult = checkCraftingResult();
        m_craftingResult = craftResult.first;

        if (m_craftingResult != EMPTY) {
            m_player.eraseCrafting();
        }
    } else if (e->key() == Qt::Key_0 && m_inventoryOpen && m_craftingResult != EMPTY) {
        m_player.addBlockToInventorySlot(m_craftingResult);
        m_craftingResult = EMPTY;
    } else if (e->key() == Qt::Key_1 && m_inventoryOpen) {
        m_player.returnCraftingToInventory();
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {

    if (!m_inventoryOpen) {
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = false;
        } else if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = false;
        } else if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = false;
        } else if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = false;
        } else if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = false;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = false;
        } else if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = false;
        }
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    if (m_inventoryOpen) {
        float ndcX = (2.0f * e->x()) / width() - 1.0f;
        float ndcY = 1.0f - (2.0f * e->y()) / height();
    } else {
        glm::vec2 mouseOrigin = glm::vec2(width() / 2, height() / 2);
        glm::vec2 mouseCoords = glm::vec2(e->x(), e->y());

        glm::vec2 mouseDif = mouseCoords - mouseOrigin;

        m_inputs.mouseX = 0.0f;
        m_inputs.mouseY = 0.0f;

        if (glm::length(mouseDif) >= 1.0) {

            m_inputs.mouseX = mouseDif.x;
            m_inputs.mouseY = mouseDif.y;

            moveMouseToCenter();
        }
    }
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    std::map<int, std::pair<BlockType, int>> inv = m_player.getInventory();
    std::map<int, std::pair<BlockType, int>> invC = m_player.getCraftingInventory();

    if (m_inventoryOpen) {
        float ndcX = (2.0f * e->x()) / width() - 1.0f;
        float ndcY = 1.0f - (2.0f * e->y()) / height();
        return;
    }

    if (e->button() == Qt::LeftButton) {
        Collision c = m_player.getLookCollision(5.0f);
        if (c.happened && c.block != BEDROCK) {
            glm::vec3 breakPosition = c.position;

            m_player.addBlockToInventorySlot(c.block);

            m_terrain.setGlobalBlockAt(floor(breakPosition.x), floor(breakPosition.y), floor(breakPosition.z), EMPTY);
            m_terrain.getChunkAt(breakPosition.x, breakPosition.z)->createVBOdata();
            m_terrain.getChunkAt(breakPosition.x, breakPosition.z)->bufferVBO();
        }
    } else if (e->button() == Qt::RightButton) {
    Collision c = m_player.getLookCollision(5.0f);
        if (c.happened) {
            glm::vec3 placePosition = c.position + c.normal;
            if (m_player.getBlockAtSlot(m_selectedSlot) != EMPTY) {
                m_terrain.setGlobalBlockAt(floor(placePosition.x), floor(placePosition.y), floor(placePosition.z), m_selectedBlock);
                m_terrain.getChunkAt(placePosition.x, placePosition.z)->createVBOdata();
                m_terrain.getChunkAt(placePosition.x, placePosition.z)->bufferVBO();
                m_player.removeBlockFromInventorySlot(m_selectedBlock);
            }
        }
    }
}

void MyGL::checkPlayerEnvironment() {
    glm::vec3 playerPos = m_player.mcr_position;
    int chunkX = static_cast<int>(glm::floor(playerPos.x / 16.f)) * 16;
    int chunkZ = static_cast<int>(glm::floor(playerPos.z / 16.f)) * 16;

    if (!m_terrain.hasChunkAt(chunkX, chunkZ)) {
        isPlayerInWater = false;
        isPlayerInLava = false;
        return;
    }
    BlockType blockAtPlayer = m_terrain.getGlobalBlockAt(floor(playerPos.x), floor(playerPos.y), floor(playerPos.z));
    isPlayerInWater = (blockAtPlayer == WATER);
    isPlayerInLava = (blockAtPlayer == LAVA);
}

void MyGL::renderInventory() {
    if (!m_inventoryOpen) return;
    glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());

    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progInventory.useMe();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    m_progInventory.setUnifInt("u_Texture", 0);
    m_progInventory.setUnifFloat("u_Time", u_Time);

    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    m_progInventory.setUnifMat4("u_ViewProj", proj);

    float slotSize = 0.11f;
    float padding = 0.11f;
    float startX = -0.8f;
    float startY = -0.93f;

    const int SLOTS_PER_ROW = 9;

    const float craftingStartX = -0.5f;
    const float craftingStartY = 0.3f;

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            float posX = craftingStartX + (x * (slotSize + padding));
            float posY = craftingStartY - (y * (slotSize + padding));

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, 0.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

            std::vector<glm::vec2> emptyUVs = {
                glm::vec2(0.f, 0.f),
                glm::vec2(1.f, 0.f),
                glm::vec2(1.f, 1.f),
                glm::vec2(0.f, 1.f)
            };
            invRect.updateUVs(emptyUVs);
            m_progInventory.setUnifMat4("u_Model", model);
            m_progInventory.setUnifInt("u_IsSelected", 0);
            m_progInventory.setUnifInt("u_IsEmpty", 1);
            m_progInventory.setUnifInt("u_IsCrafting", 1);
            m_progInventory.draw(invRect);

            int slotIndex = y * 3 + x;
            BlockType blockType = EMPTY;
            std::map<int, std::pair<BlockType, int>> invC = m_player.getCraftingInventory();

            if (invC.count(slotIndex) > 0) {
                blockType = invC[slotIndex].first;
            }

            if (blockType != EMPTY) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, 0.0f)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

                m_progInventory.setUnifMat4("u_Model", model);

                glm::vec2 frontTexLoc = getBlockFaceTextureLocation(blockType, ZPOS);
                glm::vec2 topTexLoc = getBlockFaceTextureLocation(blockType, YPOS);
                glm::vec2 rightTexLoc = getBlockFaceTextureLocation(blockType, XPOS);

                float uvSize = 1.0f / 16.0f;

                std::vector<glm::vec2> frontUVs = {
                    glm::vec2(frontTexLoc.x * uvSize, frontTexLoc.y * uvSize),
                    glm::vec2((frontTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
                    glm::vec2((frontTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
                    glm::vec2(frontTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize)
                };

                std::vector<glm::vec2> topUVs = {
                    glm::vec2(topTexLoc.x * uvSize, (topTexLoc.y + 1) * uvSize),
                    glm::vec2((topTexLoc.x + 1) * uvSize, (topTexLoc.y + 1) * uvSize),
                    glm::vec2((topTexLoc.x + 1) * uvSize, topTexLoc.y * uvSize),
                    glm::vec2(topTexLoc.x * uvSize, topTexLoc.y * uvSize)
                };

                std::vector<glm::vec2> rightUVs = {
                    glm::vec2((rightTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
                    glm::vec2((rightTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
                    glm::vec2(rightTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize),
                    glm::vec2(rightTexLoc.x * uvSize, frontTexLoc.y * uvSize)
                };

                invCube.updateUVs(frontUVs, topUVs, rightUVs);
                m_progInventory.setUnifInt("u_IsEmpty", 0);
                m_progInventory.setUnifInt("u_IsCrafting", 1);
                m_progInventory.draw(invCube);

            }
        }
    }

    float outputX = craftingStartX + (4 * (slotSize + padding));
    float outputY = craftingStartY - (slotSize + padding);

    glm::mat4 outputModel = glm::translate(glm::mat4(1.0f), glm::vec3(outputX, outputY, 0.0f)) *
                            glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

    m_progInventory.setUnifMat4("u_Model", outputModel);
    m_progInventory.setUnifInt("u_IsEmpty", 1);
    m_progInventory.setUnifInt("u_IsCrafting", 1);
    invRect.updateUVs(std::vector<glm::vec2>{
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f)
    });
    m_progInventory.draw(invRect);

    if (m_craftingResult != EMPTY) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(outputX, outputY, 0.0f)) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                          glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

         m_progInventory.setUnifMat4("u_Model", model);

        glm::vec2 frontTexLoc = getBlockFaceTextureLocation(m_craftingResult, ZPOS);
        glm::vec2 topTexLoc = getBlockFaceTextureLocation(m_craftingResult, YPOS);
        glm::vec2 rightTexLoc = getBlockFaceTextureLocation(m_craftingResult, XPOS);

        float uvSize = 1.0f / 16.0f;

        std::vector<glm::vec2> frontUVs = {
            glm::vec2(frontTexLoc.x * uvSize, frontTexLoc.y * uvSize),
            glm::vec2((frontTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
            glm::vec2((frontTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
            glm::vec2(frontTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize)
        };

        std::vector<glm::vec2> topUVs = {
            glm::vec2(topTexLoc.x * uvSize, (topTexLoc.y + 1) * uvSize),
            glm::vec2((topTexLoc.x + 1) * uvSize, (topTexLoc.y + 1) * uvSize),
            glm::vec2((topTexLoc.x + 1) * uvSize, topTexLoc.y * uvSize),
            glm::vec2(topTexLoc.x * uvSize, topTexLoc.y * uvSize)
        };

        std::vector<glm::vec2> rightUVs = {
            glm::vec2((rightTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
            glm::vec2((rightTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
            glm::vec2(rightTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize),
            glm::vec2(rightTexLoc.x * uvSize, frontTexLoc.y * uvSize)
        };

        invCube.updateUVs(frontUVs, topUVs, rightUVs);
        m_progInventory.setUnifInt("u_IsEmpty", 0);
        m_progInventory.setUnifInt("u_IsCrafting", 1);
        m_progInventory.draw(invCube);
    }

    float inventoryStartY = startY + padding;
    const auto& inventory = m_player.getInventory();
    auto it = inventory.cbegin();

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < SLOTS_PER_ROW; x++) {
            float posX = startX + (x * (slotSize + padding));
            float posY = inventoryStartY + (y * (slotSize + padding));

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, 0.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

            invRect.updateUVs({
                glm::vec2(0.f, 0.f),
                glm::vec2(1.f, 0.f),
                glm::vec2(1.f, 1.f),
                glm::vec2(0.f, 1.f)
            });
            m_progInventory.setUnifMat4("u_Model", model);
            m_progInventory.setUnifInt("u_IsEmpty", 1);
            m_progInventory.setUnifInt("u_IsSelected", 0);
            m_progInventory.draw(invRect);

            if (it != inventory.cend() && it->second.first != EMPTY) {

                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, 0.0f)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(slotSize));

                m_progInventory.setUnifMat4("u_Model", model);

                glm::vec2 frontTexLoc = getBlockFaceTextureLocation(it->second.first, ZPOS);
                glm::vec2 topTexLoc = getBlockFaceTextureLocation(it->second.first, YPOS);
                glm::vec2 rightTexLoc = getBlockFaceTextureLocation(it->second.first, XPOS);

                float uvSize = 1.0f / 16.0f;

                std::vector<glm::vec2> frontUVs = {
                   glm::vec2(frontTexLoc.x * uvSize, frontTexLoc.y * uvSize),
                   glm::vec2((frontTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
                   glm::vec2((frontTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
                   glm::vec2(frontTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize)
                };

                std::vector<glm::vec2> topUVs = {
                    glm::vec2(topTexLoc.x * uvSize, (topTexLoc.y + 1) * uvSize),
                    glm::vec2((topTexLoc.x + 1) * uvSize, (topTexLoc.y + 1) * uvSize),
                    glm::vec2((topTexLoc.x + 1) * uvSize, topTexLoc.y * uvSize),
                    glm::vec2(topTexLoc.x * uvSize, topTexLoc.y * uvSize)
                };

                std::vector<glm::vec2> rightUVs = {
                   glm::vec2((rightTexLoc.x + 1) * uvSize, frontTexLoc.y * uvSize),
                   glm::vec2((rightTexLoc.x + 1) * uvSize, (frontTexLoc.y + 1) * uvSize),
                   glm::vec2(rightTexLoc.x * uvSize, (frontTexLoc.y + 1) * uvSize),
                   glm::vec2(rightTexLoc.x * uvSize, frontTexLoc.y * uvSize)
                };

                invCube.updateUVs(frontUVs, topUVs, rightUVs);
                m_progInventory.setUnifInt("u_IsEmpty", 0);

                bool isSelected = (it->second.first == m_selectedBlock);
                m_progInventory.setUnifInt("u_IsSelected", isSelected ? 1 : 0);

                m_progInventory.draw(invCube);
            }
                if (it != inventory.cend()) ++it;
            }

        }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

glm::vec2 MyGL::getBlockFaceTextureLocation(BlockType t, Direction dir) {
    glm::vec2 location;
    switch (t) {
    case GRASS:
        switch (dir) {
        case YPOS:
            location = glm::vec2(8, 13);
            break;
        case YNEG:
            location = glm::vec2(2, 15);
            break;
        default:
            location = glm::vec2(3, 15);
            break;
        }
        break;
    case STONE:
        location = glm::vec2(1, 15);
        break;
    case DIRT:
        location = glm::vec2(2, 15);
        break;
    case SNOW:
        location = glm::vec2(2, 11);
        break;
    case BEDROCK:
        location = glm::vec2(1, 14);
        break;
    case WATER:
        location = glm::vec2(13, 3);
        break;
    case LAVA:
        location = glm::vec2(13, 1);
        break;
    case SAND:
        location = glm::vec2(2, 14);
        break;
    case GRAVEL:
        location = glm::vec2(3, 14);
        break;
    case GRASSPLANT:
        location = glm::vec2(7, 13);
        break;
    case POPPY:
        location = glm::vec2(12, 15);
        break;
    case DANDELION:
        location = glm::vec2(13, 15);
        break;
    case CACTUS:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 11);
            break;
        case YNEG:
            location = glm::vec2(5, 11);
            break;
        default:
            location = glm::vec2(6, 11);
            break;
        }
        break;
    case DEADBUSH:
        location = glm::vec2(7, 12);
        break;
    case OAKLOG:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 14);
            break;
        case YNEG:
            location = glm::vec2(5, 14);
            break;
        default:
            location = glm::vec2(4, 14);
            break;
        }
        break;
    case OAKLEAVES:
        location = glm::vec2(5, 12);
        break;
    case OAKPLANKS:
        location = glm::vec2(4, 15);
        break;
    case ICE:
        location = glm::vec2(3, 11);
        break;
    case CORAL:
        location = glm::vec2(4, 1);
        break;
    case MELON:
        switch (dir) {
        case YPOS:
            location = glm::vec2(9, 7);
            break;
        case YNEG:
            location = glm::vec2(9, 7);
            break;
        default:
            location = glm::vec2(8, 7);
            break;
        }
        break;
    case REDMUSHROOM:
        location = glm::vec2(12, 14);
        break;
    case BROWNMUSHROOM:
        location = glm::vec2(13, 14);
        break;
    case SANDSTONE:
        switch (dir) {
        case YPOS:
            location = glm::vec2(0, 3);
            break;
        case YNEG:
            location = glm::vec2(0, 2);
            break;
        default:
            location = glm::vec2(0, 4);
            break;
        }
        break;
    case WOOL:
        location = glm::vec2(0, 11);
        break;
    case JUNGLELOG:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 14);
            break;
        case YNEG:
            location = glm::vec2(5, 14);
            break;
        default:
            location = glm::vec2(9, 6);
            break;
        }
        break;
    case JUNGLELEAVES:
        location = glm::vec2(5, 12);
        break;
    default:
        location = glm::vec2(0, 0);
        break;
    }
    return location;
}


void MyGL::initializeCraftingRecipes() {
    m_craftingRecipes = {
        {
            {DIRT, STONE, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            SAND,
            1
        },

        {
            {GRASS, DIRT, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            GRASSPLANT,
            1
        },

        {
            {DIRT, DIRT, DIRT,
             DIRT, DIRT, DIRT,
             DIRT, DIRT, DIRT},
            GRAVEL,
            1
        },


        {
            {OAKLEAVES, OAKLEAVES, OAKLEAVES,
             DIRT, DIRT, EMPTY,
             EMPTY, EMPTY, EMPTY},
            DANDELION,
            4
        },

        {
            {OAKLOG, OAKLOG, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            OAKPLANKS,
            4
        },

        {
            {JUNGLELOG, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            OAKPLANKS,
            4
        },

        {
            {SAND, SAND, SAND,
             SAND, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            SANDSTONE,
            1
        },

        {
            {SNOW, SNOW, SNOW,
             SNOW, SNOW, EMPTY,
             EMPTY, EMPTY, EMPTY},
            ICE,
            1
        },

        {
            {OAKLOG, SAND, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            DEADBUSH,
            1
        },

        {
            {JUNGLELEAVES, JUNGLELEAVES, JUNGLELEAVES,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            MELON,
            1
        },

        {
            {GRASS, GRASS, GRASS,
             GRASS, GRASS, EMPTY,
             EMPTY, EMPTY, EMPTY},
            WOOL,
            1
        },

        {
            {REDMUSHROOM, BROWNMUSHROOM, EMPTY,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            GRASS,
            1
        },

        {
            {SAND, SAND, SAND,
             EMPTY, EMPTY, EMPTY,
             EMPTY, EMPTY, EMPTY},
            CACTUS,
            1
        },

    };
}

void MyGL::checkCrafting() {
    m_craftingResult = EMPTY;

    for (const auto& recipe : m_craftingRecipes) {
        bool matches = true;
        for (size_t i = 0; i < 9; i++) {
            if (recipe.pattern[i] != m_craftingGrid[i]) {
                matches = false;
                break;
            }
        }

        if (matches) {
            m_craftingResult = recipe.result;
            return;
        }

        if (recipe.pattern[2] == EMPTY && recipe.pattern[5] == EMPTY &&
            recipe.pattern[6] == EMPTY && recipe.pattern[7] == EMPTY &&
            recipe.pattern[8] == EMPTY) {

            for (int startRow = 0; startRow <= 1; startRow++) {
                for (int startCol = 0; startCol <= 1; startCol++) {
                    matches = true;
                    for (int y = 0; y < 2; y++) {
                        for (int x = 0; x < 2; x++) {
                            int gridIndex = (startRow + y) * 3 + (startCol + x);
                            int recipeIndex = y * 3 + x;
                            if (recipe.pattern[recipeIndex] != m_craftingGrid[gridIndex]) {
                                matches = false;
                                break;
                            }
                        }
                        if (!matches) break;
                    }
                    if (matches) {
                        m_craftingResult = recipe.result;
                        return;
                    }
                }
            }
        }
    }
}


std::pair<BlockType, int> MyGL::checkCraftingResult() {
    std::map<int, std::pair<BlockType, int>> invC = m_player.getCraftingInventory();

    std::vector<BlockType> currentPattern(9, EMPTY);
    for(const auto& slot : invC) {
        if(slot.first < 9) {
            currentPattern[slot.first] = slot.second.first;
        }
    }

    for(const auto& recipe : m_craftingRecipes) {
        if(currentPattern == recipe.pattern) {
            return {recipe.result, 1};
        }
    }

    return {EMPTY, 0};
}
