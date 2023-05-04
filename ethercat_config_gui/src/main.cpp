/**
 * @file main.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_config_gui/main_window.hpp"
#include "ethercat_config_gui/defs.hpp"

#include <QApplication>

int main(int argc, char** argv)
{   

    qRegisterMetaType<BasicSlaveInformation>();

    QApplication app(argc, argv);

    ethercat_config_gui::MainWindow* mw = new ethercat_config_gui::MainWindow();

    mw->show();

    return app.exec();
}