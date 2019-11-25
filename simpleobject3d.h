#ifndef SIMPLEOBJECT3D_H
#define SIMPLEOBJECT3D_H

#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector2D>
#include "transformational.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

struct VertexData //Структура с информацией о вершинах
{
    VertexData() //Конструктор по умолчанию не принимающий аргументы
    {
    }
    VertexData(QVector3D p, QVector2D t, QVector3D n):
        position(p), textCoord(t), normal(n)
    {//Конструктор с списком инициализации
    }
    QVector3D position; //позиция
    QVector2D textCoord; //текстурные координаты
    QVector3D normal; //нормали
};

class simpleobject3d : public Transformational
{
public:
    simpleobject3d();
    simpleobject3d(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture);
    ~simpleobject3d();//деструктор
    void init(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture);
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions);
    void rotate(const QQuaternion &r);
    void translate(const QVector3D  &t);
    void scale(const float &s);
    void setGlobalTransform(const QMatrix4x4 &g);


private:
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLTexture *m_texture;

    QQuaternion m_rotate;
    QVector3D m_translate;
    float m_scale;
    QMatrix4x4 m_globalTransform;
};

#endif // SIMPLEOBJECT3D_H
