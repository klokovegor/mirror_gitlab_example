#pragma once

#include <iostream>
#include <string>

class logger_base {
public:
    virtual void write_message(const std::string& msg) {
        std::cout << "[base] " << msg << std::endl;
    }
    virtual ~logger_base() = default;
};

class file_logger : public logger_base {
public:
    void write_message(const std::string& msg) override {
        std::cout << "[file] " << msg << std::endl;
    }
};

class max_messenger_logger : public logger_base {
public:
    void write_message(const std::string& msg) override {
        std::cout << "[max] " << msg << std::endl;
    }
};

inline void log_error(logger_base& logger) {
    logger.write_message("disk is almost full");
}
