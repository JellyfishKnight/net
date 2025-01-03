#include "logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>

namespace utils {

LoggerManager* LoggerManager::instance = nullptr;

std::queue<std::tuple<LogLevel, LoggerManager::Logger, std::string>> LoggerManager::m_log_queue;

std::unordered_map<std::string, LoggerManager::Logger> LoggerManager::m_loggers;

std::unordered_map<std::string, std::fstream> LoggerManager::m_files;

std::mutex LoggerManager::m_file_mutex;

std::thread LoggerManager::m_log_thread;

bool LoggerManager::m_async_logging_enabled = false;

LoggerManager::LoggerManager() {}

LoggerManager& LoggerManager::get_instance() {
    if (instance == nullptr) {
        instance = new LoggerManager();
    } 
    return *instance;
}

LoggerManager::Logger& LoggerManager::get_logger(std::string logger_name, std::string path) {
    if (m_loggers.find(logger_name) == m_loggers.end()) {
        m_loggers[logger_name].logger_name = logger_name;
        m_loggers[logger_name].path = path;
    } else {
        if (path != m_loggers[logger_name].path) {
            set_log_path(m_loggers[logger_name], path);
        }
    }
    return m_loggers[logger_name];
}

void LoggerManager::log(const Logger& logger, LogLevel log_level, std::string message) {
    const static std::unordered_map<LogLevel, std::string> log_level_map = {
        { LogLevel::DEBUG, "DEBUG" },
        { LogLevel::INFO, "INFO" },
        { LogLevel::WARNING, "WARNING" },
        { LogLevel::ERROR, "ERROR" },
        { LogLevel::FATAL, "FATAL" }
    };

    std::lock_guard<std::mutex> lock(m_file_mutex);
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    if (logger.path.empty()) {
        // log to console
        std::cout << std::ctime(&now_c) << " [" << log_level_map.at(log_level) << "][" << logger.logger_name << "]:" << message << std::endl;
        return;
    }
    if (m_files.find(logger.logger_name) == m_files.end()) {
        m_files[logger.logger_name].open(logger.path, std::ios::out | std::ios::app);
    }

    m_files[logger.logger_name] << std::ctime(&now_c) << " [" << log_level_map.at(log_level) << "][" << logger.logger_name << "]:" << message << std::endl;
}

void LoggerManager::async_log(LogLevel log_level, const Logger& logger, std::string message) {
    m_log_queue.push(std::make_tuple(log_level, logger, message));
}

void LoggerManager::set_log_path(Logger& logger, const std::string& path) {
    logger.path = path;
}

LoggerManager::~LoggerManager() {
    for (auto& [logger_name, file] : m_files) {
        file.close();
    }
    delete instance;

    m_log_thread.join();
}

void LoggerManager::enable_async_logging() {
    assert(instance != nullptr);
    // if thread is already running, return
    if (m_log_thread.joinable()) {
        return;
    }
    m_async_logging_enabled = true;
    m_log_thread = std::thread([&] {
        while (instance != nullptr && m_async_logging_enabled) {
            if (!m_log_queue.empty()) {
                auto [log_level, logger, message] = m_log_queue.front();
                log(logger, log_level, message);
                m_log_queue.pop();
            }
        }
    });
}

void LoggerManager::disable_async_logging() {
    m_async_logging_enabled = false;
    m_log_thread.join();
}

} // namespace utils