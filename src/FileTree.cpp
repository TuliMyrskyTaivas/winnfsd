/////////////////////////////////////////////////////////////////////
/// file: FileTree.cpp
///
/// summary: File tree
/////////////////////////////////////////////////////////////////////

#include "FileTree.h"
#include <string>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>
#include "tree.hh"
#include "conv.h"

static tree<FileItem> filesTree;
static tree<FileItem>::iterator topNode;

/////////////////////////////////////////////////////////////////////
std::string FirstDirname(const std::string& path)
{
	auto wcs = ConvFromCp932(path.c_str());
	if (wcs == nullptr)
	{
		return path.substr(0, path.find('\\'));
	}

	auto wpath = std::basic_string<wchar_t>(wcs);
	delete wcs;

	auto dest = ConvToCp932(wpath.substr(0, wpath.find(L'\\')).c_str());
	if (dest == nullptr) {
		return path.substr(0, path.find('\\'));
	}

	auto result = std::string(dest);
	delete dest;

	return result;
}

/////////////////////////////////////////////////////////////////////
std::string Dirname932(const std::string& path)
{
	auto wcs = ConvFromCp932(path.c_str());
	auto result = path.find('\\') != std::string::npos ? path.substr(0, path.find_last_of('\\')) : std::string("");
	if (wcs == nullptr)
	{
		return result;
	}
	auto wpath = std::basic_string<wchar_t>(wcs);
	delete wcs;

	if (wpath.find(L'\\') == std::basic_string<wchar_t>::npos)
	{
		return result;
	}
	auto dest = ConvToCp932(wpath.substr(0, wpath.find_last_of(L'\\')).c_str());
	if (dest == nullptr)
	{
		return result;
	}
	result = std::string(dest);
	delete dest;
	return result;
}

/////////////////////////////////////////////////////////////////////
std::string FollowingPath(const std::string& path)
{
	auto wcs = ConvFromCp932(path.c_str());
	if (wcs == nullptr)
	{
		return path.find('\\') != std::string::npos ? path.substr(path.find('\\') + 1) : std::string("");
	}
	auto wpath = std::basic_string<wchar_t>(wcs);
	delete wcs;
	if (wpath.find(L'\\') == std::basic_string<wchar_t>::npos)
	{
		return std::string("");
	}
	auto dest = ConvToCp932(wpath.substr(wpath.find(L'\\') + 1).c_str());
	if (dest == nullptr)
	{
		return path.find('\\') != std::string::npos ? path.substr(path.find('\\') + 1) : std::string("");
	}
	auto result = std::string(dest);
	delete dest;
	return result;
}

/////////////////////////////////////////////////////////////////////
std::string Basename932(const std::string& path)
{
	auto wcs = ConvFromCp932(path.c_str());
	if (wcs == nullptr)
	{
		return path.substr(path.find_last_of('\\') + 1);
	}
	auto wpath = std::basic_string<wchar_t>(wcs);
	delete wcs;
	auto dest = ConvToCp932(wpath.substr(wpath.find_last_of(L'\\') + 1).c_str());
	if (dest == nullptr)
	{
		return path.substr(path.find_last_of('\\') + 1);
	}
	auto result = std::string(dest);
	delete dest;
	return result;
}

/////////////////////////////////////////////////////////////////////
FileItem FileTree::AddItem(const char* absolutePath, unsigned char* handle)
{
	FileItem item;
	item.handle = handle;
	item.cached = false;

	// If the tree is empty just add the new path as node on the top level.
	if (filesTree.empty())
	{
		item.path = new char[strlen(absolutePath) + 1];
		strcpy_s(item.path, (strlen(absolutePath) + 1), absolutePath);
		item.pathLen = static_cast<int>(strlen(item.path));

		filesTree.set_head(item);
		topNode = filesTree.begin();
	}
	else
	{
		// Check if the requested path belongs to an already registered parent node.
		std::string sPath(absolutePath);
		tree_node_<FileItem>* parentNode = FindParentNodeFromRootForPath(absolutePath);
		std::string splittedPath = Basename932(sPath);
		//printf("spl %s %s\n", splittedPath.c_str(), absolutePath);
		item.path = new char[splittedPath.length() + 1];
		strcpy_s(item.path, (splittedPath.length() + 1), splittedPath.c_str());
		// If a parent was found use th parent.
		if (parentNode)
		{
			//printf("parent %s\n", parentNode->data.path);
			filesTree.append_child(tree<FileItem>::iterator_base(parentNode), item);
		}
		else
		{
			// Node wasn't found - most likely a new root - add it to the top level.
			//printf("No parent node found for %s. Adding new sibbling.", absolutePath);
			item.path = new char[strlen(absolutePath) + 1];
			strcpy_s(item.path, (strlen(absolutePath) + 1), absolutePath);
			item.pathLen = static_cast<int>(strlen(item.path));

			filesTree.insert(tree<FileItem>::iterator_base(topNode), item);
			topNode = filesTree.begin();
		}
	}

	return item;
}

/////////////////////////////////////////////////////////////////////
void FileTree::RemoveItem(const char* absolutePath)
{
	tree_node_<FileItem>* node = FindNodeFromRootWithPath(absolutePath);
	if (node != nullptr) {
		filesTree.erase(tree<FileItem>::iterator(node));
	}
}

/////////////////////////////////////////////////////////////////////
void FileTree::RenameItem(const char* absolutePathFrom, const char* absolutePathTo)
{
	tree_node_<FileItem>* node = FindNodeFromRootWithPath(absolutePathFrom);
	tree_node_<FileItem>* parentNode = FindParentNodeFromRootForPath(absolutePathTo);

	if (parentNode != nullptr && node != nullptr)
	{
		if (filesTree.number_of_children(parentNode) < 1)
		{
			FileItem emptyItem;
			emptyItem.pathLen = 0;
			emptyItem.path = const_cast<char*>("");
			filesTree.append_child(tree<FileItem>::iterator_base(parentNode), emptyItem);
		}
		tree<FileItem>::iterator firstChild = filesTree.begin(parentNode);
		filesTree.move_after(firstChild, tree<FileItem>::iterator(node));

		std::string sPath(absolutePathTo);
		std::string splittedPath = sPath.substr(sPath.find_last_of('\\') + 1);
		node->data.path = new char[splittedPath.length() + 1];
		strcpy_s(node->data.path, (splittedPath.length() + 1), splittedPath.c_str());
	}
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTree::FindFileItemForPath(const char* absolutePath)
{
	return FindNodeFromRootWithPath(absolutePath);
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTree::FindNodeFromRootWithPath(const char* path)
{
	// No topNode - bail out.
	if (topNode.node == nullptr)
	{
		return nullptr;
	}
	std::string sPath(path);
	std::string nPath(topNode->path);
	// topNode path and requested path are the same? Use the node.
	if (sPath == nPath)
	{
		return topNode.node;
	}

	// If the topNode path is part of the requested path this is a subpath.
	// Use the node.
	if (sPath.find(nPath) != std::string::npos)
	{
		std::string splittedString = sPath.substr(strlen(topNode->path) + 1);
		return FindNodeWithPathFromNode(splittedString.c_str(), topNode.node);
	}
	else
	{
		// If the current topNode isn't related to the requested path
		// iterate over all _top_ level elements in the tree to look for
		// a matching item and register it as current top node.
		tree<FileItem>::sibling_iterator it;
		for (it = filesTree.begin(); it != filesTree.end(); it++)
		{
			std::string itPath(it.node->data.path);
			// Current item path matches the requested path - use the item as topNode.
			if (sPath == itPath)
			{
				topNode = it;
				return it.node;
			}
			else if (sPath.find(itPath) != std::string::npos)
			{
				// If the item path is part of the requested path this is a subpath.
				// Use the the item as topNode and continue analyzing.
				topNode = it;
				std::string splittedString = sPath.substr(itPath.length() + 1);
				return FindNodeWithPathFromNode(splittedString.c_str(), it.node);
			}
		}
	}
	// Nothing found
	return nullptr;
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTree::FindNodeWithPathFromNode(const char* path, tree_node_<FileItem>* node)
{
	tree<FileItem>::sibling_iterator sib = filesTree.begin(node);
	tree<FileItem>::sibling_iterator end = filesTree.end(node);

	std::string currentPath = FirstDirname(path);
	std::string followingPath = FollowingPath(path);
	bool currentLevel = followingPath.empty();

	while (sib != end)
	{
		if (strcmp(sib->path, currentPath.c_str()) == 0)
		{
			if (currentLevel)
			{
				return sib.node;
			}
			else
			{
				return FindNodeWithPathFromNode(followingPath.c_str(), sib.node);
			}
		}
		++sib;
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////
tree_node_<FileItem>* FileTree::FindParentNodeFromRootForPath(const char* path)
{
	std::string sPath(path);
	std::string nPath(topNode->path);

	// If the topNode path is not part of the requested path bail out.
	// This avoids also issues with taking substrings of incompatible
	// paths below.
	if (sPath.find(nPath) == std::string::npos)
	{
		return nullptr;
	}
	std::string currentPath = sPath.substr(strlen(topNode->path) + 1);
	std::string followingPath = Dirname932(currentPath);
	if (followingPath.empty())
	{
		return topNode.node;
	}
	else
	{
		return FindNodeWithPathFromNode(followingPath.c_str(), topNode.node);
	}
}

/////////////////////////////////////////////////////////////////////
void FileTree::GetNodeFullPath(tree_node_<FileItem>* node, std::string& path)
{
	path.append(node->data.path);
	tree_node_<FileItem>* parentNode = node->parent;
	while (parentNode != nullptr)
	{
		path.insert(0, "\\");
		path.insert(0, parentNode->data.path);
		parentNode = parentNode->parent;
	}
}