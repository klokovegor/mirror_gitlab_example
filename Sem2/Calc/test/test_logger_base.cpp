/// @brief Демонстрация полиморфизма logger_base (lab10, этап 1): вызовы без проверок.

#include "logger_base.h"

#include "gtest/gtest.h"

#include <iostream>
#include <string>

namespace {

inline void log_error_by_value(logger_base logger) {
    logger.write_message("disk is almost full");
}

class logger_base_static {
public:
    void write_message(const std::string& msg) {
        std::cout << "[base] " << msg << std::endl;
    }
};

class file_logger_static : public logger_base_static {
public:
    void write_message(const std::string& msg) {
        std::cout << "[file] " << msg << std::endl;
    }
};

inline void log_error_static(logger_base_static& logger) {
    logger.write_message("disk is almost full");
}

class file_logger_wrong_sig : public logger_base {
public:
    void write_message(const std::string& msg, int /*level*/) {
        std::cout << "[file-wrong] " << msg << std::endl;
    }
};

} // namespace

TEST(LoggerBaseDemo, PolymorphicViaReference) {
    file_logger file_log;
    max_messenger_logger max_log;
    log_error(file_log);
    log_error(max_log);
}

TEST(LoggerBaseDemo, ObjectSlicingByValue) {
    file_logger file_log;
    log_error_by_value(file_log);
}

TEST(LoggerBaseDemo, WithoutVirtual) {
    file_logger_static sl;
    log_error_static(sl);
}

TEST(LoggerBaseDemo, WithoutOverrideWrongSignature) {
    file_logger_wrong_sig bad;
    log_error(bad);
}
