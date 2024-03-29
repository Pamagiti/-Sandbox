#include "group3d.h"
#include <QMatrix4x4>

Group3D::Group3D()
{
    m_scale = 1.0f;
}

void Group3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    for(int i = 0; i < m_objects.size(); i++){
        m_objects[i]->draw(program, functions);
    }
}

void Group3D::rotate(const QQuaternion &r)
{
    m_rotate = r * m_rotate;
    QMatrix4x4 localeMatrix;
    localeMatrix.setToIdentity();
    localeMatrix.translate(m_translate);
    localeMatrix.rotate(m_rotate);
    localeMatrix.scale(m_scale);
    localeMatrix = m_globalTransform * localeMatrix;

    for(int i = 0; i < m_objects.size(); i++){
        m_objects[i]->setGlobalTransform(localeMatrix);
    }
}

void Group3D::translate(const QVector3D &t)
{
    m_translate += t;
    QMatrix4x4 localeMatrix;
    localeMatrix.setToIdentity();
    localeMatrix.translate(m_translate);
    localeMatrix.rotate(m_rotate);
    localeMatrix.scale(m_scale);
    localeMatrix = m_globalTransform * localeMatrix;

    for(int i = 0; i < m_objects.size(); i++){
        m_objects[i]->setGlobalTransform(localeMatrix);
    }
}

void Group3D::scale(const float &s)
{
    m_scale *= s;
    QMatrix4x4 localeMatrix;
    localeMatrix.setToIdentity();
    localeMatrix.translate(m_translate);
    localeMatrix.rotate(m_rotate);
    localeMatrix.scale(m_scale);
    localeMatrix = m_globalTransform * localeMatrix;

    for(int i = 0; i < m_objects.size(); i++){
        m_objects[i]->setGlobalTransform(localeMatrix);
    }
}

void Group3D::setGlobalTransform(const QMatrix4x4 &g)
{
    m_globalTransform = g;
    QMatrix4x4 localeMatrix;
    localeMatrix.setToIdentity();
    localeMatrix.translate(m_translate);
    localeMatrix.rotate(m_rotate);
    localeMatrix.scale(m_scale);
    localeMatrix = m_globalTransform * localeMatrix;

    for(int i = 0; i < m_objects.size(); i++){
        m_objects[i]->setGlobalTransform(localeMatrix);
    }
}

void Group3D::addObeject(Transformational *obj)
{
    m_objects.append(obj);
    QMatrix4x4 localeMatrix;
    localeMatrix.setToIdentity();
    localeMatrix.translate(m_translate);
    localeMatrix.rotate(m_rotate);
    localeMatrix.scale(m_scale);
    localeMatrix = m_globalTransform * localeMatrix;
    m_objects[m_objects.size() - 1]->setGlobalTransform(localeMatrix);
}

void Group3D::delObeject(Transformational *obj)
{
    m_objects.removeAll(obj);
}

void Group3D::delObeject(const int &index)
{
    m_objects.remove(index);
}

