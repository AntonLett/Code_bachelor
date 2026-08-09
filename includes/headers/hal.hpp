// Source - https://stackoverflow.com/a/68696701
// Posted by Adrian Mole
// Retrieved 2026-05-12, License - CC BY-SA 4.0
#ifndef hal
#define hal

#include <string>

#if defined(_WIN32)
    // Source: https://www.google.com/search?client=ubuntu-sn&channel=fs&q=does+windows+have+file+ownership&fbs=ADc_l-YGrpJMQtvjQ6h14rj-dfIrH4mwN5r0Z1FZtFNB2w3Upe2HDPC6akWpYUJBWeXXRd0BtTsaeIMiSqrSSe4pv7ADcP9WHmHSnvgEp0DCuDaMFQVNHY0iYeRTPcXSUj7IStmoGS7kkoLRJW_n4Fi7HBwGTum-kk1a0wYDmtR7oYw-l5pJAOGDyQbBKQJYbyF4lFhS7wN2&ved=2ahUKEwiWzbCD-rOUAxWEQfEDHdBBIqsQ0NsOegQIAxAB&aep=10&ntc=1&mstk=AUtExfAbur-QyjW_-0n-lymDlT_1QtStliMRgioNdVFzg2wEUb91TMoYsEjK3kIV2SJGimMXMtgF51oKzVTCRTiWZntJVURXRzSoW3pWRE94Zdz79RJbwmHpI3H5C-apZoj5x5d_eaT7SNBqXMEAJ4yHhKj8PhuBGNY2v6s&csuir=1&udm=50
    // Dieser Codeabschnitt wurde mit Unterstützung von AI auf Google Search (Version 1.5 Pro) 
    // am 12. Mai.2026 erstellt.
    #include <windows.h>
    #include <tchar.h>
    #include <aclapi.h>
    #include <iostream>

    #pragma comment(lib, "advapi32.lib")

    std::string getOwnerInfo(const std::string& path) {
        PSID pSidOwner = NULL;
        PSECURITY_DESCRIPTOR pSD = NULL;

        DWORD result = GetNamedSecurityInfoA(
            path.c_str(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
            &pSidOwner, NULL, NULL, NULL, &pSD
        );

        if (result != ERROR_SUCCESS && result != 5) { // error 5 means access denied -> for performance testing deactivated
            std::cerr << "Error when accessing Security Information: " << result << std::endl;
            return "\"FileOwner\": \"unknown\"";
        }

        // Translate SID to readable name (User, Domain)
        WCHAR acctName[256], domainName[256];
        DWORD dwAcctName = 256, dwDomainName = 256;
        SID_NAME_USE eUse = SidTypeUnknown;

        if (LookupAccountSidW(NULL, pSidOwner, acctName, &dwAcctName, domainName, &dwDomainName, &eUse)) {
            // Type conversions
            int size = WideCharToMultiByte(
                CP_UTF8,
                0,
                acctName,
                -1,
                nullptr,
                0,
                nullptr,
                nullptr
            );
            std::string acct(size - 1, 0);
            WideCharToMultiByte(
                CP_UTF8,
                0,
                acctName,
                -1,
                &acct[0],
                size,
                nullptr,
                nullptr
            );

            if (pSD != NULL) LocalFree(pSD);
            return "\"FileOwner\": \"" + acct + "\"";
        } else {
            std::cerr << "Error in LookupAccountSid: " << GetLastError() << std::endl;
        }

        // Free the allocated descriptor 
        if (pSD != NULL) LocalFree(pSD);
        return "\"FileOwner\": \"unknown\"";
    }
#elif defined(unix)
    #include <pwd.h>
    #include <grp.h>
    #include <sys/stat.h>

    // Source: https://chat-ai.academiccloud.de/chat/9280e65b-6ed3-466b-a5d4-9f50afcdda64
    //ChatAI Mistral Large 3 675N Instruct 2512 - Chat: C++ ACL File Reading - Abgefrage: 12. Mai 2026
    std::string getOwnerInfo(const std::string& path){
        struct stat file_stat;
        if (stat(path.c_str(), &file_stat) != 0) {
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
#else
    #error "Operating system not supported"
#endif

#endif