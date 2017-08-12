#include <QImage>

#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>

void handleKeypress(unsigned char key, int, int) {
    switch (key) {
    case 27:
        exit(0);
    }
}

int lastX, lastY;
bool firstPass = true;
float xAngle = 0, yAngle = 0;

void handleMouse(int x, int y) {
    if (!firstPass) {
        xAngle += x - lastX;
        yAngle += y - lastY;
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    return textureId;
}

GLuint textureId;
GLUquadric *quad;

void initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    quad = gluNewQuadric();

    QImage image = QImage("texture.jpg");
    textureId = loadTexture(image);
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, (float) w / (float) h, 1.0, 200.0);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glRotatef(yAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(xAngle, 0.0f, 0.0f, 1.0f);

    gluQuadricTexture(quad, 1);
    gluSphere(quad, 10, 50, 50);

    glutSwapBuffers();
}

void update(int) {
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Sphere");

    initialize();

    glutTimerFunc(25, update, 0);
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutPassiveMotionFunc(handleMouse);
    glutReshapeFunc(handleResize);
    glutMainLoop();

    return 0;
}
