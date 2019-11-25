#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QBasicTimer>

class simpleobject3d;
class Transformational;
class Group3D;
class Camera3D;
class SkyBox;

class Widget : public QOpenGLWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    enum CreateState {
        Idle,
        CreatePlane,
        CreateHeight
    };

    //При наследовании от класса данного, первые 3 метода переопределяются
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void initShaders();
    void initCube(float width);
    void loadObj(QString path);

    QVector3D screenCoordsToWorldCoords(const QVector2D &mousePos);

private:
    QMatrix4x4 m_projectionMatrix;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_programSkybox;
    QVector2D m_mousePosition;

    QVector<simpleobject3d *> m_objects;//класс для описания 3д объектов
    QVector<Group3D *> m_groups;
    QVector<Transformational *> m_TransformObjects;

    QBasicTimer m_timer;

    float angleObects;
    float angleGroup1;
    float angleGroup2;
    float angleMain;

    Camera3D *m_camera;
    SkyBox *m_skybox;

    CreateState m_createState;


};
#endif // WIDGET_H
