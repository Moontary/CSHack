#include <QApplication>
#include <QImage>

#include <GL/glut.h>
#include <QTime>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include "tiny_obj_loader.h"
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

tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

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
GLuint sdobnikovTextureId, goncharukTextureId;

void initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    quad = gluNewQuadric();

    QImage image = QImage("background.jpg");
    textureId = loadTexture(image);

    QImage rawImage = QImage("raw.jpg");
    rawTextureId = loadTexture(rawImage);

    QImage sdobnikovTexture = QImage("texture.jpg");
    sdobnikovTextureId = loadTexture(sdobnikovTexture);

    QImage goncharukTexture = QImage("texture2.jpg");
    goncharukTextureId = loadTexture(goncharukTexture);
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

        glPushMatrix();

        glTranslatef(0.0f, 0.0f, 0.0f);

        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glRotatef(yAngle, 1.0f, 0.0f, 0.0f);
        glRotatef(zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(xAngle, 0.0f, 0.0f, 1.0f);

        gluQuadricTexture(quad, 1);
        gluSphere(quad, 100, 50, 50);

        glPopMatrix();
    }

    for (auto data : map) {
        auto ww = w / map.size() - 6;

        glViewport(i++ * (w / map.size()) + 3, 0, ww, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(50, (float) w / (float) h, 1.0, 200.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();

        glTranslatef(0.0f, 0.0f, 0.0f);

        mutex.lock();

        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glRotatef(data.yAngle, 1.0f, 0.0f, 0.0f);
        glRotatef(data.zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(-data.xAngle, 0.0f, 0.0f, 1.0f);

        mutex.unlock();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, isRaw ? rawTextureId : textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        gluQuadricTexture(quad, 1);
        gluSphere(quad, 100, 50, 50);
        glPopMatrix();

        size_t face_i = 0;
        for (auto dataFace : map) {
            glPushMatrix();
            glLoadIdentity();

            glRotatef(90, 1.0f, 0.0f, 0.0f);
            glRotatef(data.yAngle, 1.0f, 0.0f, 0.0f);
            glRotatef(data.zAngle, 0.0f, 1.0f, 0.0f);
            glRotatef(-data.xAngle, 0.0f, 0.0f, 1.0f);

            glTranslatef(face_i ? 25 : -25, 0, 0);

            glRotatef(-90, 1.0f, 0.0f, 0.0f);
            glRotatef(90, 0.0f, 1.0f, 0.0f);
            glRotatef(0, 0.0f, 0.0f, 1.0f);

            glRotatef(dataFace.yAngle, 1.0f, 0.0f, 0.0f);
            //glRotatef(data.zAngle, 0.0f, 1.0f, 0.0f);
            glRotatef(dataFace.xAngle + 90, 0.0f, 1.0f, 0.0f);

            float scale = 1.0f / 50;
            glScalef(scale, scale, scale);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, face_i ? goncharukTextureId : sdobnikovTextureId);

            face_i++;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            for (tinyobj::shape_t shape : shapes) {
                int currentIndex = 0;

                for (unsigned char c : shape.mesh.num_face_vertices) {
                    assert(c == 3);

                    glBegin(GL_TRIANGLES);

                    for (size_t i = 0; i < c; i++) {
                        tinyobj::index_t index = shape.mesh.indices[currentIndex + i];

                        int vi = index.vertex_index;
                        int ni = index.normal_index;
                        int ti = index.texcoord_index;

                        glVertex3f(attrib.vertices[3 * vi + 0], attrib.vertices[3 * vi + 1], attrib.vertices[3 * vi + 2]);
                        glNormal3f(attrib.normals[3 * ni + 0], attrib.normals[3 * ni + 1], attrib.normals[3 * ni + 2]);
                        glTexCoord2f(1 - attrib.texcoords[2 * ti + 0], 1 - attrib.texcoords[2 * ti + 1]);
                    }

                    glEnd();

                    currentIndex += c;
                }
            }

            glPopMatrix();
        }
    }

    if (map.empty()) {

        glPushMatrix();
        glLoadIdentity();

        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glRotatef(yAngle, 1.0f, 0.0f, 0.0f);
        glRotatef(zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(xAngle, 0.0f, 0.0f, 1.0f);

        glTranslatef(25, 0, 0);

        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glRotatef(90, 0.0f, 1.0f, 0.0f);
        glRotatef(0, 0.0f, 0.0f, 1.0f);

        glRotatef(yAngle, 1.0f, 0.0f, 0.0f);
        // glRotatef(zAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(xAngle + 90, 0.0f, 1.0f, 0.0f);

        float scale = 1.0f / 50;
        glScalef(scale, scale, scale);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sdobnikovTextureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for (tinyobj::shape_t shape : shapes) {
            int currentIndex = 0;

            for (unsigned char c : shape.mesh.num_face_vertices) {
                assert(c == 3);

                glBegin(GL_TRIANGLES);

                for (size_t i = 0; i < c; i++) {
                    tinyobj::index_t index = shape.mesh.indices[currentIndex + i];

                    int vi = index.vertex_index;
                    int ni = index.normal_index;
                    int ti = index.texcoord_index;

                    glVertex3f(attrib.vertices[3 * vi + 0], attrib.vertices[3 * vi + 1], attrib.vertices[3 * vi + 2]);
                    glNormal3f(attrib.normals[3 * ni + 0], attrib.normals[3 * ni + 1], attrib.normals[3 * ni + 2]);
                    glTexCoord2f(1 - attrib.texcoords[2 * ti + 0], 1 - attrib.texcoords[2 * ti + 1]);
                }

                glEnd();

                currentIndex += c;
            }
        }

        glPopMatrix();
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

void loadMesh() {
    std::string inputfile = "sdobnikov.obj";

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty())
        std::cerr << err << std::endl;

    if (!ret)
        exit(1);
}

int main(int argc, char **argv) {
    std::thread thread(glutLoop, argc, argv);

    QApplication app(argc, argv);

    UDPServer server;

    loadMesh();

    return app.exec();
}
