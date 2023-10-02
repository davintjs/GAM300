/*!***************************************************************************************
\file			FileWatcher.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/09/2023

\brief
    This file declares a file watcher than runs on another thread

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef FILEWATCHER_H
#define FILEWATCHER_H

class FileWatcher
{
public:
    //Constructor, open file handle
    FileWatcher();

    //Thread that uses WIN API to check for file updates
    void ThreadWork();

    //Constructor, closes file handle
    ~FileWatcher();

    void* hDir;
};

#endif