
#include "FileWatcher.h"
//#include "Core/FileTypes.h"
#include <Windows.h>
#include <Utilities/MultiThreading.h>

FileWatcher::FileWatcher()
{
    THREADS.AddThread(&FileWatcher::ThreadWork, this);
    PRINT("FILE WATCHER OK",'\n');
}

void FileWatcher::ThreadWork()
{
    HANDLE hDir = CreateFile(
        std::wstring(L"ASSETS").c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    wchar_t filename[MAX_PATH];
    FILE_NOTIFY_INFORMATION buffer[1024];
    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening directory: " << GetLastError() << std::endl;
        return;
    }
    while (!THREADS.HasQuit()) /*&& 
*/
    {
        //OVERLAPPED overlapped = { 0 };

        //BOOL success =
        //    ReadDirectoryChangesW
        //    (
        //        hDir,
        //        buffer,
        //        sizeof(buffer),
        //        TRUE,
        //        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
        //        FILE_NOTIFY_CHANGE_LAST_WRITE,
        //        nullptr,
        //        &overlapped,
        //        nullptr
        //    );

        //if (!success) {
        //    // Handle the error
        //    std::cerr << "Failed to start directory monitoring: " << GetLastError() << std::endl;
        //    CloseHandle(hDir);
        //    return;
        //}

        //int offset = 0;
        //FILE_NOTIFY_INFORMATION* pNotify;
        //pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buffer + offset);
        //wcscpy_s(filename, L"");

        //wcsncpy_s(filename, pNotify->FileName, pNotify->FileNameLength / 2);

        //filename[pNotify->FileNameLength / 2] = NULL;


        //switch (buffer[0].Action)
        //{
        //case FILE_ACTION_MODIFIED:
        //    PRINT("MODIFIED: ");
        //    break;
        //case FILE_ACTION_ADDED:
        //    PRINT("ADDED: ");
        //    break;
        //case FILE_ACTION_REMOVED:
        //    PRINT("REMOVED: ");
        //    break;
        //case FILE_ACTION_RENAMED_OLD_NAME:
        //    PRINT("OLD NAME: ");
        //    break;
        //case FILE_ACTION_RENAMED_NEW_NAME:
        //    PRINT("NEW NAME: ");
        //    break;
        //}
        //std::wcout << filename << std::endl;
        //if (!CancelIoEx(hDir, &overlapped)) {
        //    // Handle the error if CancelIoEx fails
        //    std::cerr << "Failed to cancel directory monitoring: " << GetLastError() << std::endl;
        //}
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    CloseHandle(hDir);

}