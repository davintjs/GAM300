/*!***************************************************************************************
\file			FileWatcher.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/09/2023

\brief
    This file defines a file watcher than runs on another thread

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "FileWatcher.h"
#include <Utilities/ThreadPool.h>
#include <Windows.h>
#include <Core/EventsManager.h>
#include <unordered_set>
#include <string>

#include <Core/Events.h>

FileWatcher::FileWatcher()
{
    THREADS.EnqueueTask([this] {ThreadWork(); });
    hDir = CreateFile(
        std::wstring(L"ASSETS").c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );
}

FileWatcher::~FileWatcher()
{
    CloseHandle(hDir);
}

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        // Combine the hashes of the first and second elements
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

// Define a custom equality comparator for std::pair<std::wstring, FileState>
struct PairEqual {
    template <class T1, class T2>
    bool operator()(const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};


void FileWatcher::ThreadWork()
{
    BYTE  fni[32 * 1024];
    DWORD offset = 0;
    TCHAR szFile[MAX_PATH];
    DWORD bytesret;
    PFILE_NOTIFY_INFORMATION pNotify;
    std::unordered_set<std::pair<std::wstring,size_t>, PairHash, PairEqual> events;


    while (!THREADS.HasStopped())
    {
        offset = 0;
        memset(fni, 0, 32 * 1024);

        ReadDirectoryChangesW
        (hDir, fni, sizeof(fni), true,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_CREATION,
            &bytesret, NULL, NULL
        );
        do
        {
            pNotify = (PFILE_NOTIFY_INFORMATION)&fni[offset];
            offset += pNotify->NextEntryOffset;


            #if defined(UNICODE)
            {
                lstrcpynW(szFile, pNotify->FileName,
                    min(MAX_PATH, pNotify->FileNameLength / sizeof(WCHAR) + 1));
            }
            #else
            {
                int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName,
                    pNotify->FileNameLength / sizeof(WCHAR),
                    szFile, MAX_PATH - 1, NULL, NULL);
                szFile[count] = TEXT('\0');
            }
            #endif
            events.insert( std::make_pair(std::wstring(szFile), pNotify->Action));
        } while (pNotify->NextEntryOffset != 0);

        for (auto& pair : events)
        {
            FileModifiedEvent fileEvent(pair.first.c_str(), pair.second);
            EVENTS.Publish(&fileEvent);
        }
        events.clear();
    }
}

    