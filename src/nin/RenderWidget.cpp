#include "RenderWidget.h"
#include "Emulator.h"

RenderWidget::RenderWidget(Emulator& emu, QWidget* parent)
: QOpenGLWidget(parent)
, _texture(0)
, _emu(emu)
{

}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::initializeGL()
{
    static const char black[4] = { 0x00, 0x00, 0x00, 0x00 };

    makeCurrent();
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, black);
}

void RenderWidget::paintGL()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(-1.f, -1.f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(-1.f, 1.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(1.f, 1.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(1.f, -1.f);
    glEnd();
}

void RenderWidget::resizeGL(int w, int h)
{
    glViewport(-1, -1, 1, 1);
}

void RenderWidget::updateTexture(const char* texture)
{
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 240, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
    QOpenGLWidget::update();
}