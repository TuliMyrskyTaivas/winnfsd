/////////////////////////////////////////////////////////////////////
/// file: Settings.h
///
/// summary: settings parser
/////////////////////////////////////////////////////////////////////

#ifndef ICENFSD_SETTINGS_H
#define ICENFSD_SETTINGS_H

#include <memory>
#include <string>
#include <map>

struct sockaddr_in;
struct SettingsData;

using Exports = std::map<std::string, std::string>;

class Settings
{
	std::unique_ptr<SettingsData> m_data;

public:
	Settings(int argc, char* argv[]);
	~Settings();

	const Exports& GetExports() const noexcept;
	const sockaddr_in& GetRpcEndpoint() const noexcept;
	const sockaddr_in& GetNfsEndpoint() const noexcept;
	const sockaddr_in& GetMountEndpoint() const noexcept;
	unsigned int GetUid() const noexcept;
	unsigned int GetGid() const noexcept;

private:
	void SetupLogger(bool verbose) const;
};

#endif // ICENFSD_SETTINGS_H