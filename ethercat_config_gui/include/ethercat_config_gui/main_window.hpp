/**
 * @file main_window.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QObject>
#include <QWindow>

#include <ethercat_config_gui/central_widget.hpp>

namespace ethercat_config_gui
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        public:

        MainWindow(QWidget* parent = nullptr);

        ~MainWindow();

        private:

        CentralWidget* m_CentralWidget;


    };
} // End of namespace ethercat_config_gui

#endif // MAIN_WINDOW_HPP
