/*!***************************************************************************************
\file			FileTypes.h
\project
\author			Zacharie Hong
\co-authors

\par			Course: GAM300
\par			Section:
\date			05/09/2023

\brief
	Contains declarations for File enums

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

namespace FileType
{
	enum FILETYPE
	{
		SCRIPT,
		MODEL,
		TEXTURE
	};
}

namespace FileState
{
	enum FILESTATE
	{
		CREATED = 0x00000001,
		DELETED = 0x00000002,
		MODIFIED = 0x00000003,
		RENAMED_OLD = 0x00000004,
		RENAMED_NEW = 0x00000005
	};
}