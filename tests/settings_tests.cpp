/////////////////////////////////////////////////////////////////////
/// file: tests/settings_tests.cpp
///
/// summary: unit tests for the settings
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

#include "../src/Settings.cpp"

/////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestParseExportsFile);
struct ParseExportsFileFixture
{
	const std::string exportsFile = "test_exports.txt";

	ParseExportsFileFixture()
	{}

	~ParseExportsFileFixture()
	{
		if (fs::exists(exportsFile))
		{
			fs::remove(exportsFile);
		}
	}

	void CreateExports(const std::string& content)
	{
		std::ofstream file(exportsFile, std::ios::trunc | std::ios::out);
		BOOST_CHECK(file.is_open());
		file << content;
		file.close();
	}
};

/////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_CASE(NonExistingFile, ParseExportsFileFixture)
{
	Exports exports;
	BOOST_CHECK_THROW(exports = ParseExportsFile("xxx"), std::runtime_error);
	BOOST_CHECK(exports.empty());
}

/////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_CASE(EmptyFile, ParseExportsFileFixture)
{
	CreateExports(std::string{});
	Exports exports;
	BOOST_CHECK_NO_THROW(exports = ParseExportsFile(exportsFile));
	BOOST_CHECK(exports.empty());
}

/////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_CASE(InvalidExport, ParseExportsFileFixture)
{
	const std::string exports =
		"C:\\Temp > /test\n"
		"c:\\xxx = yyyy\n";
	CreateExports(exports);
	Exports result;
	BOOST_CHECK_THROW(result = ParseExportsFile(exportsFile), std::runtime_error);
	BOOST_CHECK(result.empty());
}

/////////////////////////////////////////////////////////////////////
BOOST_FIXTURE_TEST_CASE(ValidExport, ParseExportsFileFixture)
{
	const std::string exports =
		"    C:\\Temp   >  /test   \n";
	CreateExports(exports);
	Exports result;
	BOOST_CHECK_NO_THROW(result = ParseExportsFile(exportsFile));
	BOOST_CHECK_EQUAL(1, result.size());

	const auto& item = *result.cbegin();
	BOOST_CHECK_EQUAL(item.first, "C:\\Temp");
	BOOST_CHECK_EQUAL(item.second, "/test");
}
BOOST_AUTO_TEST_SUITE_END();

/////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE(TestBuildEndpoint)
BOOST_AUTO_TEST_CASE(InvalidPort)
{
	sockaddr_in result{};
	BOOST_CHECK_THROW(result = BuildEndpoint("127.0.0.1", 112220), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(InvalidAddress)
{
	sockaddr_in result{};
	BOOST_CHECK_THROW(result = BuildEndpoint("256.0.0.1", 1024), std::runtime_error);
}
BOOST_AUTO_TEST_CASE(ValidInput)
{
	const std::string addr = "127.0.0.1";
	const unsigned int port = 2048;
	sockaddr_in result{};
	BOOST_CHECK_NO_THROW(result = BuildEndpoint(addr, port));
	BOOST_CHECK_EQUAL(result.sin_family, AF_INET);
	BOOST_CHECK_EQUAL(result.sin_port, htons(static_cast<unsigned short>(port)));

	const auto& addrBytes = result.sin_addr.S_un.S_un_b;
	BOOST_CHECK_EQUAL(addrBytes.s_b4, 1);
	BOOST_CHECK_EQUAL(addrBytes.s_b3, 0);
	BOOST_CHECK_EQUAL(addrBytes.s_b2, 0);
	BOOST_CHECK_EQUAL(addrBytes.s_b1, 127);
}
BOOST_AUTO_TEST_SUITE_END();