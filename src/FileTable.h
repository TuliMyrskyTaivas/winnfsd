/////////////////////////////////////////////////////////////////////
/// file: FileTable.h
///
/// summary: File table
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_FILETABLE_H
#define ICENFSD_FILETABLE_H

#define TABLE_SIZE 1024

#include "tree.hh"
#include <string>

struct FileItem
{
	char* path;
	unsigned int pathLen;
	unsigned char* handle;
	bool cached;
};

struct FileTableNode
{
	tree_node_<FileItem>* items[TABLE_SIZE];
	FileTableNode* next;
};

struct CacheList
{
	FileItem* item;
	CacheList* next;
};

class FileTable
{
public:
	FileTable();
	~FileTable();

	unsigned long GetIDByPath(const char* path);
	unsigned char* GetHandleByPath(const char* path);
	bool GetPathByHandle(unsigned char* handle, std::string& path);
	tree_node_<FileItem>* FindItemByPath(const char* path);
	bool RemoveItem(const char* path);
	void RenameFile(const char* pathFrom, const char* pathTo);

protected:
	tree_node_<FileItem>* AddItem(const char* path);

private:
	FileTableNode* m_firstTable, * m_lastTable;
	unsigned int m_tableSize;
	CacheList* m_cacheList;

	tree_node_<FileItem>* GetItemByID(unsigned int id);
	void PutItemInCache(FileItem* item);
};

extern bool FileExists(const std::string& path);
extern unsigned long GetFileID(const std::string& path);
extern unsigned char* GetFileHandle(const std::string& path);
extern bool GetFilePath(unsigned char* handle, std::string& filePath);
extern int RenameFile(const std::string&, const std::string& pathTo);
extern int RenameDirectory(const std::string& pathFrom, const std::string& pathTo);
extern int RemoveFolder(const std::string& path);
extern bool RemoveFile(const std::string& path);
#endif // ICENFSD_FILETABLE_H
