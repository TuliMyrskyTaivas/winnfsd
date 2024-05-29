/////////////////////////////////////////////////////////////////////
/// file: FileTree.h
///
/// summary: File tree
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_FILETREE_H
#define ICENFSD_FILETREE_H

#include <string>
#include "tree.hh"

class FileTree
{
public:
	struct FileItem
	{
		std::string path;
		uint64_t handle = 0;
		bool cached = false;
	};
	using Node = tree_node_<FileItem>*;

	Node AddItem(const std::string& absolutePath, uint64_t handle);
	void RemoveItem(const std::string& absolutePath);
	void RenameItem(const std::string& absolutePathFrom, const std::string& absolutePathTo);

	Node FindFileItemForPath(const std::string& absolutePath);
	void GetNodeFullPath(Node node, std::string& fullPath);

private:
	Node FindNodeFromRootWithPath(const std::string& path);
	Node FindNodeWithPathFromNode(const std::string& path, tree_node_<FileItem>* node) const;
	Node FindParentNodeFromRootForPath(const std::string& path) const;

	tree<FileItem> m_filesTree;
	tree<FileItem>::iterator m_topNode;
};

#endif // ICENFSD_FILETREE_H