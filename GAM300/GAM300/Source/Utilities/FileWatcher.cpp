
#include "FileWatcher.h"
//#include "Core/FileTypes.h"
#include <Utilities/MultiThreading.h>
#include <Windows.h>
#include <Core/EventsManager.h>

FileWatcher::FileWatcher()
{
    thread = &THREADS.AddThread(&FileWatcher::ThreadWork, this);
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

void FileWatcher::Quit()
{
    CancelSynchronousIo(thread->native_handle());
}

void FileWatcher::ThreadWork()
{
    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening directory: " << GetLastError() << std::endl;
        return;
    }
    BYTE  fni[32 * 1024];
    DWORD offset = 0;
    TCHAR szFile[MAX_PATH];
    DWORD bytesret;
    PFILE_NOTIFY_INFORMATION pNotify;
    std::filesystem::path oldNameFile;


    while (!THREADS.HasQuit())
    {

        offset = 0;
        memset(fni, 0, 32 * 1024);

        ReadDirectoryChangesW
        (hDir, fni, sizeof(fni), 0,
            FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_FILE_NAME,
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
            FileState fileState = FileState(pNotify->Action);
            std::wstring name{ pNotify->FileName };
            std::filesystem::path filePath{ name };
            //oldNameFile = pNotify->FileName;
            if (filePath.extension() == ".cs")
            {
                PRINT("HARLO");
                FileModifiedEvent<FileType::SCRIPT> fileScript{filePath,fileState};
                EVENT.Publish(&fileScript);
            }

        } while (pNotify->NextEntryOffset != 0);
    }
}

    