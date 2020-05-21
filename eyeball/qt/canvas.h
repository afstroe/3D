#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <qopenglwidget.h>

class Canvas : public QOpenGLWidget
{
  Q_OBJECT

public:
  Canvas(QWidget* parent);

protected:
  void initializeGL();

  void paintGL();

  void resizeGL(int w, int h) override;

protected:
};

#endif // !__CANVAS_H__
