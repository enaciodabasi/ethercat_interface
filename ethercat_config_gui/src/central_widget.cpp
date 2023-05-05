/**
 * @file central_widget.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_config_gui/central_widget.hpp"

namespace ethercat_config_gui
{

    CentralWidget::CentralWidget(QWidget* parent)
        : QWidget(parent)
    {
        m_LineDisplay = new EcLineDisplayWidget(this);
        m_SlaveInfoWidget = new SlaveInfoWidget(this);

        connect(
            m_LineDisplay,
            &EcLineDisplayWidget::networkRefreshed,
            m_SlaveInfoWidget,
            &SlaveInfoWidget::onNetworkRefreshed
        );

        m_MainLayout = new QHBoxLayout();
        m_MainLayout->addWidget(m_LineDisplay);
        m_MainLayout->addWidget(m_SlaveInfoWidget);

        this->setLayout(m_MainLayout);
    }

    CentralWidget::~CentralWidget()
    {

    }

} // End of namespace ethercat_config_gui