// implementation: Alin Stroe
// mail to: afstroe@gmail.com
// linkedin: https://www.linkedin.com/in/alinstroe/
// purpose: a canvas widget
// Copyright © 2020 Alin Stroe, all rights reserved

#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <qopenglwidget.h>
#include <thread>
#include <future>

class Canvas : public QOpenGLWidget
{
  Q_OBJECT

public:
  Canvas(QWidget* parent);

protected:
  void initializeGL();

  void paintGL();

  void resizeGL(int w, int h) override;

  void closeEvent(QCloseEvent* event) override;

protected:
  std::thread paintTrigger;
  std::promise<void> exitSignal;
};

#endif // !__CANVAS_H__
