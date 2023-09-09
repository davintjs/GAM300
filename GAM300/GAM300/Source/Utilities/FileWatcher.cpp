
#include "FileWatcher.h"
#include <Utilities/ThreadPool.h>
#include <Windows.h>
#include <Core/EventsManager.h>

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

void FileWatcher::ThreadWork()
{
    BYTE  fni[32 * 1024];
    DWORD offset = 0;
    TCHAR szFile[MAX_PATH];
    DWORD bytesret;
    PFILE_NOTIFY_INFORMATION pNotify;

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
            FileModifiedEvent fileEvent(pNotify->FileName,pNotify->Action);
            EVENTS.Publish(&fileEvent);

        } while (pNotify->NextEntryOffset != 0);
    }
}

    