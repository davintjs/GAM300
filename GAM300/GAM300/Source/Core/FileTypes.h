#pragma once

namespace FileType
{
	constexpr static size_t SCRIPT = 0;
	constexpr static size_t MODEL = 1;
	constexpr static size_t TEXTURE = 2;
};

namespace FileState
{
	constexpr static size_t CREATED = 0x00000001;
	constexpr static size_t DELETED = 0x00000002;
	constexpr static size_t MODIFIED = 0x00000003;
	constexpr static size_t RENAMED_OLD = 0x00000004;
	constexpr static size_t RENAMED_NEW = 0x00000005;
}