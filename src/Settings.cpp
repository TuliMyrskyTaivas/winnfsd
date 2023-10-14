/////////////////////////////////////////////////////////////////////
/// file: Settings.h
///
/// summary: settings parser
/////////////////////////////////////////////////////////////////////

#include "Settings.h"
#include <WS2tcpip.h>
#include <winsock.h>
#include <iostream>
#include <fstream>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/program_options.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>

/////////////////////////////////////////////////////////////////////
struct SettingsData
{
	unsigned int uid = 0;
	unsigned int gid = 0;
	sockaddr_in endpoint{};
	Exports exports{};
};

/////////////////////////////////////////////////////////////////////
Exports ParseExportsFile(const std::string& exportsPath)
{
	Exports result;

	std::ifstream exports(exportsPath);
	if (!exports)
	{
		throw std::runtime_error("failed to open file " + exportsPath);
	}

	std::string line;
	while (std::getline(exports, line))
	{
		// split path and alias separated by '>'
		auto delimiter = line.find('>');
		if (std::string::npos == delimiter)
		{
			throw std::runtime_error("Invalid line in exports: " + line);
		}

		auto path = line.substr(0, delimiter);
		auto alias = line.substr(delimiter + 1);

		// clean path, trim spaces and slashes (except drive letter)
		path.erase(path.find_last_not_of(" ") + 1);
		if (path.substr(path.size() - 2) != ":\\")
		{
			path.erase(path.find_last_not_of("/\\ ") + 1);
		}

		result.emplace(path, alias);
	}

	return result;
}

/////////////////////////////////////////////////////////////////////
sockaddr_in BuildEndpoint(const std::string& addr, unsigned int port)
{
	if (port > 65535)
	{
		throw std::runtime_error("invalid value for IPv4 port: " + std::to_string(port));
	}

	sockaddr_in result{};
	const int err = inet_pton(AF_INET, addr.c_str(), &result.sin_addr);
	if (0 == err)
	{
		throw std::runtime_error("not a valid IPv4 address: " + addr);
	}
	result.sin_family = AF_INET;
	result.sin_port = htons(static_cast<unsigned short>(port));
	return result;
}

/////////////////////////////////////////////////////////////////////
Settings::Settings(int argc, char* argv[])
	: m_data{ std::make_unique<SettingsData>() }
{
	bool verboseMode = false;
	unsigned int uid = 0, gid = 0;
	unsigned int port = 0;
	std::string address, exports;

	namespace po = boost::program_options;
	po::options_description cmdLine("Available options");
	cmdLine.add_options()
		("address,a", po::value<std::string>(&address)->default_value("127.0.0.1"), "IPv4 address to listen on")
		("port,p", po::value<unsigned int>(&port), "port to listen on (required)")
		("exports,e", po::value<std::string>(&exports), "path to exports file")
		("uid,u", po::value<unsigned int>(&uid), "user ID")
		("uid,g", po::value<unsigned int>(&gid), "group ID")
		("verbose,v", po::bool_switch(&verboseMode), "enable verbose logging")
		("help,h", "show this message");

	po::variables_map map;
	try
	{
		po::store(po::command_line_parser(argc, argv).options(cmdLine).run(), map);
		if (map.count("help"))
		{
			std::cout << "IceNFSd {{VERSION}} [{{HASH}}]\n"
				<< "Network File System server for Windows\n"
				<< "Copyright (C) 2005 Ming-Yang Kao\n"
				<< "Edited in 2011 by ZeWaren\n"
				<< "Edited in 2013 by Alexander Schneider (Jankowfsky AG)\n"
				<< "Edited in 2014 2015 by Yann Schepens\n"
				<< "Edited in 2016 by Peter Philipp (Cando Image GmbH), Marc Harding\n"
				<< "Rewritten to C++ in 2022 by Andrey Sploshnov\n"
				<< cmdLine << std::endl;
			exit(EXIT_SUCCESS);
		}

		po::notify(map);
	}
	catch (po::error& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << cmdLine << std::endl;
		throw std::runtime_error("invalid command line");
	}

	SetupLogger(verboseMode);
	if (!exports.empty())
	{
		m_data->exports = std::move(ParseExportsFile(exports));
	}
	m_data->endpoint = BuildEndpoint(address, port);
}

/////////////////////////////////////////////////////////////////////
Settings::~Settings()
{}

/////////////////////////////////////////////////////////////////////
const Exports& Settings::GetExports() const noexcept
{
	return m_data->exports;
}

/////////////////////////////////////////////////////////////////////
const sockaddr_in& Settings::GetEndpoint() const noexcept
{
	return m_data->endpoint;
}

/////////////////////////////////////////////////////////////////////
void Settings::SetupLogger(bool verbose) const
{
	const auto logLevel = verbose ? boost::log::trivial::debug : boost::log::trivial::info;
	const auto logFormat = verbose ? "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%" : "%Message%";
	boost::log::add_common_attributes();
	boost::log::add_console_log(std::cout, boost::log::keywords::format = logFormat);
	auto logCore = boost::log::core::get();
	logCore->set_filter(boost::log::trivial::severity >= logLevel);
}

/////////////////////////////////////////////////////////////////////
unsigned int Settings::GetUid() const noexcept
{
	return m_data->uid;
}

/////////////////////////////////////////////////////////////////////
unsigned int Settings::GetGid() const noexcept
{
	return m_data->gid;
}