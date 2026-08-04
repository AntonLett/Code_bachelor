#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

std::vector<std::string> created, deleted, altered;

struct basicFileInfo
{
    std::string name;
    std::string fileSize;
    std::string mtime;
};

bool check_change(const basicFileInfo &old, const basicFileInfo &cur)
{
    if (old.fileSize != cur.fileSize)
        return true;
    if (old.mtime != cur.mtime)
        return true;
    return false;
}

bool readLine(std::ifstream &file, basicFileInfo &fi, char delimValue = ';', char delimEntry = '\n')
{
    std::string line;
    bool status = static_cast<bool>(getline(file, line, delimEntry));
    // when no new line available, stop
    if (!status)
        return status;
    // get the metadata from the line
    std::stringstream ss(line);
    std::getline(ss, fi.name, delimValue);
    std::getline(ss, fi.fileSize, delimValue);
    std::getline(ss, fi.mtime, delimValue);

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
            }
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

int main(int argc, char **argv)
{
    readFile(argv[1], argv[2]);
    std::cout << "Created: ";
    for (const auto &a : created)
    {
        std::cout << a << std::endl;
    }
    std::cout << "Deleted: ";
    for (const auto &a : deleted)
    {
        std::cout << a << std::endl;
    }
    std::cout << "Altered: " << std::endl;
    for (const auto &a : altered)
    {
        std::cout << a << std::endl;
    }
}