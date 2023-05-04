/**
 * @file main_window.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_config_gui/main_window.hpp"

namespace ethercat_config_gui
{

    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
    {
        m_CentralWidget = new CentralWidget(this);

        this->setCentralWidget(m_CentralWidget);   
    }

    MainWindow::~MainWindow()
    {

    }

} // End of namespace ethercat_config_gui