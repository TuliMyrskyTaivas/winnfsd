/////////////////////////////////////////////////////////////////////
/// file: FileTable.cpp
///
/// summary: File table
/////////////////////////////////////////////////////////////////////

#include "FileTable.h"
#include "FileTree.h"
#include <cstring>
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#include "tree.hh"

#define FHSIZE 32
#define NFS3_FHSIZE 64

static FileTable g_FileTable;
static FileTree g_FileTree;

/////////////////////////////////////////////////////////////////////
FileTable::FileTable()
	: m_firstTable(new FileTableNode)
	, m_lastTable(m_firstTable)
	, m_tableSize(0)
	, m_cacheList(nullptr)
{
	memset(m_firstTable, 0, sizeof(FileTableNode));
}

/////////////////////////////////////////////////////////////////////
FileTable::~FileTable()
{
	auto table = m_firstTable;
	while (table != nullptr)
	{
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			if (!table->items[i])
			{
				delete[] table->items[i];
			}
		}

		auto temp = table;
		table = table->next;
		delete temp;
	}

	while (m_cacheList != nullptr)
	{
		auto pPrev = m_cacheList;
		m_cacheList = m_cacheList->next;
		delete pPrev;
	}
}

/////////////////////////////////////////////////////////////////////
unsigned long FileTable::GetIDByPath(const char* path)
{
	const unsigned char* handle = GetHandleByPath(path);
	return handle == nullptr ? 0 : *(unsigned long*)handle;
}

/////////////////////////////////////////////////////////////////////
unsigned char* FileTable::GetHandleByPath(const char* path)
{
	tree_node_<FileItem>* node = g_FileTree.FindFileItemForPath(path);
	if (node == nullptr)
	{
		AddItem(path);
		node = g_FileTree.FindFileItemForPath(path);
	}

	return node == nullptr ? nullptr : node->data.handle;
}

/////////////////////////////////////////////////////////////////////
bool FileTable::GetPathByHandle(unsigned char* handle, std::string& path)
{
	const unsigned id = *(unsigned int*)handle;
	tree_node_<FileItem>* node = GetItemByID(id);
	if (node != nullptr)
	{
		g_FileTree.GetNodeFullPath(node, path);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTable::FindItemByPath(const char* /*path*/)
{
	tree_node_<FileItem>* node = NULL;/*
	CACHE_LIST *pCurr;

	unsigned int i, j, nPathLen;
	FILE_TABLE *pTable;

	nPathLen = strlen(path);
	pItem = NULL;

	pCurr = m_pCacheList;

	while (pCurr != NULL) { //search in cache
		if (nPathLen == pCurr->pItem->nPathLen) { //comparing path length is faster than comparing path
			if (strcmp(path, pCurr->pItem->path) == 0) { //compare path
				pItem = pCurr->pItem;  //path matched
				break;
			}
		}

		pCurr = pCurr->pNext;
	}

	if (pItem == NULL) { //not found in cache
		pTable = m_pFirstTable;

		for (i = 0; i < m_nTableSize; i += TABLE_SIZE) { //search in file table
			for (j = 0; j < TABLE_SIZE; j++) {
				if (i + j >= m_nTableSize) { //all items in file table are compared
					return NULL;
				}

				if (nPathLen == pTable->pItems[j].nPathLen) { //comparing path length is faster than comparing path
					if (strcmp(path, pTable->pItems[j].path) == 0) { //compare path
						pItem = pTable->pItems + j;  //path matched
						break;
					}
				}
			}

			if (pItem != NULL) {
				break;
			}

			pTable = pTable->pNext;
		}
	}

	if (pItem != NULL) {
		//TODO IMPLEMENTED CACHE RIGHT
		//PutItemInCache(pItem);  //put the found item in cache
	}
	*/
	return node;
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTable::AddItem(const char* path)
{
	FileItem item;

	item.path = new char[strlen(path) + 1];
	strcpy_s(item.path, (strlen(path) + 1), path);  //path
	item.pathLen = static_cast<int>(strlen(item.path));  //path length
	item.handle = new unsigned char[NFS3_FHSIZE];
	memset(item.handle, 0, NFS3_FHSIZE * sizeof(unsigned char));
	*(unsigned int*)item.handle = m_tableSize;  //let its handle equal the index
	item.cached = false;  //not in the cache

	if (m_tableSize > 0 && (m_tableSize & (TABLE_SIZE - 1)) == 0) {
		m_lastTable->next = new FileTableNode;
		m_lastTable = m_lastTable->next;
		memset(m_lastTable, 0, sizeof(FileTableNode));
	}

	g_FileTree.AddItem(path, item.handle);
	tree_node_<FileItem>* node = g_FileTree.FindFileItemForPath(path);

	m_lastTable->items[m_tableSize & (TABLE_SIZE - 1)] = node;  //add the new item in the file table
	++m_tableSize;

	return node;  //return the pointer to the new item
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTable::GetItemByID(unsigned int id)
{
	if (id >= m_tableSize) {
		return nullptr;
	}

	unsigned int i;
	FileTableNode* table = m_firstTable;
	for (i = TABLE_SIZE; i <= id; i += TABLE_SIZE) {
		table = table->next;
	}

	return table->items[id + TABLE_SIZE - i];
}

/////////////////////////////////////////////////////////////////////
void FileTable::PutItemInCache(FileItem* item)
{
	CacheList* prev = nullptr;
	CacheList* curr = m_cacheList;

	if (item->cached) // item is already in the cache
	{
		while (curr != nullptr)
		{
			if (item == curr->item)
			{
				if (curr == m_cacheList) // at the first
				{
					return;
				}
				else {  //move to the first
					prev->next = curr->next;
					curr->next = m_cacheList;
					m_cacheList = curr;
					return;
				}
			}

			prev = curr;
			curr = curr->next;
		}
	}
	else {
		item->cached = true;

		int count;
		for (count = 0; count < 9 && curr != NULL; count++) { //seek to the end of the cache
			prev = curr;
			curr = curr->next;
		}

		if (count == 9 && curr != NULL) { //there are 10 items in the cache
			prev->next = NULL;  //remove the last
			curr->item->cached = false;
		}
		else {
			curr = new CacheList;
		}

		curr->item = item;
		curr->next = m_cacheList;
		m_cacheList = curr;  //insert to the first
	}
}

/////////////////////////////////////////////////////////////////////
bool FileTable::RemoveItem(const char* path) {
	/* CACHE_LIST *pCurr;
	 FILE_ITEM *pItem;
	 unsigned int i, j, nPathLen;
	 FILE_TABLE *pTable;
	 int pItemIndex;

	 nPathLen = strlen(path);
	 pItem = NULL;

	 bool foundDeletedItem = false;

	 pTable = m_pFirstTable;

	 for (i = 0; i < m_nTableSize; i += TABLE_SIZE) { //search in file table
		 for (j = 0; j < TABLE_SIZE; j++) {
			 if (i + j >= m_nTableSize) { //all items in file table are compared
				 break;
			 }

			 if (!foundDeletedItem)
			 {
				 if (nPathLen == pTable->pItems[j].nPathLen) { //comparing path length is faster than comparing path
					 if (strcmp(path, pTable->pItems[j].path) == 0) { //compare path
						 foundDeletedItem = true;
						 memset(&(pTable->pItems[j]), 0, sizeof(FILE_ITEM));
					 }
				 }
			 }
		 }

		 pTable = pTable->pNext;
	 }

	 if (foundDeletedItem) {
		 // we should not uncrement table size, because new file handle base on it
		 //--m_nTableSize;
	 }*/
	tree_node_<FileItem>* foundDeletedItem;
	foundDeletedItem = g_FileTree.FindFileItemForPath(path);
	if (foundDeletedItem != nullptr)
	{
		// Remove from table
		unsigned int handle = *(unsigned int*)foundDeletedItem->data.handle;

		if (handle >= m_tableSize)
		{
			return false;
		}
		else
		{
			FileTableNode* pTable = m_firstTable;

			unsigned int i;
			for (i = TABLE_SIZE; i <= handle; i += TABLE_SIZE)
			{
				pTable = pTable->next;
			}

			pTable->items[handle + TABLE_SIZE - i] = nullptr;
		}

		// Remove from table end
		std::string fullPath;
		g_FileTree.GetNodeFullPath(foundDeletedItem, fullPath);
		g_FileTree.RemoveItem(fullPath.c_str());
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
void FileTable::RenameFile(const char* pathFrom, const char* pathTo)
{
	g_FileTree.RenameItem(pathFrom, pathTo);
}

/////////////////////////////////////////////////////////////////////
bool FileExists(const char* path)
{
	struct _finddata_t fileinfo;

	auto handle = _findfirst(path, &fileinfo);
	_findclose(handle);

	return handle == -1 ? false : strcmp(fileinfo.name, strrchr(path, '\\') + 1) == 0;  //filename must match case
}

/////////////////////////////////////////////////////////////////////
unsigned long GetFileID(const char* path)
{
	return g_FileTable.GetIDByPath(path);
}

/////////////////////////////////////////////////////////////////////
unsigned char* GetFileHandle(const char* path)
{
	return g_FileTable.GetHandleByPath(path);
}

/////////////////////////////////////////////////////////////////////
bool GetFilePath(unsigned char* handle, std::string& filePath)
{
	return g_FileTable.GetPathByHandle(handle, filePath);
}

/////////////////////////////////////////////////////////////////////
int RenameFile(const char* pathFrom, const char* pathTo)
{
	tree_node_<FileItem>* node = g_FileTable.FindItemByPath(pathFrom);
	FileItem* item = &(node->data);

	if (item == nullptr)
	{
		return false; // WHAT?
	}

	errno_t errorNumber = rename(pathFrom, pathTo);
	if (errorNumber == 0)
	{
		g_FileTable.RenameFile(pathFrom, pathTo);
	}
	return errorNumber;
}

/////////////////////////////////////////////////////////////////////
int RenameDirectory(const char* pathFrom, const char* pathTo)
{
	errno_t errorNumber = RenameFile(pathFrom, pathTo);

	const std::string dotFile = "\\.";
	const std::string backFile = "\\..";
	const std::string dotDirectoryPathFrom = pathFrom + dotFile;
	const std::string dotDirectoryPathTo = pathTo + dotFile;
	const std::string backDirectoryPathFrom = pathFrom + backFile;
	const std::string backDirectoryPathTo = pathTo + backFile;

	g_FileTable.RenameFile(dotDirectoryPathFrom.c_str(), dotDirectoryPathTo.c_str());
	g_FileTable.RenameFile(backDirectoryPathFrom.c_str(), backDirectoryPathTo.c_str());
	return errorNumber;
}

/////////////////////////////////////////////////////////////////////
bool RemoveFile(const char* path)
{
	if (0 == _chmod(path, S_IREAD | S_IWRITE))
	{
		if (0 == remove(path))
		{
			g_FileTable.RemoveItem(path);
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
int RemoveFolder(const char* path)
{
	if (0 != _chmod(path, S_IREAD | S_IWRITE))
	{
		return -1;
	}

	if (RemoveDirectory(path) != 0) {
		const std::string dotFile = "\\.";
		const std::string backFile = "\\..";
		const std::string dotDirectoryPath = path + dotFile;
		const std::string backDirectoryPath = path + backFile;

		g_FileTable.RemoveItem(dotDirectoryPath.c_str());
		g_FileTable.RemoveItem(backDirectoryPath.c_str());
		g_FileTable.RemoveItem(path);
		return 0;
	}
	return  GetLastError();
}

/////////////////////////////////////////////////////////////////////
void RemovePathFromFileTable(char* path)
{
	g_FileTable.RemoveItem(path);
}