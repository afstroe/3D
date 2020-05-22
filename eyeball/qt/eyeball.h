#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_eyeball.h"

class eyeball : public QMainWindow
{
  Q_OBJECT

public:
  eyeball(QWidget *parent = Q_NULLPTR);

  void closeEvent(QCloseEvent* event) override;

protected slots:
  void onActionViewWireframe();
  
  void onActionViewSolidWireframe();

  void onActionViewAuto();
  
  void onActionRenderBothFaces();

private:
    Ui::eyeballClass ui;
};
