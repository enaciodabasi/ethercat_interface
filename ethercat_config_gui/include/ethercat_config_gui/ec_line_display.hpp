/**
 * @file ec_line_display.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef EC_LINE_DISPLAY_HPP
#define EC_LINE_DISPLAY_HPP

#include <QWidget>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include <iostream>
#include <cstdio>
#include <array>
#include <thread>
#include <optional>

namespace ethercat_config_gui
{

    std::optional<QString> scanEcNetwork();
    
    QStringList parseNetworkInfo(const QString& network_info_raw);

    class EcLineDisplayWidget : public QWidget
    {   
        Q_OBJECT

        public:

        EcLineDisplayWidget(QWidget* parent = nullptr);
        ~EcLineDisplayWidget();

        signals:

        void refreshSlaveList();

        private:

        QVBoxLayout* m_MainLayout;

        QPushButton* m_ScanNetworkBtn;

        QListWidget* m_SlaveListWidget;

        QStringList m_SlaveNameList;

        void onScanNetworkBtnClicked();
        
        void onRefreshSlaveList();

    };
} // End of namespace ethercat_config_gui

#endif // EC_LINE_DISPLAY_HPP