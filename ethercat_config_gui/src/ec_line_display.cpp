/**
 * @file ec_line_display.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_config_gui/ec_line_display.hpp"

#include <QDebug>

namespace ethercat_config_gui
{

EcLineDisplayWidget::EcLineDisplayWidget(QWidget* parent)
    : QWidget(parent)
{   

    m_MainLayout = new QVBoxLayout(this);

    m_ScanNetworkBtn = new QPushButton("Scan EtherCAT network");
    m_MainLayout->addWidget(m_ScanNetworkBtn);

    m_SlaveListWidget = new QListWidget(this);
    m_MainLayout->addWidget(m_SlaveListWidget);

    connect(
        m_ScanNetworkBtn,
        &QPushButton::clicked,
        this,
        &EcLineDisplayWidget::onScanNetworkBtnClicked  
    );

    connect(
        this,
        &EcLineDisplayWidget::refreshSlaveList,
        this,
        &EcLineDisplayWidget::onRefreshSlaveList
    );

    this->setLayout(m_MainLayout);
}

EcLineDisplayWidget::~EcLineDisplayWidget()
{

}

void EcLineDisplayWidget::onScanNetworkBtnClicked()
{
    std::optional<QString> networkInfoRawOpt;

    try{
        networkInfoRawOpt = scanEcNetwork();
    }catch(std::runtime_error& ex)
    {
        qDebug() << ex.what();
    }

    if(networkInfoRawOpt == std::nullopt)
    {
        return;
    }

    const QString networkInfoRaw = networkInfoRawOpt.value();

    m_SlaveNameList = parseNetworkInfo(networkInfoRaw);

    emit refreshSlaveList();
}

void EcLineDisplayWidget::onRefreshSlaveList()
{   
    m_SlaveListWidget->clear();

    for(QStringList::iterator it = m_SlaveNameList.begin(); it != m_SlaveNameList.end(); it++)
    {
        const QString slaveName = *it;
        m_BasicSlaveInfoContainer.push_back({*it, 0, 0});

        m_SlaveListWidget->addItem(
            new QListWidgetItem(
                slaveName
            )
        );
    }
    
    emit networkRefreshed(m_BasicSlaveInfoContainer);
}

std::optional<QString> scanEcNetwork()
{
    QString networkInfo;

    std::thread scannerThread(
        [&networkInfo]()
        {
            std::array<char, 256> buffer;
            std::string res;
            std::unique_ptr<FILE, decltype(&pclose)> pPipe(popen(
                "ethercat slaves",
                "r"
            ), pclose);

            if(!pPipe)
            {   
                throw std::runtime_error("popen() failed");
            }
            while(fgets(buffer.data(), buffer.size(), pPipe.get()) != nullptr)
            {
                res += buffer.data();
            }

            networkInfo = QString::fromStdString(res);
        }
    );
    if(scannerThread.joinable())
        scannerThread.join();
    if(networkInfo.isEmpty())
        return std::nullopt;

    return networkInfo;
}

QStringList parseNetworkInfo(const QString& network_info_raw)
{
    auto networkInfo = network_info_raw.split('\n');
    QStringList slaveNames;
    QRegularExpression rx("(?<=\\+)\\s*\\S+?(?=\\s|$)");
    
    for(QStringList::iterator it = networkInfo.begin(); it != networkInfo.end(); it++)
    {
        QRegularExpressionMatchIterator rxIt = rx.globalMatch(*it);
        while(rxIt.hasNext())
        {
            QRegularExpressionMatch match = rxIt.next();
            QString matchedStr = match.captured(0);
            matchedStr = matchedStr.simplified();
            matchedStr.replace(" ", "");
            slaveNames << matchedStr;
        }    
        
    }

    return slaveNames;
}

} // End of namespace ethercat_config_gui 