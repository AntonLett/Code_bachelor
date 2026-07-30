#include <fstream>
#include <sstream>
#include <unordered_map>

std::unordered_map<std::string, std::string> parse_settings(std::string file_path){
    std::ifstream file (file_path);
    std::unordered_map<std::string, std::string> settings;
    if(file){
        std::string key, separator, value;
        while(file >> key >> separator >> value) {
            settings[key] = value;
        }
    }

    return settings;
}
