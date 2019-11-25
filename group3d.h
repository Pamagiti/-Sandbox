#ifndef GROUP3D_H
#define GROUP3D_H

#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector>
#include "transformational.h"

class Group3D : public Transformational
{
public:
    Group3D();

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D  &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &g);

    void addObeject(Transformational *obj);
    void delObeject(Transformational *obj);
    void delObeject(const int &index);

private:
    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransform;

    QVector<Transformational *> m_objects;
};

#endif // GROUP3D_H
