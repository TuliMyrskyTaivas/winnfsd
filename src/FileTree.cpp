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

#include "conv.h"

/////////////////////////////////////////////////////////////////////
static std::string FirstDirname(const std::string& path)
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
static std::string Dirname932(const std::string& path)
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
static std::string FollowingPath(const std::string& path)
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
static std::string Basename932(const std::string& path)
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
FileTree::Node FileTree::AddItem(const std::string& absolutePath, uint64_t handle)
{
	FileItem item;
	item.handle = handle;
	item.cached = false;

	// If the tree is empty just add the new path as node on the top level.
	if (m_filesTree.empty())
	{
		item.path = absolutePath;
		m_filesTree.set_head(item);
		m_topNode = m_filesTree.begin();
	}
	else
	{
		// Check if the requested path belongs to an already registered parent node.
		tree_node_<FileItem>* parentNode = FindParentNodeFromRootForPath(absolutePath);
		item.path = Basename932(absolutePath);
		// If a parent was found use th parent.
		if (parentNode)
		{
			auto result = m_filesTree.append_child(tree<FileItem>::iterator_base(parentNode), item);
			return result.node;
		}
		else
		{
			// Node wasn't found - most likely a new root - add it to the top level.
			item.path = absolutePath;
			m_filesTree.insert(tree<FileItem>::iterator_base(m_topNode), item);
			m_topNode = m_filesTree.begin();
		}
	}

	return m_topNode.node;
}

/////////////////////////////////////////////////////////////////////
void FileTree::RemoveItem(const std::string& absolutePath)
{
	auto node = FindNodeFromRootWithPath(absolutePath);
	if (node != nullptr) {
		m_filesTree.erase(tree<FileItem>::iterator(node));
	}
}

/////////////////////////////////////////////////////////////////////
void FileTree::RenameItem(const std::string& absolutePathFrom, const std::string& absolutePathTo)
{
	auto node = FindNodeFromRootWithPath(absolutePathFrom);
	auto parentNode = FindParentNodeFromRootForPath(absolutePathTo);

	if (parentNode != nullptr && node != nullptr)
	{
		if (m_filesTree.number_of_children(parentNode) < 1)
		{
			FileItem emptyItem{};
			m_filesTree.append_child(tree<FileItem>::iterator_base(parentNode), emptyItem);
		}
		tree<FileItem>::iterator firstChild = m_filesTree.begin(parentNode);
		m_filesTree.move_after(firstChild, tree<FileItem>::iterator(node));
		node->data.path = absolutePathTo.substr(absolutePathTo.find_last_of('\\') + 1);
	}
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTree::FindFileItemForPath(const std::string& absolutePath)
{
	return FindNodeFromRootWithPath(absolutePath);
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTree::FindNodeFromRootWithPath(const std::string& path)
{
	// No topNode - bail out.
	if (m_topNode.node == nullptr)
	{
		return nullptr;
	}

	const std::string& rootPath = m_topNode->path;
	// topNode path and requested path are the same? Use the node.
	if (path == rootPath)
	{
		return m_topNode.node;
	}

	// If the topNode path is part of the requested path this is a subpath.
	// Use the node.
	if (path.find(rootPath) != std::string::npos)
	{
		std::string subPath = path.substr(m_topNode->path.length() + 1);
		return FindNodeWithPathFromNode(subPath, m_topNode.node);
	}
	else
	{
		// If the current topNode isn't related to the requested path
		// iterate over all _top_ level elements in the tree to look for
		// a matching item and register it as current top node.
		tree<FileItem>::sibling_iterator it;
		for (it = m_filesTree.begin(); it != m_filesTree.end(); it++)
		{
			const std::string& itPath = it.node->data.path;
			// Current item path matches the requested path - use the item as topNode.
			if (path == itPath)
			{
				m_topNode = it;
				return it.node;
			}
			else if (path.find(itPath) != std::string::npos)
			{
				// If the item path is part of the requested path this is a subpath.
				// Use the the item as topNode and continue analyzing.
				m_topNode = it;
				std::string subPath = path.substr(itPath.length() + 1);
				return FindNodeWithPathFromNode(subPath.c_str(), it.node);
			}
		}
	}
	// Nothing found
	return nullptr;
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTree::FindNodeWithPathFromNode(const std::string& path, tree_node_<FileItem>* node) const
{
	tree<FileItem>::sibling_iterator sib = m_filesTree.begin(node);
	tree<FileItem>::sibling_iterator end = m_filesTree.end(node);

	std::string currentPath = FirstDirname(path);
	std::string followingPath = FollowingPath(path);
	bool currentLevel = followingPath.empty();

	while (sib != end)
	{
		if (sib->path == currentPath)
		{
			if (currentLevel)
			{
				return sib.node;
			}
			else
			{
				return FindNodeWithPathFromNode(followingPath, sib.node);
			}
		}
		++sib;
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////
FileTree::Node FileTree::FindParentNodeFromRootForPath(const std::string& path) const
{
	const std::string& rootPath = m_topNode->path;

	// If the topNode path is not part of the requested path bail out.
	// This avoids also issues with taking substrings of incompatible
	// paths below.
	if (path.find(rootPath) == std::string::npos)
	{
		return nullptr;
	}
	std::string currentPath = path.substr(rootPath.length() + 1);
	std::string followingPath = Dirname932(currentPath);
	if (followingPath.empty())
	{
		return m_topNode.node;
	}
	else
	{
		return FindNodeWithPathFromNode(followingPath, m_topNode.node);
	}
}

/////////////////////////////////////////////////////////////////////
void FileTree::GetNodeFullPath(tree_node_<FileItem>* node, std::string& path)
{
	// TODO: refactor this, we build string from bottom to top
	path.append(node->data.path);
	tree_node_<FileItem>* parentNode = node->parent;
	while (parentNode != nullptr)
	{
		path.insert(0, "\\");
		path.insert(0, parentNode->data.path);
		parentNode = parentNode->parent;
	}
}