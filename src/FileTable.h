/////////////////////////////////////////////////////////////////////
/// file: FileTable.h
///
/// summary: File table
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_FILETABLE_H
#define ICENFSD_FILETABLE_H

#define DEFAULT_ROW_SIZE 1024

#include <vector>
#include <string>
#include <list>

#include "FileTree.h"

class FileTable
{
	using TableRow = std::vector<FileTree::Node>;
	using Table = std::list<TableRow>;

public:

	FileTable(uint64_t rowSize = DEFAULT_ROW_SIZE);
	~FileTable();

	bool FileExists(const std::string& path);
	uint64_t GetFileHandle(const std::string& path);
	bool GetFilePath(uint64_t handle, std::string& filePath);
	errno_t RenameFile(const std::string&, const std::string& pathTo);
	errno_t RenameDirectory(const std::string& pathFrom, const std::string& pathTo);
	errno_t RemoveFolder(const std::string& path);
	errno_t RemoveFile(const std::string& path);

	uint64_t GetHandleByPath(const std::string& path);
	bool GetPathByHandle(uint64_t handle, std::string& path);

	bool RemoveItem(const std::string& path);

protected:
	FileTree::Node AddItem(const std::string& path);

private:
	FileTree m_tree;
	Table m_table;
	const uint64_t m_rowSizeLimit;
	uint64_t m_rowPos;

	FileTree::Node GetItemByID(uint64_t id);
};

#endif // ICENFSD_FILETABLE_H
