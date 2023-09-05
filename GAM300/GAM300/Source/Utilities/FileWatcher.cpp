
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
    FILE_NOTIFY_INFORMATION buffer[1024];
    OVERLAPPED overlapped = { 0 };

    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening directory: " << GetLastError() << std::endl;
        return;
    }
    while (!THREADS.HasQuit())
    {
       /* bool success = ReadDirectoryChangesW
        (
            hDir,
            buffer,
            sizeof(buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_SECURITY |
            FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_LAST_ACCESS |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_FILE_NAME,
            nullptr,
            &overlapped,
            nullptr
        );

        if (!success)
        {
            PRINT("FAILED TO DETECT CHANGES\n");
            continue;
        }

        FILE_NOTIFY_INFORMATION* start = buffer;

        while (start && start->NextEntryOffset != 0) 
        {
            FileState fileState{};
            switch (start->Action)
            {
            case FILE_ACTION_MODIFIED:
                fileState = FileState::MODIFIED;
                break;
            case FILE_ACTION_ADDED:
                fileState = FileState::CREATED;
                break;
            case FILE_ACTION_REMOVED:
                fileState = FileState::DELETED;
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                PRINT("OLD NAME: ");
                break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                PRINT("NEW NAME: ");
                break;
            }
            std::wcout << start->FileName << std::endl;
            start = (FILE_NOTIFY_INFORMATION*)((char*)start + start->NextEntryOffset);
        }*/
    }
}