/////////////////////////////////////////////////////////////////////
/// file: FileTable.cpp
///
/// summary: File table
/////////////////////////////////////////////////////////////////////

#include "FileTable.h"
#include "FileTree.h"
#include <boost/log/trivial.hpp>
#include <array>
#include <cstring>
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#include "tree.hh"

#define NFS3_FHSIZE 64

/////////////////////////////////////////////////////////////////////
static std::string GetLastSystemError()
{
	const size_t bufSize = 1024;
	std::array<char, bufSize> buf;
	strerror_s(buf.data(), bufSize, errno);
	return std::string{ buf.data() };
}

/////////////////////////////////////////////////////////////////////
FileTable::FileTable(uint64_t rowSize)
	: m_rowSizeLimit(rowSize)
	, m_rowPos(0)
{
	// Add first row
	m_table.emplace_back(TableRow{ m_rowSizeLimit });
}

/////////////////////////////////////////////////////////////////////
FileTable::~FileTable()
{}

/////////////////////////////////////////////////////////////////////
uint64_t FileTable::GetHandleByPath(const std::string& path)
{
	auto node = m_tree.FindFileItemForPath(path);
	if (node == nullptr)
	{
		node = AddItem(path);
	}

	return node->data.handle;
}

/////////////////////////////////////////////////////////////////////
bool FileTable::GetPathByHandle(uint64_t handle, std::string& path)
{
	auto node = GetItemByID(handle);
	if (node != nullptr)
	{
		m_tree.GetNodeFullPath(node, path);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTable::AddItem(const std::string& path)
{
	// Add new table row if needed
	if (m_rowPos > 0 && (m_rowPos & (m_rowSizeLimit - 1)) == 0)
	{
		m_table.emplace_back(TableRow{ m_rowSizeLimit });
	}

	const uint64_t handle = m_rowPos; // handle in equal the index of the table
	auto node = m_tree.AddItem(path, handle);
	auto& lastRow = m_table.back();

	lastRow[m_rowPos & (m_rowSizeLimit - 1)] = node;  //add the new item in the file table
	++m_rowPos;

	return node;  //return the pointer to the new item
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTable::GetItemByID(uint64_t id)
{
	if (id >= m_rowPos)
	{
		return nullptr;
	}

	uint64_t i = m_rowSizeLimit;
	auto row = m_table.cbegin();
	for (; i <= id; i += m_rowSizeLimit)
	{
		++row;
	}

	return (*row)[id + m_rowSizeLimit - i];
}

/////////////////////////////////////////////////////////////////////
bool FileTable::RemoveItem(const std::string& path)
{
	auto foundDeletedItem = m_tree.FindFileItemForPath(path);
	if (foundDeletedItem != nullptr)
	{
		// Remove from table
		const uint64_t handle = foundDeletedItem->data.handle;

		if (handle >= m_rowPos)
		{
			return false;
		}

		auto row = m_table.begin();
		uint64_t i = m_rowSizeLimit;
		for (; i <= handle; i += m_rowSizeLimit)
		{
			++row;
		}

		(*row)[handle + m_rowSizeLimit - i] = nullptr;

		// Remove from table end
		std::string fullPath;
		m_tree.GetNodeFullPath(foundDeletedItem, fullPath);
		m_tree.RemoveItem(fullPath);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
bool FileTable::FileExists(const std::string& path)
{
	struct _finddata_t fileinfo;

	auto handle = _findfirst(path.c_str(), &fileinfo);
	_findclose(handle);

	return handle == -1 ? false : strcmp(fileinfo.name, strrchr(path.c_str(), '\\') + 1) == 0;  //filename must match case
}

/////////////////////////////////////////////////////////////////////
uint64_t FileTable::GetFileHandle(const std::string& path)
{
	return GetHandleByPath(path);
}

/////////////////////////////////////////////////////////////////////
bool FileTable::GetFilePath(uint64_t handle, std::string& filePath)
{
	return GetPathByHandle(handle, filePath);
}

/////////////////////////////////////////////////////////////////////
errno_t FileTable::RenameFile(const std::string& pathFrom, const std::string& pathTo)
{
	auto node = m_tree.FindFileItemForPath(pathFrom);
	if (node == nullptr)
	{
		BOOST_LOG_TRIVIAL(error) << "trying to rename non existing file: " << pathFrom;
		return ENOENT;
	}

	if (0 != rename(pathFrom.c_str(), pathTo.c_str()))
	{
		BOOST_LOG_TRIVIAL(error) << "failed to rename file " << pathFrom << ": " << GetLastSystemError();
		return errno;
	}

	m_tree.RenameItem(pathFrom, pathTo);
	BOOST_LOG_TRIVIAL(debug) << "path " << pathFrom << " renamed to " << pathTo;
	return 0;
}

/////////////////////////////////////////////////////////////////////
errno_t FileTable::RenameDirectory(const std::string& pathFrom, const std::string& pathTo)
{
	auto err = RenameFile(pathFrom, pathTo);
	if (0 != err)
	{
		return err;
	}

	const std::string dotFile = "\\.";
	const std::string backFile = "\\..";
	const std::string dotDirectoryPathFrom = pathFrom + dotFile;
	const std::string dotDirectoryPathTo = pathTo + dotFile;
	const std::string backDirectoryPathFrom = pathFrom + backFile;
	const std::string backDirectoryPathTo = pathTo + backFile;

	err = RenameFile(dotDirectoryPathFrom, dotDirectoryPathTo);
	if (0 != err)
	{
		return err;
	}
	return RenameFile(backDirectoryPathFrom, backDirectoryPathTo);
}

/////////////////////////////////////////////////////////////////////
errno_t FileTable::RemoveFile(const std::string& path)
{
	if (0 != _chmod(path.c_str(), S_IREAD | S_IWRITE))
	{
		BOOST_LOG_TRIVIAL(error) << "failed to set RW permissions on file " << path << ": " << GetLastSystemError();
		return errno;
	}
	if (0 != remove(path.c_str()))
	{
		BOOST_LOG_TRIVIAL(error) << "failed to remove file " << path << ": " << GetLastSystemError();
		return errno;
	}
	RemoveItem(path);
	return 0;
}

/////////////////////////////////////////////////////////////////////
errno_t FileTable::RemoveFolder(const std::string& path)
{
	if (0 != _chmod(path.c_str(), S_IREAD | S_IWRITE))
	{
		BOOST_LOG_TRIVIAL(error) << "failed to set RW permissions on directory " << path << ": " << GetLastSystemError();
		return errno;
	}

	auto err = RemoveFile(path);
	if (0 != err)
	{
		return err;
	}
	const std::string dotFile = "\\.";
	const std::string backFile = "\\..";
	const std::string dotDirectoryPath = path + dotFile;
	const std::string backDirectoryPath = path + backFile;

	RemoveItem(dotDirectoryPath);
	RemoveItem(backDirectoryPath);
	RemoveItem(path);
	return 0;
}