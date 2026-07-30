#pragma once
#include <string>
#include <regex>
#include <cmath>
#include "color_print.hpp"

std::string unit_to_byte(const std::string& s){
    //TODO: Error handling
    std::cout << Color::warning_message() << "This conversion uses base 1000, not base 1024.\n";
    std::regex sizeRegex(R"(^(\d+[\.,]?\d+) ?(B|kB|MB|GB|TB|PB|bytes)$)", std::regex_constants::icase);
    std::smatch match;
    std::regex_match(s, match, sizeRegex);
    std::string number = match.str(1);
    std::replace(number.begin(),number.end(), ',', '.');    //both decimal point and decimal comma are allowed
    double size = std::stod(number);
    switch(tolower(match.str(2).at(0))){
        case 'b':
            std::cout << "Unit is already byte." << std::endl;
            break;
        case 'k':
            size *= pow(10,3);
            break;
        case 'm':
            size *= pow(10,6);
            break;
        case 'g':
            size *= pow(10,9);
            break;
        case 'p':
            size *= pow(10,15);
            break;
        default:
            std::cerr << Color::colorize("Error: ", Color::BOLD,Color::RED) << "Unknown Unit in unit_to_byte.\n";
            break;
    }
    std::string result = std::to_string(size);
    result.erase(result.find_last_not_of('0') + 1, std::string::npos);
    if (!result.empty() && result.back() == '.') {
        result.pop_back();
    }
    return result;
}

// check if string is a date in the ISO format
std::string makeISODate(const std::string& s) {
    /*
    WARNING: THIS IS EXIFTOOL TIME FORMAT SPECIFIC
    Matches current date, current time, timezone:
    YEAR:MONTH:DAY whitespace/T HOUR:MINUTES:SECONDS +/- hours:minutes  (eg: 2026:03:04 15:58:07+01:00 or 2026:03:04 15:58:07-01:00)
    YEAR:MONTH:DAY whitespace/T HOUR:MINUTES:SECONDS                    (eg: 2026:03:04 15:58:07 or 2026:03:04T15:58:07)
    YEAR:MONTH:DAY                                                      (eg: 2026:03:04)

    if matched, matches are in FOLLOWING ORDER:
    total matched string                        (eg: 2026:03:04 15:58:07+01:00)
    current date                                (eg: 2026:03:04)
    current time + leading wspc/T + timezone    (eg: T15:58:07 or  15:58:07)
    current time no leading wspc/T, no timezone (eg: 15:58:07)
    timezone                                    (eg: +01:00 or -01:00)
    only the hours of the timezone with sign    (eg: +01 or -01)
    */
    std::regex dateRegex(R"(^(\d{4}:\d{2}:\d{2})([\ T](\d{2}:\d{2}:\d{2})(([\+\-]\d{2}):\d{2})?)?$)");
    std::smatch matches;
    if(std::regex_match(s, matches, dateRegex)) {
        std::string dateStr = s;
        dateStr.replace(4,1,"-");
        dateStr.replace(7,1,"-");
        if(matches[2].matched) {    // String includes Time
            dateStr.replace(10,1,"T");
            if(matches[4].matched){ //string also includes timezone
            } 
        } 
        return dateStr;
    }
    else return "";
}

// check if string is a decimal number, using a comma "," or a dot "."
bool isDouble(const std::string& s) {
    std::regex doubleRegex(R"(^[+-]?\d+[,\.]\d+$)");
    return std::regex_match(s, doubleRegex);
}

// check if string is integer, without comma or whitespaces
bool isInteger(const std::string& s) {
    std::regex intRegex(R"(^[+-]?\d+$)");
    return std::regex_match(s, intRegex);
}

bool isBoolean(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return lower == "true" || lower == "false" || lower == "True" || lower == "False";
}

bool isFileSize(const std::string& s){
    std::regex sizeRegex(R"(^\d+[\.,]?\d* ?(B|kB|MB|GB|TB|PB|bytes)$)", std::regex_constants::icase);
    return std::regex_match(s, sizeRegex);
}

std::string needsQuotes(const std::string& value) {
    std::string val = makeISODate(value);
    if (val != "") {
        return "\"" + val + "\"";
    }
    else if (isBoolean(value)) {
        return value;
    } else if (isDouble(value)) {
        return value;
    } else if (isInteger(value)) {
        return value;
    } else if(isFileSize(value)) {
        return unit_to_byte(value);
    } else {
        return "\"" + value + "\""; // Standard: String
    }
}
