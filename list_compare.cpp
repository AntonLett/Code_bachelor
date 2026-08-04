#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
// #include <chrono>

std::vector<std::string> created, deleted, altered, same;

struct basicFileInfo
{
    std::string name;
    std::vector<std::string> mdata;
};

bool check_change(const basicFileInfo &old, const basicFileInfo &cur)
{
    if(old.mdata.size() != cur.mdata.size()) return true;
    for(int i{}; i<cur.mdata.size(); ++i){
        if(old.mdata.at(i) != cur.mdata.at(i)) return true;
    }
    return false;
}

bool readLine(std::ifstream &file, basicFileInfo &fi, char delimValue = ';', char delimEntry = '\n'){
    std::string line;
    bool status = static_cast<bool>(getline(file, line, delimEntry));
    // when no new line available, stop
    if (!status)
        return status;
    size_t pathPos = line.rfind(" -- ");
    if(pathPos != std::string::npos && (pathPos + 4) < line.length()){
        fi.name = line.substr(pathPos + 4);
        std::string metadata = line.substr(0, pathPos);
        std::stringstream ss(metadata);
        std::string token;
        std::vector<std::string> tokens;

        while(std::getline(ss, token, delimValue)){
            tokens.push_back(token);
        }
    } else {
        std::cerr << "Filename not found.\n";
        fi.name = line;
    }
    return status;
}

void readFile(const std::string &oldFileName, const std::string &newFileName)
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
                altered.push_back(old.name);
            } else same.push_back(old.name);
            // std::cout << "same" << std::endl;
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

void printToFile(const std::string& fileName, const std::vector<std::string>& data, int eventNumber){
    std::ofstream file(fileName);

    if(file.is_open()){
        for(const auto& d : data){
            file << eventNumber << " " << d << "\n";
        }
    }
}

int main(int argc, char **argv)
{
    // clock_t start_time, end_time;
    // double time_taken;
    // auto t1 = std::chrono::high_resolution_clock::now();

    readFile(argv[1], argv[2]);

    // auto t2 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<long double, std::milli> ms_double = t2 - t1;
    // std::cout << "Comparing took: " << ms_double.count() << " ms\n";

    printToFile("updater_files/created", created, 1);
    printToFile("updater_files/deleted", deleted, 2);
    printToFile("updater_files/altered", altered, 4);
    printToFile("updater_files/same", same, 0);
    // t2 = std::chrono::high_resolution_clock::now();
    // ms_double = t2 - t1;
    // std::cout << "Including writing to file it took: " << ms_double.count() << " ms\n";
}