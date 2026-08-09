#include <argparse/argparse.hpp>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "includes/headers/client.hpp"
#include "includes/headers/protocol.h"
#include "includes/headers/color_print.hpp"
#include "includes/headers/toml.hpp"
// #include <chrono>

std::string SERVER_PORT{};
std::string SERVER_IP{};
std::vector<std::string> created, deleted, altered;
const size_t MAX_SIZE = 4096;

struct basicFileInfo
{
    std::string name;
    std::vector<std::string> mdata;
};

void send_data(const std::vector<std::string> &paths, int event)
{
    // TODO: get timing right, wait for answers before sending more data as to not overwhelm the server
    std::string response = "";
    unsigned short try_count = 0;
    Client *client = Client::getClientInstance(SERVER_IP, SERVER_PORT);
    std::string message = "";
    for (const std::string path : paths)
    {
        std::string next_path = std::to_string(event) + " " + path + "\n";
        if ((message + next_path).length() > MAX_SIZE)
        {
            response = client->sendMessage(message);
            if (response != "ERROR sending failed")
            {
                std::cout << Color::info_message() << response << std::endl;
            }
            message = "";
        }
        message += next_path;
    }
    if (message != "")
    {
        response = client->sendMessage(message);
    }
    if (response == "ERROR")
    {
        std::cerr << "Error in response. Exiting." << std::endl;
        exit(1);
    }
}

bool check_change(const basicFileInfo &old, const basicFileInfo &cur)
{
    if (old.mdata.size() != cur.mdata.size())
        return true;
    for (int i{}; i < cur.mdata.size(); ++i)
    {
        if (old.mdata.at(i) != cur.mdata.at(i))
            return true;
    }
    return false;
}

bool readLine(std::ifstream &file, basicFileInfo &fi, char delimValue = ';', char delimEntry = '\n')
{
    std::string line;
    bool status = static_cast<bool>(getline(file, line, delimEntry));
    // when no new line available, stop
    if (!status)
        return status;
    size_t pathPos = line.rfind(" -- ");
    if (pathPos != std::string::npos && (pathPos + 4) < line.length())
    {
        fi.name = line.substr(pathPos + 4);
        std::string metadata = line.substr(0, pathPos);
        std::stringstream ss(metadata);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, delimValue))
        {
            tokens.push_back(token);
        }
    }
    else
    {
        std::cerr << "Filename not found.\n";
        fi.name = line;
    }
    return status;
}

void readFile(const std::filesystem::path &oldFileName, const std::filesystem::path &newFileName)
{
    std::ifstream oldFile(oldFileName);
    std::ifstream newFile(newFileName);

    basicFileInfo old, current;
    char delim = '\n';
    bool oldNext = readLine(oldFile, old);
    bool newNext = readLine(newFile, current);

    while (oldNext && newNext)
    {
        if (old.name == current.name)
        {
            // check if change happened
            if (check_change(old, current))
            {
                created.push_back(old.name); // changed not created, but bug in change so for testing
            }
            oldNext = readLine(oldFile, old);
            newNext = readLine(newFile, current);
            continue;
        }
        if (old.name > current.name)
        {
            // std::cout << "Old: " << old << "\nNew: " << current << std::endl;
            // std::cout << "File inserted: " << current << "\n";
            created.push_back(current.name);
            newNext = readLine(newFile, current);
        }
        else if (current.name > old.name)
        {
            // std::cout << "File deleted: " << old << "\n";
            deleted.push_back(old.name);
            oldNext = readLine(oldFile, old);
        }
    }
    if (!oldNext && !newNext)
    {
        std::cout << "Both done.\n";
    }
    else if (oldNext)
    {
        // add old to Deleted
        // std::cout << "Deleted: " << old << std::endl;
        deleted.push_back(old.name);
        // continue through file to the end
        while (readLine(oldFile, old))
        {
            // add to Deleted
            // std::cout << "Deleted: " << old << std::endl;
            deleted.push_back(old.name);
        }
    }
    else if (newNext)
    {
        // add current to Created
        // std::cout << "Created: " << current << std::endl;
        created.push_back(current.name);
        // continue through file to the end
        while (readLine(newFile, current))
        {
            // add to Created
            // std::cout << "Created: " << current << std::endl;
            created.push_back(current.name);
        }
    }
}

void printToFile(const std::string &fileName, const std::vector<std::string> &data, int eventNumber)
{
    std::ofstream file(fileName);

    if (file.is_open())
    {
        for (const auto &d : data)
        {
            file << eventNumber << " " << d << "\n";
        }
    }
}

void sortFile(const std::filesystem::path &fileName){
    std::ifstream file(fileName);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)){
        lines.push_back(line);
    }
    file.close();
    std::sort( lines.begin(), lines.end(),
        [](const std::string& a, const std::string b){
            return a.substr(a.find(" -- ") + 4) < b.substr(b.find(" -- ") + 4);
        }
    );
    std::ofstream ofile(fileName);
    for(const std::string& l : lines){
        ofile << l << "\n";
    }
}

int main(int argc, char **argv)
{
    // clock_t start_time, end_time;
    // double time_taken;
    // auto t1 = std::chrono::high_resolution_clock::now();

    argparse::ArgumentParser program("Crawler");
    program.add_argument("-o", "--old")
        .help("File containing the old status of the folder");
    program.add_argument("-c", "--current")
        .help("File containing the current status of the folder");
    program.add_argument("-s", "--settings")
        .help("Path to settings")
        .default_value("../settings/settings.toml");

    try
    {
        program.parse_args(argc, argv);
        std::filesystem::path path_to_settings = program.get("-s");
        std::filesystem::path old = program.get("-o");
        std::filesystem::path cur = program.get("-c");
        if (!std::filesystem::exists(path_to_settings) || !std::filesystem::exists(old) || !std::filesystem::exists(cur))
        {
            std::cerr << "Invalid path.\n";
            std::cerr << "Usage: " << argv[0] << " -s <path/to/settings.toml> -o <path/tp/old> -c <path/to/current>\n";
            return 1;
        }

        // settings for server
        const auto settings = toml::parse(path_to_settings);
        const auto SERVER_SETTINGS = toml::find(settings, "server");
        int server_port = toml::find<int>(SERVER_SETTINGS, "PORT");
        SERVER_PORT = std::to_string(server_port);
        SERVER_IP = toml::find<std::string>(SERVER_SETTINGS, "IP");

        sortFile(cur);
        readFile(old, cur);

        // auto t2 = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<long double, std::milli> ms_double = t2 - t1;
        // std::cout << "Comparing took: " << ms_double.count() << " ms\n";

        send_data(deleted, DELETE);
        send_data(created, CREATE);

        // printToFile("updater_files/created", created, 1);
        // printToFile("updater_files/deleted", deleted, 2);
        // printToFile("updater_files/altered", altered, 4);
        // t2 = std::chrono::high_resolution_clock::now();
        // ms_double = t2 - t1;
        // std::cout << "Including writing to file it took: " << ms_double.count() << " ms\n";
    }
    catch (...)
    {
        std::cerr << Color::error_message() << "in Updater\n";
    }
}