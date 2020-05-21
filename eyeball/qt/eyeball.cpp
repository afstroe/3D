#include "eyeball.h"

eyeball::eyeball(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void eyeball::closeEvent(QCloseEvent* event)
{
  UNREFERENCED_PARAMETER(event);

  ui.canvas->close();  
}
