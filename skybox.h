#ifndef SKYBOX_H
#define SKYBOX_H

#include "transformational.h"

class simpleobject3d;
class QImage;
class SkyBox : public Transformational
{
public:
    SkyBox(float width, const QImage &texture);
    ~SkyBox();
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D  &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &g);

private:
    simpleobject3d *m_cube;
};

#endif // SKYBOX_H
