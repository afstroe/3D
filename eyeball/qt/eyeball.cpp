#include "eyeball.h"
#include <eyeball/app/appglobalstate.h>
#include <eyeball/graphics/opencv/opencvLink.h>

eyeball::eyeball(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);

  connect(ui.actionViewAuto, SIGNAL(triggered()), SLOT(onActionViewAuto()));
  connect(ui.actionViewWireframe, SIGNAL(triggered()), SLOT(onActionViewWireframe()));
  connect(ui.actionViewSolidWireframe, SIGNAL(triggered()), SLOT(onActionViewSolidWireframe()));
  connect(ui.actionRenderBothFaces, SIGNAL(triggered()), SLOT(onActionRenderBothFaces()));
  connect(ui.actionDrawNormals, SIGNAL(triggered()), SLOT(onActionDrawNormals()));
  connect(ui.actionRenderMonochrome, SIGNAL(triggered()), SLOT(onActionRenderMonochrome()));
}

void eyeball::closeEvent(QCloseEvent* event)
{
  UNREFERENCED_PARAMETER(event);

  ui.canvas->close();  
}

void eyeball::onActionViewWireframe()
{
  ui.actionViewAuto->setChecked(false);
  ui.actionViewWireframe->setChecked(true);
  ui.actionViewSolidWireframe->setChecked(false);
  reinterpret_cast<int&>(AppGlobalState::get().debugMode()) |= AppGlobalState::DM_WIREFRAME;
  reinterpret_cast<int&>(AppGlobalState::get().debugMode()) &= ~AppGlobalState::DM_SOLID_WIRE;
}

void eyeball::onActionViewSolidWireframe()
{
  ui.actionViewAuto->setChecked(false);
  ui.actionViewWireframe->setChecked(false);
  ui.actionViewSolidWireframe->setChecked(true);
  reinterpret_cast<int&>(AppGlobalState::get().debugMode()) |= AppGlobalState::DM_SOLID_WIRE;
  reinterpret_cast<int&>(AppGlobalState::get().debugMode()) &= ~AppGlobalState::DM_WIREFRAME;
}

void eyeball::onActionViewAuto()
{
  ui.actionViewAuto->setChecked(true);
  ui.actionViewWireframe->setChecked(false);
  ui.actionViewSolidWireframe->setChecked(false);
  AppGlobalState::get().debugMode() = AppGlobalState::DM_DISABLED;
}

void eyeball::onActionRenderBothFaces()
{
  if (ui.actionRenderBothFaces->isChecked())
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) |= AppGlobalState::DM_RENDER_BOTH_FACES;
  }
  else
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) &= ~AppGlobalState::DM_RENDER_BOTH_FACES;
  }
}

void eyeball::onActionDrawNormals()
{
  if (ui.actionDrawNormals->isChecked())
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) |= AppGlobalState::DM_DRAW_NORMALS;
  }
  else
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) &= ~AppGlobalState::DM_DRAW_NORMALS;
  }
}

void eyeball::onActionRenderMonochrome()
{
  if (ui.actionRenderMonochrome->isChecked())
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) |= AppGlobalState::DM_RENDER_MONOCHROME;
  }
  else
  {
    reinterpret_cast<int&>(AppGlobalState::get().debugMode()) &= ~AppGlobalState::DM_RENDER_MONOCHROME;
  }
}
