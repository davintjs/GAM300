
#include "FileWatcher.h"
//#include "Core/FileTypes.h"
#include <Utilities/MultiThreading.h>
#include <Windows.h>

FileWatcher::FileWatcher()
{
    THREADS.AddThread(&FileWatcher::ThreadWork, this);
    PRINT("FILE WATCHER OK",'\n');
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
    CancelIo(hDir);
    CloseHandle(hDir);
}

void FileWatcher::ThreadWork()
{    wchar_t filename[MAX_PATH];
    char buffer[4096];

    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening directory: " << GetLastError() << std::endl;
        return;
    }
    BYTE  fni[32 * 1024];
    DWORD offset = 0;
    TCHAR szFile[MAX_PATH];
    DWORD bytesret;
    PFILE_NOTIFY_INFORMATION pNotify;

    while (!THREADS.HasQuit())
    {
        
        //    FileState fileState{};
        //    switch (pNotify->Action)
        //    {
        //    case FILE_ACTION_MODIFIED:
        //        fileState = FileState::MODIFIED;
        //        break;
        //    case FILE_ACTION_ADDED:
        //        fileState = FileState::CREATED;
        //        break;
        //    case FILE_ACTION_REMOVED:
        //        fileState = FileState::DELETED;
        //        break;
        //    case FILE_ACTION_RENAMED_OLD_NAME:
        //        PRINT("OLD NAME: ");
        //        break;
        //    case FILE_ACTION_RENAMED_NEW_NAME:
        //        PRINT("NEW NAME: ");
        //        break;
        //    }
        //    std::wcout << pNotify->FileName << std::endl;
        //} while (pNotify->NextEntryOffset != 0);
        //offset = 0;
        //memset(fni, 0, 32 * 1024);

        //ReadDirectoryChangesW
        //(hDir, fni, sizeof(fni), 0,
        //    FILE_NOTIFY_CHANGE_CREATION |
        //    FILE_NOTIFY_CHANGE_SIZE |
        //    FILE_NOTIFY_CHANGE_FILE_NAME,
        //    &bytesret, NULL, NULL
        //);
        //do
        //{
        //    pNotify = (PFILE_NOTIFY_INFORMATION)&fni[offset];
        //    offset += pNotify->NextEntryOffset;

        //    #if defined(UNICODE)
        //    {
        //        lstrcpynW(szFile, pNotify->FileName,
        //            min(MAX_PATH, pNotify->FileNameLength / sizeof(WCHAR) + 1));
        //        std::wcout << pNotify->FileName << std::endl;
        //    }
        //    #else
        //    {
        //        int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName,
        //            pNotify->FileNameLength / sizeof(WCHAR),
        //            szFile, MAX_PATH - 1, NULL, NULL);
        //        szFile[count] = TEXT('\0');
        //    }
        //    #endif

        //} while (pNotify->NextEntryOffset != 0);
    }
}