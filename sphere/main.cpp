#include <QApplication>
#include <QImage>

#include <GL/glut.h>
#include <QTime>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include "udpserver.h"

bool isRaw = false;

void handleKeypress(unsigned char key, int, int) {
    switch (key) {
    case 'w':
        isRaw = !isRaw;
        return;
    case 27:
        exit(0);
    }
}

int lastX, lastY;
bool firstPass = true;
float xAngle, yAngle, zAngle;

void handleMouse(int x, int y) {
    if (!map.empty())
        return;

    static float scale = 2;

    if (!firstPass) {
        xAngle += (x - lastX) / scale;
        yAngle -= (y - lastY) / scale;
    } else
        firstPass = false;

    yAngle = std::max(-90.0f, yAngle);
    yAngle = std::min(90.0f, yAngle);

    lastX = x;
    lastY = y;
}

GLuint loadTexture(const QImage &image) {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    return textureId;
}

GLuint textureId;
GLUquadric *quad;

GLuint rawTextureId;

void initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    quad = gluNewQuadric();

    QImage image = QImage("texture.jpg");
    textureId = loadTexture(image);

    QImage rawImage = QImage("raw.jpg");
    rawTextureId = loadTexture(rawImage);
}

int w, h;

void handleResize(int _w, int _h) {
    w = _w;
    h = _h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, (float) w / (float) h, 1.0, 200.0);
}

void drawScene() {
    int i = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, isRaw ? rawTextureId : textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (map.empty()) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, 0.0f);

        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glRotatef(yAngle, 1.0f, 0.0f, 0.0f);
        glRotatef(zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(xAngle, 0.0f, 0.0f, 1.0f);

        gluQuadricTexture(quad, 1);
        gluSphere(quad, 10, 50, 50);
    }

    for (auto data : map) {
        auto ww = w / map.size() - 6;

        glViewport(i++ * (w / map.size()) + 3, 0, ww, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(50, (float) w / (float) h, 1.0, 200.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, 0.0f);

        mutex.lock();

        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glRotatef(data.yAngle, 1.0f, 0.0f, 0.0f);
        glRotatef(data.zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(-data.xAngle, 0.0f, 0.0f, 1.0f);

        mutex.unlock();

        gluQuadricTexture(quad, 1);
        gluSphere(quad, 10, 50, 50);
    }

    glutSwapBuffers();
}

void update(int) {
    mutex.lock();

    QMutableMapIterator<QString, AngleData> i(map);
    while (i.hasNext()) {
        i.next();

        if (i.value().time.secsTo(QTime::currentTime()) > 2)
            i.remove();
    }

    mutex.unlock();

    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

void glutLoop(int argc, char **argv) {
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutCreateWindow("Sphere");

    initialize();

    glutTimerFunc(25, update, 0);
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutPassiveMotionFunc(handleMouse);
    glutReshapeFunc(handleResize);
    glutMainLoop();
}

int main(int argc, char **argv) {
    std::thread thread(glutLoop, argc, argv);

    QApplication app(argc, argv);

    UDPServer server;

    return app.exec();
}
