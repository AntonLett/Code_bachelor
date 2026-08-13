#include <argparse/argparse.hpp>
#include <algorithm>
#include <benchmark/benchmark.h>
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
    Client *client = Client::getClientInstance("127.0.0.1", "8085");
    std::string message = "";
    for (const std::string path : paths)
    {
        std::string next_path = std::to_string(event) + " " + path + "\n";
        if ((message + next_path).length() > 4096)
        {
            response = client->sendMessage(message);
            if (response == "ERROR sending failed")
            {
                std::cout << Color::warning_message() << response << std::endl;
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

void readFile(const std::filesystem::path &oldFileName, const std::filesystem::path &newFileName, std::vector<std::string> &created)
{
    std::vector<std::string> deleted, altered;
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
        [](const std::string& a, const std::string& b){
            return a.substr(a.find(" -- ") + 4) < b.substr(b.find(" -- ") + 4);
        }
    );
    std::ofstream ofile(fileName);
    for(const std::string& l : lines){
        ofile << l << "\n";
    }
}

class UpdaterFixture : public benchmark::Fixture
{
public:
    std::vector<std::string> created {};
    std::string old = ".\\testing\\lists\\new_og_d_sorted.txt";
    std::string cur = ".\\testing\\lists\\new_25.txt";
    void SetUp(const ::benchmark::State &state) override
    {
        readFile(old, cur, created);
        for (auto &a : created){
            if(!a.empty()){
                a.pop_back();
            }
        }
    }
    void TearDown(const ::benchmark::State &state) override
    {
        created.clear();
    }
};

BENCHMARK_F(UpdaterFixture, BM_Compare)(benchmark::State &state)
{
    for (auto _ : state)
    {
        sortFile(cur);
        readFile(old, cur, created);
    }
}

BENCHMARK_F(UpdaterFixture, BM_SendData)(benchmark::State &state)
{
    for (auto _ : state)
    {
        send_data(created, CREATE);
        // send_data(deleted, DELETE);
        
        const std::string done_message = std::to_string(DONE) + " Done\n";
        Client *client = Client::getClientInstance("127.0.0.1", "8085");
        client->sendMessage(done_message);
    }
}

BENCHMARK_MAIN();