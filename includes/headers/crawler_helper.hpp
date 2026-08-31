#if define(_WIN32)
#include <string>
#include <functional>

#include <windows.h>
#include <tchar.h>
#include <aclapi.h>
#include <iostream>
#include <exiv2/exiv2.hpp>
#include <sddl.h>

#pragma comment(lib, "advapi32.lib")

struct OwnerGroup
{
    std::string owner;
    std::string group;
};

std::string sidToString(PSID sid)
{
    if (!sid || !IsValidSid(sid))
        return "unknown";

    LPSTR sidString = nullptr;

    if (!ConvertSidToStringSidA(sid, &sidString))
        return "unknown";

    std::string result(sidString);
    LocalFree(sidString);

    return result;
}

OwnerGroup getOwnerAndGroup(const std::string &path)
{
    PSID pSidOwner = nullptr;
    PSID pSidGroup = nullptr;
    PSECURITY_DESCRIPTOR pSD = nullptr;

    DWORD result = GetNamedSecurityInfoA(
        path.c_str(),
        SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
        &pSidOwner,
        &pSidGroup,
        nullptr,
        nullptr,
        &pSD);

    if (result != ERROR_SUCCESS)
    {
        return {"unknown", "unknown"};
    }

    OwnerGroup resultStrings{
        sidToString(pSidOwner),
        sidToString(pSidGroup)};

    if (pSD)
        LocalFree(pSD);

    return resultStrings;
}
#endif