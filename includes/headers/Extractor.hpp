#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <libCZI.h>
#include <CZIReader.h>
#include <filesystem>
#include <ctime>
#include <chrono>
#include <tuple>
#include <functional>
#include <string>
#include <exiv2/exiv2.hpp>
#include "xml2json.hpp"
#include "hal.hpp"
#include "../exiftool/inc/ExifTool.h"
#include "check_if_quotes_needed.hpp"

namespace fs = std::filesystem;

// Source - https://cplusplus.com/reference/functional/hash/
// Retrieved 2026-08-07
std::string getHash(std::string s)
{
    std::hash<std::string> h;
    return std::to_string(h(s));
}

/**
 * The Extractor classes implement different interfaces to extract metadata from given files.
 * The Registry class implements a singleton design pattern, collecting all activated extractors and sorts them by their supported file types.
 */

// Source - https://stackoverflow.com/a/58237530
// Posted by Gulrak, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-11, License - CC BY-SA 4.0
template <typename TP>
std::string time_to_string(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    std::time_t tt = system_clock::to_time_t(sctp);
    std::tm *local_time = std::localtime(&tt);
    std::stringstream buffer;
    // buffer << std::put_time(local_time, "%A, %d %B %Y %H:%M");
    buffer << std::put_time(local_time, "%Y-%m-%dT%H:%M:%S%z");
    return buffer.str();
}

// Source - https://en.cppreference.com/cpp/filesystem/file_status/permissions
// Last accessed: 12. Mai 2026 - 10:30 Uhr
std::string permsToString(std::filesystem::perms p)
{
    std::string result{};
    auto show = [&](char op, fs::perms perm)
    {
        result += (fs::perms::none == (perm & p) ? '-' : op);
    };
    show('r', fs::perms::owner_read);
    show('w', fs::perms::owner_write);
    show('x', fs::perms::owner_exec);
    show('r', fs::perms::group_read);
    show('w', fs::perms::group_write);
    show('x', fs::perms::group_exec);
    show('r', fs::perms::others_read);
    show('w', fs::perms::others_write);
    show('x', fs::perms::others_exec);

    return result;
}

const int get_inode_number(const std::string &filename)
{
    struct stat file_info;
    if (stat(filename.c_str(), &file_info) == -1)
    {
        std::cerr << "Stat Not working: " << filename << std::endl;
        throw std::runtime_error("error using stat.\n");
    }
    return file_info.st_ino;
}

// https://anubhav-gupta62.medium.com/registry-design-pattern-ad4b4c3350e6 05.Mai.2026
// https://refactoring.guru/design-patterns/singleton/cpp/example 05.Mai.2026

class Extractor;

class Registry
{
private:
    static Registry *instance_;
    std::unordered_map<std::string, std::vector<Extractor *>> registryMap;
    Registry() : registryMap() {}

public:
    static Registry *getRegistry()
    {
        if (instance_ == nullptr)
        {
            instance_ = new Registry();
        }
        return instance_;
    }
    void addToRegistry(const std::string &s, Extractor *e)
    {
        registryMap[s].push_back(e);
    }
    void addToRegistry(const std::vector<std::string> &vs, Extractor *e)
    {
        for (const std::string &s : vs)
        {
            registryMap[s].push_back(e);
        }
    }
    std::vector<Extractor *> getExtractors(std::string key)
    {
        if (registryMap.find(key) != registryMap.end())
            return registryMap.at(key);
        else
            return {};
    }
    void printMap()
    {
        for (const auto &[key, value] : registryMap)
        {
            std::cout << key << std::endl;
        }
    }
};
Registry *Registry::instance_ = nullptr;

class Extractor
{
protected:
    std::vector<std::string> supported_types;
    std::string name;
    Extractor(const std::vector<std::string> &vs) : supported_types(vs) {}

public:
    virtual std::string extract(const std::string &path) = 0;
    virtual std::vector<std::tuple<std::string, std::string>> extract(const std::vector<std::string> &paths) = 0;
    virtual void registerExtractor() = 0;
    std::string getName() { return name; }
};

class CZI_Extractor : public Extractor
{
public:
    void registerExtractor()
    {
        Registry *reg = Registry::getRegistry();
        reg->Registry::addToRegistry(supported_types, this);
    }
    std::string extract(const std::string &path) override
    {
        try
        {
            // coversion from Mistral Large 3 675B Instruct 2512 (C++ Inheritance Issues, 06.Mai.2026)
            std::wstring wpath;
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            wpath = converter.from_bytes(path);
            auto stream = libCZI::CreateStreamFromFile(wpath.c_str());
            auto cziReader = libCZI::CreateCZIReader();
            cziReader->Open(stream);

            auto statistics = cziReader->GetStatistics();

            auto mds = cziReader->ReadMetadataSegment();
            auto md = mds->CreateMetaFromMetadataSegment();
            auto docInfo = md->GetDocumentInfo();

            auto metadata = mds->CreateMetaFromMetadataSegment();
            auto xml = metadata->GetXml();

            if (xml.empty())
            {
                throw std::logic_error("Empty metadata string.");
                return "Error";
            }
            std::string result = xml2json(xml.c_str());
            // remove first and last element of string, since the string is surrounded by {}, which makes it impossible to combine results later
            return result.substr(1, result.size() - 2);
        }

        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return "Error";
        }
    }

    std::vector<std::tuple<std::string, std::string>> extract(const std::vector<std::string> &paths)
    {
        try
        {
            std::vector<std::tuple<std::string, std::string>> result{};
            for (const auto &s : paths)
            {
                std::string id = getHash(s);
                // coversion from Mistral Large 3 675B Instruct 2512 (C++ Inheritance Issues, 06.Mai.2026)
                std::wstring wpath;
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                wpath = converter.from_bytes(s);
                auto stream = libCZI::CreateStreamFromFile(wpath.c_str());
                auto cziReader = libCZI::CreateCZIReader();
                cziReader->Open(stream);

                auto statistics = cziReader->GetStatistics();

                auto mds = cziReader->ReadMetadataSegment();
                auto md = mds->CreateMetaFromMetadataSegment();
                auto docInfo = md->GetDocumentInfo();

                auto metadata = mds->CreateMetaFromMetadataSegment();
                auto xml = metadata->GetXml();

                if (xml.empty())
                {
                    throw std::logic_error("Empty metadata string.");
                    return {};
                }

                std::string res = xml2json(xml.c_str());
                result.push_back(std::tuple(id, res.substr(1, res.size() - 2)));
            }
            return result;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return {};
        }
    }

    CZI_Extractor(std::vector<std::string> vs) : Extractor(vs)
    {
        this->name = "CZI";
        registerExtractor();
    }
};

/**
 * Exiftool Extractor inheriting from Extractor implements a call to exiftool to extract metadata from given files.
 */
class Exiftool_Extractor : public Extractor
{
public:
    void registerExtractor()
    {
        Registry *reg = Registry::getRegistry();
        reg->Registry::addToRegistry(supported_types, this);
    }
    std::string extract(const std::string &path) override
    {
        std::string result{};
        ExifTool *et = new ExifTool();
        std::string metadata = "";
        const char *file_path = path.c_str();
        int cmdNum = et->ExtractInfo(file_path, "-File:all\n-s\n-a\n-FileGroupID\n");

        TagInfo *info = et->GetInfo(cmdNum, 1);

        if (info)
        {
            // print returned information
            for (TagInfo *i = info; i; i = i->next)
            {
                std::string val = needsQuotes(std::string(i->value));
                metadata += "\"" + std::string(i->name) + "\": " + val;
                if (i->next)
                    metadata += ",";
                else
                    metadata += "";
            }
            result = metadata;
            delete info;
        }
        else if (et->LastComplete() <= 0)
        {
            std::cerr << path << std::endl;
            std::cerr << ("Error executing exiftool!\n");
            std::cerr << "File might be too large, consider increasing timeout duration.\n";
        }
        // print exiftool stderr messages
        char *err = et->GetError();
        if (err)
        {
            throw std::runtime_error(std::string(err));
        }
        delete et; // delete our ExifTool object
        return result;
    }

    std::vector<std::tuple<std::string, std::string>> extract(const std::vector<std::string> &paths)
    {
        std::vector<std::tuple<std::string, std::string>> results;
        ExifTool *et = new ExifTool();
        for (std::string fp : paths)
        {
            std::string id = getHash(fp);
            std::string metadata = "";
            const char *file_path = fp.c_str();
            int cmdNum = et->ExtractInfo(file_path, "-File:all\n-s\n-a\n-FileGroupID\n");

            TagInfo *info = et->GetInfo(cmdNum, 1);

            if (info)
            {
                // print returned information
                for (TagInfo *i = info; i; i = i->next)
                {
                    std::string val = needsQuotes(std::string(i->value));
                    metadata += "\"" + std::string(i->name) + "\": " + val;
                    if (i->next)
                        metadata += ",";
                    else
                        metadata += "";
                }
                results.push_back(std::tuple(id, metadata));
                delete info;
            }
            else if (et->LastComplete() <= 0)
            {
                std::cout << fp << std::endl
                          << std::endl;
                std::cerr << ("Error executing exiftool!\n");
                std::cerr << "File might be too large, consider increasing timeout duration.\n";
            }
            // print exiftool stderr messages
            char *err = et->GetError();
            if (err)
            {
                throw std::runtime_error(std::string(err));
            }
        }
        delete et; // delete our ExifTool object
        return results;
    }

    Exiftool_Extractor(std::vector<std::string> vs) : Extractor(vs)
    {
        this->name = "Exiftool";
        registerExtractor();
    }
};

class Exiv2_Extractor : public Extractor
{
public:
    void registerExtractor()
    {
        Registry *reg = Registry::getRegistry();
        reg->Registry::addToRegistry(supported_types, this);
    }

    FilesystemInfo_Extractor(std::vector<std::string> vs) : Extractor(vs)
    {
        this->name = "FileInfo";
        registerExtractor();
    }

    template <typename T>
    void read_metadata(const T &data, std::string &md)
    {
        auto end = data.end();
        for (auto i = data.begin(); i != end; ++i)
        {
            md += "\"" + i->key() + "\": ";
            if (i->typeName() == "SHORT" || i->typeName() == "LONG")
            {
                md += i->value();
            }
            else
            {
                md += "\"" + i->value() + "\"";
            }
            if (std::next(i) != end)
            {
                md += ",";
            }
        }
    }

    std::vector<std::tuple<std::string, std::string>>
    extract(const std::vector<std::string> &paths)
    {
        std::vector<std::tuple<std::string, std::string>> results;
        for (const auto &fp : paths)
        {
            // Source - https://exiv2.org/examples.html
            // Accessed: 08.08.2026
            std::string id = getHash(fp);
            Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(fp);
            image->readMetadata();
            if (!image)
            {
                std::cerr << "Cannot open file: " << fp << std::endl;
                continue;
            }

            Exiv2::ExifData &exifData = image->exifData();
            Exiv2::IptcData &iptcData = image->iptcData();
            Exiv2::XmpData &xmpData = image->xmpData();
            std::string md = "";
            if (!exifData.empty())
            {
                read_metadata(exifData, md);
                md += ",";
            }
            if (!iptcData.empty())
            {
                read_metadata(iptcData, md);
                md += ",";
            }
            if (!xmpData.empty())
                read_metadata(xmpData, md);
            md += "\n";
            results.push_back(std::tuple(id, md));
        }
        return results;
    }
};

class FilesystemInfo_Extractor : public Extractor
{
public:
    void registerExtractor()
    {
        Registry *reg = Registry::getRegistry();
        reg->Registry::addToRegistry(supported_types, this);
    }

    FilesystemInfo_Extractor(std::vector<std::string> vs) : Extractor(vs)
    {
        this->name = "FileInfo";
        registerExtractor();
    }

    std::vector<std::tuple<std::string, std::string>> extract(const std::vector<std::string> &paths)
    {
        std::vector<std::tuple<std::string, std::string>> results{};
        for (const fs::path &fp : paths)
        {
            std::string id = getHash(fp);
            results.push_back(std::tuple(id,
                                         "\"FileSize\": " + std::to_string(fs::file_size(fp)) + "," + "\"LastWrite\": \"" + time_to_string(fs::last_write_time(fp)) + "\"," + "\"FileStatus\": \"" + permsToString(fs::status(fp).permissions()) + "\"," + "\"FilePath\": \"" + fp.string() + "\", " + getOwnerInfo(fp)));
        }
        return results;
    }

    std::string extract(const std::string &path)
    {
        std::string results{};
        return "\"FileSize\": " + std::to_string(fs::file_size(path)) + "," + "\"LastWrite\": \"" + time_to_string(fs::last_write_time(path)) + "\"," + "\"FileStatus\": \"" + permsToString(fs::status(path).permissions()) + "\"," + "\"FilePath\": \"" + path + "\"," + getOwnerInfo(path) + "";
    }
};

class ACL_Extractor : public Extractor
{
public:
    void registerExtractor()
    {
        Registry *reg = Registry::getRegistry();
        reg->Registry::addToRegistry(supported_types, this);
    }

    ACL_Extractor(std::vector<std::string> vs) : Extractor(vs)
    {
        this->name = "ACL";
        registerExtractor();
    }
};

#endif