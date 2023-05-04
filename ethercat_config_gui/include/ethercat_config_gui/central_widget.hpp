/**
 * @file central_widget.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CENTRAL_WIDGET_HPP
#define CENTRAL_WIDGET_HPP

#include <QWidget>
#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QProcess>

#include "ethercat_config_gui/ec_line_display.hpp"

namespace ethercat_config_gui
{

    class CentralWidget : public QWidget
    {
        public:

        CentralWidget(QWidget* parent = nullptr);
        ~CentralWidget();

        private:

        EcLineDisplayWidget* m_LineDisplay;


        QHBoxLayout* m_MainLayout;

    };

} // End of namespace ethercat_config_gui

#endif // CENTRAL_WIDGET_HPP