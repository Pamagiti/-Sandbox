#include "simpleobject3d.h"
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

simpleobject3d::simpleobject3d():
    m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_texture(0)
{
    m_scale = 1.0f;
}

simpleobject3d::simpleobject3d(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture):
    m_indexBuffer(QOpenGLBuffer::IndexBuffer), m_texture(0)
{
    m_scale = 1.0f;
    init(vertData, indexes, texture);
}

simpleobject3d::~simpleobject3d()
{
    if(m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if(m_indexBuffer.isCreated())
        m_indexBuffer.destroy();
    if(m_texture != 0){
        if(m_texture->isCreated())
            m_texture->destroy();
    }
}

void simpleobject3d::init(const QVector<VertexData> &vertData, const QVector<GLuint> &indexes, const QImage &texture)
{
    if(m_vertexBuffer.isCreated())
        m_vertexBuffer.destroy();
    if(m_indexBuffer.isCreated())
        m_indexBuffer.destroy();
    if(m_texture != 0){
        if(m_texture->isCreated()){
            delete m_texture;
            m_texture = 0;
        }
    }

    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(vertData.constData(), vertData.size() * sizeof (VertexData));
    m_vertexBuffer.release();

    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(indexes.constData(), indexes.size() * sizeof (GLuint));
    m_indexBuffer.release();

    m_texture = new QOpenGLTexture(texture.mirrored());

    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);//?

    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);//?

    m_texture->setWrapMode(QOpenGLTexture::Repeat);//?
}

void simpleobject3d::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    if(!m_vertexBuffer.isCreated() || !m_indexBuffer.isCreated()) return;

    m_texture->bind(0);
    program->setUniformValue("u_texture", 0);

    QMatrix4x4 modalMatrix;
    modalMatrix.setToIdentity();
    //Следующий порядок ОЧЕНЬ ВАЖЕН
    modalMatrix.translate(m_translate);
    modalMatrix.rotate(m_rotate);
    modalMatrix.scale(m_scale);
    modalMatrix = m_globalTransform * modalMatrix;

    program->setUniformValue("u_modalMatrix", modalMatrix);

    m_vertexBuffer.bind();

    int offset = 0;

    int vertLoc = program->attributeLocation("a_position");//передача атрибутов вершин в шейдер
    program->enableAttributeArray(vertLoc);//включаем использование
    program->setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int textLoc = program->attributeLocation("a_textcoord");//передача атрибутов текстурных координат в шейдер
    program->enableAttributeArray(textLoc);//включаем использование
    program->setAttributeBuffer(textLoc, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    int normLoc = program->attributeLocation("a_normal");//передача атрибутов нормалей в шейдер
    program->enableAttributeArray(normLoc);//включаем использование
    program->setAttributeBuffer(normLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

    m_indexBuffer.bind();

    functions->glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);

    m_vertexBuffer.release();
    m_indexBuffer.release();
    m_texture->release();
}

void simpleobject3d::rotate(const QQuaternion &r)
{
    m_rotate = r * m_rotate;
}

void simpleobject3d::translate(const QVector3D &t)
{
    m_translate += t;
}

void simpleobject3d::scale(const float &s)
{
    m_scale *= s;
}

void simpleobject3d::setGlobalTransform(const QMatrix4x4 &g)
{
    m_globalTransform = g;
}
