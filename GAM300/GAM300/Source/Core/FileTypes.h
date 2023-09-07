#pragma once

enum class FileType
{
	SCRIPT,
	MODEL,
	TEXTURE
};

enum class FileState : size_t
{
	CREATED = 0x00000001,
	DELETED = 0x00000002,
	MODIFIED = 0x00000003,
	RENAMED_OLD = 0x00000004,
	RENAMED_NEW = 0x00000005,
	NONE
};