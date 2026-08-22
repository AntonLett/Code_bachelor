// Source - https://stackoverflow.com/a/68696701
// Posted by Adrian Mole
// Retrieved 2026-05-12, License - CC BY-SA 4.0
#ifndef hal
#define hal

#include <string>
#include <functional>

// Source - https://cplusplus.com/reference/functional/hash/
// Retrieved 2026-08-07
std::string getHash(std::string s)
{
    std::hash<std::string> h;
    return std::to_string(h(s));
}

#if defined(_WIN32)
// Source: https://www.google.com/search?client=ubuntu-sn&channel=fs&q=does+windows+have+file+ownership&fbs=ADc_l-YGrpJMQtvjQ6h14rj-dfIrH4mwN5r0Z1FZtFNB2w3Upe2HDPC6akWpYUJBWeXXRd0BtTsaeIMiSqrSSe4pv7ADcP9WHmHSnvgEp0DCuDaMFQVNHY0iYeRTPcXSUj7IStmoGS7kkoLRJW_n4Fi7HBwGTum-kk1a0wYDmtR7oYw-l5pJAOGDyQbBKQJYbyF4lFhS7wN2&ved=2ahUKEwiWzbCD-rOUAxWEQfEDHdBBIqsQ0NsOegQIAxAB&aep=10&ntc=1&mstk=AUtExfAbur-QyjW_-0n-lymDlT_1QtStliMRgioNdVFzg2wEUb91TMoYsEjK3kIV2SJGimMXMtgF51oKzVTCRTiWZntJVURXRzSoW3pWRE94Zdz79RJbwmHpI3H5C-apZoj5x5d_eaT7SNBqXMEAJ4yHhKj8PhuBGNY2v6s&csuir=1&udm=50
// Dieser Codeabschnitt wurde mit Unterstützung von AI auf Google Search (Version 1.5 Pro)
// am 12. Mai.2026 erstellt.
#include <windows.h>
#include <tchar.h>
#include <aclapi.h>
#include <iostream>
#include <exiv2/exiv2.hpp>

#pragma comment(lib, "advapi32.lib")

std::string getOwnerInfo(const std::string &path)
{
    PSID pSidOwner = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;

    DWORD result = GetNamedSecurityInfoA(
        path.c_str(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &pSidOwner, NULL, NULL, NULL, &pSD);

    if (result != ERROR_SUCCESS)
    {
        std::cerr << "Error when accessing Security Information: " << result << std::endl;
        return "\"FileOwner\": \"unknown\"";
    }

    // Translate SID to readable name (User, Domain)
    WCHAR acctName[256], domainName[256];
    DWORD dwAcctName = 256, dwDomainName = 256;
    SID_NAME_USE eUse = SidTypeUnknown;

    if (LookupAccountSidW(NULL, pSidOwner, acctName, &dwAcctName, domainName, &dwDomainName, &eUse))
    {
        // Type conversions
        int size = WideCharToMultiByte(
            CP_UTF8,
            0,
            acctName,
            -1,
            nullptr,
            0,
            nullptr,
            nullptr);
        std::string acct(size - 1, 0);
        WideCharToMultiByte(
            CP_UTF8,
            0,
            acctName,
            -1,
            &acct[0],
            size,
            nullptr,
            nullptr);

        if (pSD != NULL)
            LocalFree(pSD);
        return "\"FileOwner\": \"" + acct + "\"";
    }
    else
    {
        std::cerr << "Error in LookupAccountSid: " << GetLastError() << std::endl;
    }

    // Free the allocated descriptor
    if (pSD != NULL)
        LocalFree(pSD);
    return "\"FileOwner\": \"unknown\"";
}

std::vector<std::tuple<std::string, std::string>> extractExif(const std::vector<std::string> &paths)
{
    std::vector<std::tuple<std::string, std::string>> results;
    for (const auto &fp : paths)
    {
        // Source - https://exiv2.org/examples.html
        // Accessed: 08.08.2026
        std::string id = getHash(fp);
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(fp);
        if (!image)
        {
            std::cerr << "Cannot open file: " << fp << std::endl;
            continue;
        }
        image->readMetadata();

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

std::string extractExif(const std::string &path)
{
    // Source - https://exiv2.org/examples.html
    // Accessed: 08.08.2026
    std::string id = getHash(path);
    Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
    if (!image)
    {
        std::cerr << "Cannot open file: " << path << std::endl;
        return "";
    }
    image->readMetadata();

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
    return md;
}

#elif defined(unix)
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include "../exiftool/inc/ExifTool.h"

// Source: https://chat-ai.academiccloud.de/chat/9280e65b-6ed3-466b-a5d4-9f50afcdda64
// ChatAI Mistral Large 3 675N Instruct 2512 - Chat: C++ ACL File Reading - Abgefrage: 12. Mai 2026
std::string getOwnerInfo(const std::string &path)
{
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) != 0)
    {
        perror("Stat failed in getOwnerInfo");
        return "\"FileOwner\": \"unknown\"";
    }

    // Get name of File Owner
    struct passwd *owner = getpwuid(file_stat.st_uid);
    std::string result = "\"FileOwner\": \"";
    result += (owner ? owner->pw_name : "unknown");
    result += "\",";

    // Get name of Group the File belongs to
    struct group *group = getgrgid(file_stat.st_gid);
    result += "\"Group\": \"";
    result += (group ? group->gr_name : "unknown");
    result += "\"";

    return result;
}

std::vector<std::tuple<std::string, std::string>> extractExif(const std::vector<std::string> &paths)
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

std::string extractExif(const std::string &path)
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
#else
#error "Operating system not supported"
#endif

#endif