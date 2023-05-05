/**
 * @file slave_info_widget.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SLAVE_INFO_WIDGET_HPP
#define SLAVE_INFO_WIDGET_HPP

#include <QWidget>
#include <QObject>

#include <thread>
#include <optional>
#include <memory>

#include "ethercat_config_gui/defs.hpp"

namespace ethercat_config_gui
{
    struct SlaveIdentifiers
    {   
        std::string slaveName;
        uint64_t vendorID;
        std::string vendorIdStr;
        uint64_t productCode;
        std::string productCodeStr;
        uint64_t revisionNumber;
        std::string revisionNumberStr;
    };

    std::optional<std::vector<SlaveIdentifiers>> parseSlaveIdentifiers(const QVector<BasicSlaveInformation>& basic_slave_infos);

    class SlaveInfoWidget : public QWidget
    {
        Q_OBJECT

        public:

        SlaveInfoWidget(QWidget* parent = nullptr);
        ~SlaveInfoWidget();

        private:


        signals:

        public slots:

        void onNetworkRefreshed(const QVector<BasicSlaveInformation>& basic_slave_info);
        
    };

} // End of namespace ethercat_config_gui

#endif // SLAVE_INFO_WIDGET_HPP