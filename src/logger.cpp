/**
 * @file logger.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/logger.hpp"

namespace ethercat_interface
{
    namespace logger
    {
        namespace 
        {
            /**
            * @brief Returns the current date and time as a string.
            * 
            * @return Current data and time as string.
            */
            const std::string getCurrentTime()
            {
                time_t now = time(0);
                struct tm tstruct;
                char buffer[80];
                tstruct = *localtime(&now);
    
                strftime(buffer, sizeof(buffer), "%Y-%m-%d.%H:%M:%S", &tstruct);
    
                return buffer;
            }
        }

        Logger::Logger(const std::string& file_path, LoggerMode logger_mode)
            : m_FilePath{file_path}, m_LoggerMode{logger_mode}
        {
            if(!m_OutFile.is_open())
            {
                m_OutFile.open(file_path + getCurrentTime() + ".txt");
            }

            m_WriterThread = std::thread(
                &Logger::writeToFile,
                this
            );

            m_WriterThread.detach();
        }

        Logger::~Logger()
        {
            m_WorkFinished = true;

            m_OutFile.close();
        }
        
        void Logger::log(
            LogLevel log_level,
            const std::string& writer_name,
            const std::string& log
        )
        {
            this->insertLogToQueue(log_level, writer_name, log);
        }

        void Logger::writeToFile()
        {
            while(!m_WorkFinished)
            {
                std::unique_lock<std::shared_mutex> ulk(m_QueueMutex);
                m_QueueCondition.wait(
                    ulk, [this]{return !this->m_LogQueue.empty();}
                );

                if(!m_OutFile.is_open())
                {
                    
                    break;
                }

                const std::string logToWrite = m_LogQueue.front();
                m_LogQueue.pop();
                ulk.unlock();

                m_OutFile << logToWrite << std::endl;
                if(m_LoggerMode == LoggerMode::FULL || m_LoggerMode == LoggerMode::SCREEN)
                {
                    std::cout << logToWrite << std::endl;
                }

            }
            
        }

        const std::string Logger::prepareLogAsString(const Log& log)
        {
            std::string logStr = "[";
            logStr += std::string(m_LogLevelStringMap.at(log.logLevel));
            logStr += "] ";
            logStr += getCurrentTime() + ": ";
            logStr += log.writerName + ": ";
            logStr += log.text;

            return logStr;

        }

        void Logger::insertLogToQueue(
                LogLevel log_level,
                const std::string& writer_name,
                const std::string& log
            )
        {
            const std::string logInfo = prepareLogAsString({log_level, writer_name, log});
            {
                std::unique_lock<std::shared_mutex> ulk(m_QueueMutex);
                m_LogQueue.push(logInfo);
            } 
            m_QueueCondition.notify_one();
        }

        
        

    }
}