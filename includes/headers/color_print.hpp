#pragma once
#include <iostream>
#include <string>

class Color {
public:
    const static std::string RESET;
    const static std::string RED;
    const static std::string GREEN;
    const static std::string YELLOW;
    const static std::string BLUE;
    const static std::string MAGENTA;
    const static std::string CYAN;
    const static std::string WHITE;
    const static std::string BOLD;
    
    template <typename... Colors>
    static std::string colorize(const std::string& text, Colors... colors) {
        std::string cs = "";
        (void)std::initializer_list<int>{(cs += colors, 0) ...};
        return cs + text + RESET;
    }

    static std::string error_message(std::string message = "ERROR:") {
        return BOLD + RED + message + RESET;
    }
    static std::string warning_message(std::string message = "WARNING:") {
        return BOLD + YELLOW + message + RESET;
    }
    static std::string info_message(std::string message = "INFO:") {
        return BOLD + CYAN + message + RESET;
    }
};

const std::string Color::RESET = "\033[0m";
const std::string Color::RED = "\x1B[31m";
const std::string Color::GREEN = "\x1B[32m";
const std::string Color::YELLOW = "\x1B[33m";
const std::string Color::BLUE = "\x1B[34m";
const std::string Color::MAGENTA = "\x1B[35m";
const std::string Color::CYAN = "\x1B[36m";
const std::string Color::WHITE = "\x1B[37m";
const std::string Color::BOLD = "\x1B[1m";