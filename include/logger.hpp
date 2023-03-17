/**
 * @file logger.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <condition_variable>
#include <string>
#include <string_view>
#include <fstream>
#include <queue>
#include <chrono>
#include <unordered_map>
#include <iostream>

#include <time.h>
#include <signal.h>


namespace ethercat_interface
{

    namespace logger
    {

        enum class LogLevel
        {
            DEBUG,
            INFO,
            ERROR,
            FATAL
        };

        enum class LoggerMode
        {
            FILE,
            SCREEN,
            FULL
        };

        struct Log
        {
            LogLevel logLevel;
            std::string writerName;
            std::string text;
        };

        class Logger
        {
            public:

            Logger(const std::string& file_path, LoggerMode logger_mode = LoggerMode::FILE);

            ~Logger();

            void log(
                LogLevel log_level,
                const std::string& writer_name,
                const std::string& log
            );

            inline void setFilePath(const std::string& path)
            {
                m_FilePath = path;
            }
            
            private:

            std::shared_mutex m_QueueMutex;

            std::condition_variable_any m_QueueCondition;

            std::queue<std::string> m_LogQueue;

            std::thread m_WriterThread;

            std::ofstream m_OutFile;

            std::string m_FilePath;

            LoggerMode m_LoggerMode;

            void writeToFile();

            const std::unordered_map<LogLevel, std::string_view> m_LogLevelStringMap = {
                {LogLevel::DEBUG, "DEBUG"},
                {LogLevel::INFO, "INFO"},
                {LogLevel::ERROR, "ERROR"},
                {LogLevel::FATAL, "FATAL"}
            };
            
            const std::string prepareLogAsString(const Log& log);

            void insertLogToQueue(
                LogLevel log_level,
                const std::string& writer_name,
                const std::string& log
            );

            bool m_WorkFinished = false;

        };

        constexpr auto DEBUG = LogLevel::DEBUG;
        constexpr auto INFO = LogLevel::INFO;
        constexpr auto ERROR = LogLevel::ERROR;
        constexpr auto FATAL = LogLevel::FATAL;
        
        constexpr auto FILE = LoggerMode::FILE;
        constexpr auto SCREEN = LoggerMode::SCREEN;
        constexpr auto FULL = LoggerMode::FULL;

        
    }

    
}

#endif