/**
 * @file defs.hpp
 * @author your name (you@domain.com)
 * @brief Contains custom objects for Metatype Declaration
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <QObject>
#include <QString>

struct BasicSlaveInformation
{
    QString slaveName_;
    uint16_t slavePosition;
    uint16_t slaveAlias;
}; Q_DECLARE_METATYPE(BasicSlaveInformation)  
