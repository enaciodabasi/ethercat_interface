/**
 * @file slave_info_widget.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_config_gui/slave_info_widget.hpp"

#include <QRegularExpression>
#include <QDebug>

namespace ethercat_config_gui
{
    
    std::optional<std::vector<SlaveIdentifiers>> parseSlaveIdentifiers(const QVector<BasicSlaveInformation>& basic_slave_infos)
    {

        std::vector<SlaveIdentifiers> identifiersVec;

        QRegularExpression slaveNameRx("(?<=<!\\[CDATA\\[)\\S+?(?=\\s)");
        QRegularExpression vendorIdRx("(?<=<Id>)[^<]+");
        QRegularExpression prDet_Rx("(?<=<Type)[^>]+");
        QRegularExpression productCodeRx ("(?<=ProductCode=\"#)[^\"]+");
        QRegularExpression revisionNoRx("(?<=RevisionNo=\"#)[^\"]+");
        for(std::size_t i = 0; i < basic_slave_infos.size(); i++)
        {
            SlaveIdentifiers siTemp;

            std::string ecXmlCmd = "ethercat xml -p" + std::to_string(i);
            std::array<char, 2048> buffer;
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pPipe(
                popen(
                    ecXmlCmd.c_str(),
                    "r"
                ),
                pclose
            );

            if(!pPipe)
            {
                throw std::runtime_error("popen() failed");
            }

            while(fgets(buffer.data(), buffer.size(), pPipe.get()) != nullptr)
            {
                result += buffer.data();
            }
            if(result.empty())
            {
                break;
            }

            QString slaveXml = QString::fromStdString(result);
            slaveXml.replace("\n", "");
            bool isNameSame = false;

            QRegularExpressionMatchIterator slaveNameRxIt = slaveNameRx.globalMatch(slaveXml);
            while(slaveNameRxIt.hasNext())
            {
                QRegularExpressionMatch match = slaveNameRxIt.next();
                QString matchedStr = match.captured(0);
                matchedStr = matchedStr.simplified();
                matchedStr.replace(" ", "");
                if(matchedStr == basic_slave_infos[i].slaveName_)
                {   
                    isNameSame = true;
                    siTemp.slaveName = basic_slave_infos[i].slaveName_.toStdString();
                }
            }

            if(!isNameSame)
            {
                break;
            }

            QRegularExpressionMatchIterator vendorIdRxIt = vendorIdRx.globalMatch(slaveXml);
            while(vendorIdRxIt.hasNext())
            {
                QRegularExpressionMatch match = vendorIdRxIt.next();
                QString matchedStr = match.captured(0);
                matchedStr = matchedStr.simplified();
                matchedStr.replace(" ", "");
                siTemp.vendorIdStr = matchedStr.toStdString();
                siTemp.vendorID = static_cast<uint64_t>(matchedStr.toInt());
            }

            QRegularExpressionMatchIterator prDetRxIt = prDet_Rx.globalMatch(slaveXml);
            while(prDetRxIt.hasNext())
            {
                QRegularExpressionMatch match = prDetRxIt.next();
                QString matchedStr = match.captured(0);

                QRegularExpressionMatchIterator productCodeRxIt = productCodeRx.globalMatch(matchedStr);
                while(productCodeRxIt.hasNext())
                {
                    QRegularExpressionMatch match = productCodeRxIt.next();
                    QString matchedStr = match.captured(0);
                    matchedStr = matchedStr.simplified();
                    matchedStr.replace(" ", "");
                    siTemp.productCodeStr = matchedStr.toStdString();
                    matchedStr.replace("x", "0x");
                    siTemp.productCode = static_cast<uint64_t>(matchedStr.toInt(nullptr, 16));
                }

                QRegularExpressionMatchIterator revisionNumberRxIt = revisionNoRx.globalMatch(matchedStr);
                while(revisionNumberRxIt.hasNext())
                {
                    QRegularExpressionMatch match = revisionNumberRxIt.next();
                    QString matchedStr = match.captured(0);
                    matchedStr = matchedStr.simplified();
                    matchedStr.replace(" ", "");
                    siTemp.revisionNumberStr = matchedStr.toStdString();
                    matchedStr.replace("x", "0x");
                    siTemp.revisionNumber = static_cast<uint64_t>(matchedStr.toInt(nullptr, 16));
                }
                
            }

            identifiersVec.emplace_back(std::move(siTemp));

        }
        
        if(!identifiersVec.empty())
        {
            return identifiersVec;
        }

        return std::nullopt;
    }

    SlaveInfoWidget::SlaveInfoWidget(QWidget* parent)
        : QWidget(parent)
    {

    }

    SlaveInfoWidget::~SlaveInfoWidget()
    {

    }

    void SlaveInfoWidget::onNetworkRefreshed(const QVector<BasicSlaveInformation>& basic_slave_info)
    {
        auto slaveIdentOpt =  parseSlaveIdentifiers(basic_slave_info);

        if(slaveIdentOpt == std::nullopt)
        {
            return;
        }

        for(auto slave_ident : slaveIdentOpt.value())
        {
            qDebug() << QString::fromStdString(slave_ident.slaveName) << " " << slave_ident.vendorID <<  " " << slave_ident.productCode << " " << slave_ident.revisionNumber;
        }
    }

} // End of namespace ethercat_config_gui