/////////////////////////////////////////////////////////////////////
/// file: FileTree.h
///
/// summary: File tree
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_FILETREE_H
#define ICENFSD_FILETREE_H

#include "FileTable.h"

class FileTree
{
public:
	bool static const debug = false;
	FileItem AddItem(const char* absolutePath, unsigned char* handle);
	void RemoveItem(const char* absolutePath);
	void RenameItem(const char* absolutePathFrom, const char* absolutePathTo);

	tree_node_<FileItem>* FindFileItemForPath(const char* absolutePath);

	void GetNodeFullPath(tree_node_<FileItem>* node, std::string& fullPath);

protected:
	tree_node_<FileItem>* FindNodeFromRootWithPath(const char* path);
	tree_node_<FileItem>* FindNodeWithPathFromNode(const char* path, tree_node_<FileItem>* node);
	tree_node_<FileItem>* FindParentNodeFromRootForPath(const char* path);
};

#endif // ICENFSD_FILETREE_H