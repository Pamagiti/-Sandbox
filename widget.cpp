#include "widget.h"
#include <simpleobject3d.h>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QtMath>
#include "group3d.h"
#include "camera3d.h"
#include <QKeyEvent>
#include <QImage>
#include "skybox.h"

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{

    m_camera = new Camera3D;
    m_camera->translate(QVector3D(0.0f, 0.0f, -5.0f));

    m_createState = Idle;

    this->setWindowTitle("Smoothed of neighbors");

}

Widget::~Widget()
{
    delete m_camera;

    for (int i = 0; i < m_objects.size(); ++i)
        delete m_objects[i];

    for (int i = 0; i < m_groups.size(); ++i)
        delete m_groups[i];

    for (int i = 0; i < m_TransformObjects.size(); ++i)
        delete m_TransformObjects[i];

}

void Widget::initializeGL()//инициализация буферов
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//первые три параметра это rgb
                                         //последнний - альфа канал 1 - непрозрачный
    glEnable(GL_DEPTH_TEST);//включение буфера глубины
    glEnable(GL_CULL_FACE);//включение отсечение задних граней

    initShaders();

    float step = 2.0f;

    m_groups.append(new Group3D);
    for(float x = -step; x <= step; x += step){
        for(float y = -step; y <= step; y += step){
             for(float z = -step; z <= step; z += step){
                 initCube(1.0f);
                 m_objects[m_objects.size() - 1]->translate(QVector3D(x, y, z));
                 m_groups[m_groups.size() - 1]->addObeject(m_objects[m_objects.size() - 1]);
             }
        }
    }
    m_groups[0]->translate(QVector3D(-8.0f, 0.0f, 0.0f));

    m_groups.append(new Group3D);
    for(float x = -step; x <= step; x += step){
        for(float y = -step; y <= step; y += step){
             for(float z = -step; z <= step; z += step){
                 initCube(1.0f);
                 m_objects[m_objects.size() - 1]->translate(QVector3D(x, y, z));
                 m_groups[m_groups.size() - 1]->addObeject(m_objects[m_objects.size() - 1]);
             }
        }
    }
    m_groups[1]->translate(QVector3D(8.0f, 0.0f, 0.0f));

    m_groups.append(new Group3D);
    m_groups[2]->addObeject(m_groups[0]);
    m_groups[2]->addObeject(m_groups[1]);

    m_TransformObjects.append(m_groups[2]);

    loadObj(":/cube.obj");
    m_TransformObjects.append(m_objects[m_objects.size() - 1]);

    m_groups[0]->addObeject(m_camera);

    m_skybox = new SkyBox(100, QImage(":/skyboxx.jpg"));

    //m_timer.start(30, this);
}

void Widget::resizeGL(int w, int h)//изменение размера
{
    float aspect = w / (h ? (float)h : 1);

    m_projectionMatrix.setToIdentity();//Создание еденичной матрицы
    m_projectionMatrix.perspective(45, aspect, 0.01f, 1000.0f);//Создание матрицы
               //перспективы      угол наш аспект передняя и задняя плоскость
}

void Widget::paintGL()//рисование
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//очистка буфера цвета и глубины

    m_programSkybox.bind();//бинд шейдера для использвания
    m_programSkybox.setUniformValue("u_projectionMatrix", m_projectionMatrix);//заполнение по формуле

    m_camera->draw(&m_programSkybox);
    m_skybox->draw(&m_programSkybox, context()->functions());
    m_programSkybox.release();

    m_program.bind();//бинд шейдера для использвания
    m_program.setUniformValue("u_projectionMatrix", m_projectionMatrix);//заполнение по формуле
    //m_program.setUniformValue("u_viewMatrix", viewMatrix);
    m_program.setUniformValue("u_lightPosition", QVector4D(0.0, 0.0, 0.0, 1.0));//Свет там же где и наблюдатель(последнее 1 тк вершина)
    m_program.setUniformValue("u_lightPower", 1.0f);

    m_camera->draw(&m_program);
    for (int i = 0; i < m_TransformObjects.size(); i++) {
        m_TransformObjects[i]->draw(&m_program, context()->functions());
    }
    m_program.release();
}

void Widget::mousePressEvent(QMouseEvent *event)//НАЖАТИЕ НА КНОПКУ
{
    if (event->buttons() == Qt::LeftButton){//только на левую кнопку мыши
        m_mousePosition = QVector2D(event->localPos());//Локальная позиция данного окна
    } else if (event->buttons() == Qt::RightButton){
        QVector3D t = screenCoordsToWorldCoords(QVector2D(event->localPos()));
        initCube(2);
        m_objects[m_objects.size() - 1]->scale(0.0f);
        m_objects[m_objects.size() - 1]->translate(t);
        m_TransformObjects.append(m_objects[m_objects.size() - 1]);

        update();
    }
    event->accept();//Подтверждение срабатывания
}

void Widget::mouseMoveEvent(QMouseEvent *event)//ПЕРЕДВИЖЕНИЕ КНОПКИ
{
    if (event->buttons() != Qt::LeftButton) return;

    QVector2D diff = QVector2D(event->localPos()) - m_mousePosition;//Просчет разницы между текущим положением и старым(направление)
    m_mousePosition = QVector2D(event->localPos());//Текущее положение

    float angle = diff.length() / 2.0; //Угол поворота

    QVector3D axis = QVector3D(diff.y(), diff.x(), 0.0); //Нормаль к вектору направления

    //Итоговый поворот
    m_camera->rotate(QQuaternion::fromAxisAndAngle(axis, angle));

//    float angleX = diff.y() / 2.0f;
//    float angleY = diff.x() / 2.0f;

//    m_camera->rotateX(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, angleX));
//    m_camera->rotateY(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angleY));

    update();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0){
        m_camera->translate(QVector3D(0.0f, 0.0f, 0.25f));
    } else if (event->delta() < 0){
        m_camera->translate(QVector3D(0.0f, 0.0f, -0.25f));
    }

    update();
}

void Widget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    for(int i = 0; i < m_objects.size() - 1; ++i){
        if (i % 2 == 0){
            m_objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qSin(angleObects)));
            m_objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, qCos(angleObects)));
        } else {
            m_objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, qSin(angleObects)));
            m_objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qCos(angleObects)));
        }
    }

    m_groups[0]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, qSin(angleGroup1)));
    m_groups[0]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -qSin(angleGroup1)));

    m_groups[1]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qCos(angleGroup1)));
    m_groups[1]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -qCos(angleGroup1)));

    m_groups[2]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qSin(angleMain)));
    m_groups[2]->rotate(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, qCos(angleMain)));

    angleObects += M_PI / 180.0f;
    angleGroup1 += M_PI / 320.0f;
    angleGroup2 -= M_PI / 360.0f;
    angleMain += M_PI / 720.0f;

    update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_groups[0]->delObeject(m_camera);
        m_groups[1]->addObeject(m_camera);
        break;
    case Qt::Key_Right:
        m_groups[0]->addObeject(m_camera);
        m_groups[1]->delObeject(m_camera);
        break;
    case Qt::Key_Down:
        m_groups[0]->delObeject(m_camera);
        m_groups[1]->delObeject(m_camera);
        break;
    case Qt::Key_Up:
        m_groups[0]->delObeject(m_camera);
        m_groups[1]->delObeject(m_camera);
        QMatrix4x4 tmp;
        tmp.setToIdentity();
        m_camera->setGlobalTransform(tmp);
    }
    update();
}

void Widget::initShaders()
{
   if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
       close();//Шейдер вершин

   if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh"))
       close();//Шейдер пиксельный

   if (!m_program.link()) //link - объединяет все шейдеры
       close();

   if (!m_programSkybox.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/skybox.vsh"))
       close();//Шейдер вершин

   if (!m_programSkybox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/skybox.fsh"))
       close();//Шейдер пиксельный

   if (!m_programSkybox.link()) //link - объединяет все шейдеры
       close();
}

void Widget::initCube(float width) //Длинна любой грани куба
{
    float width_div_2 = width / 2.0f; //Тк куб центрированный, то используем грань/2
    QVector<VertexData> vertexes; //Вектор

    //Добавление вершин: левая верхняя, левая нижняя, правая верхняя, правая нижняя
    //                                   х             у            z             текстура одинаковая(квадрат)  направлена но z
    // Положительная z
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(0.0, 1.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(0.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(1.0, 1.0), QVector3D(0.0, 0.0, 1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(1.0, 0.0), QVector3D(0.0, 0.0, 1.0)));
    // Положительная х
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(0.0, 1.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(0.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(1.0, 1.0), QVector3D(1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(1.0, 0.0), QVector3D(1.0, 0.0, 0.0)));
    // Положительная у
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2), QVector2D(0.0, 1.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(0.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(1.0, 1.0), QVector3D(0.0, 1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(1.0, 0.0), QVector3D(0.0, 1.0, 0.0)));
    // Отрицательная z
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2), QVector2D(0.0, 1.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(0.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(1.0, 1.0), QVector3D(0.0, 0.0, -1.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(1.0, 0.0), QVector3D(0.0, 0.0, -1.0)));
    // Отрицательная х
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2), QVector2D(0.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2), QVector2D(0.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(1.0, 1.0), QVector3D(-1.0, 0.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(1.0, 0.0), QVector3D(-1.0, 0.0, 0.0)));
    // Отрицательная у
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2), QVector2D(0.0, 1.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2), QVector2D(0.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2), QVector2D(1.0, 1.0), QVector3D(0.0, -1.0, 0.0)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2), QVector2D(1.0, 0.0), QVector3D(0.0, -1.0, 0.0)));
    // заполнение индексов
    QVector<GLuint> indexes;
    for (int i = 0; i < 24; i += 4) {
        indexes.append(i + 0);
        indexes.append(i + 1);
        indexes.append(i + 2);
        indexes.append(i + 2);
        indexes.append(i + 1);
        indexes.append(i + 3);
    }

    m_objects.append(new simpleobject3d(vertexes, indexes, QImage(":/cubbbbbbb.jpg")));
}

void Widget::loadObj(QString path)
{
    QFile objFile(path);
    if (!objFile.exists()){
        qDebug() << "File Nema";
        return;
    }

    QVector<QVector3D> coords;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normal;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;

    objFile.open(QIODevice::ReadOnly);
    QTextStream input(&objFile);
    while (!input.atEnd()) {
        QString str = input.readLine();
        QStringList list = str.split(" ");
        if (list[0] == "#") {
            qDebug() << "This is comment: " << str;
        } else if (list[0] == "mtllib") {
            // Обработать файл с материалами
            qDebug() << "File with material: " << str;
        } else if (list[0] == "v") {
            coords.append(QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
        } else if (list[0] == "vt") {
            texCoords.append(QVector2D(list[1].toFloat(), list[2].toFloat()));
        } else if (list[0] == "vn") {
            normal.append(QVector3D(list[1].toFloat(), list[2].toFloat(), list[3].toFloat()));
        } else if (list[0] == "f") {
            for (int i = 1; i <= 3; ++i){
                QStringList vert = list[i].split("/");
                vertexes.append(VertexData(coords[vert[0].toLong() - 1], texCoords[vert[1].toLong() - 1], normal[vert[2].toLong() - 1]));
                indexes.append(indexes.size());
            }
        }
    }

    objFile.close();

    m_objects.append(new simpleobject3d(vertexes, indexes, QImage(":/cubbbbbbb.jpg")));
}

QVector3D Widget::screenCoordsToWorldCoords(const QVector2D &mousePos)
{
    QVector4D tmp(2.0f * mousePos.x() / width() - 1.0f, -2.0f * mousePos.y() / height() + 1, -1.0f, 1.0f);
    QVector4D iTmp((m_projectionMatrix.inverted() * tmp).toVector2D(), -1.0f, 0.0f);
    QVector3D Direction((m_camera->getViewMatrix().inverted() * iTmp).toVector3D().normalized());

    QVector3D camPos((m_camera->getViewMatrix().inverted() * QVector4D(0.0f, 0.0f, 0.0f, 1.0f)).toVector3D());

    //Ax + By + Cz + D = 0
    //norm(A, B, C)
    //D - расстояние до центра системы координат
    //P * N + d = 0 где d - расстояние d центра d = -P0 * N любая точка плоскоти умножается на нормаль
    //O + D*t
    //в тетрадь смотри ебать

    QVector3D N(0.0f, 1.0f, 0.0f);
    float t = -QVector3D::dotProduct(camPos, N) / QVector3D::dotProduct(Direction, N);
    QVector3D result = camPos + Direction * t;

    return result;
}

