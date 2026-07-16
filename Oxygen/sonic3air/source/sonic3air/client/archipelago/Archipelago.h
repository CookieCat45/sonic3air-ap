/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once
#ifndef AP_NO_SCHEMA
#define AP_NO_SCHEMA
#endif
#define GAME_NAME "Sonic 3 A.I.R."
#include "oxygen_netcore/network/Sockets.h"
#include "sonic3air/client/archipelago/apclientpp/apclient.hpp"
#include <lemon/program/StringRef.h>

class Archipelago
{
public:
	void setupHandlers();
	void stopConnection();
	bool isConnected();
	void updateConnection(float timeElapsed);
	void sendLocation(uint64 id);
	int getItem(lemon::StringRef name);
	void callScriptFunction(lemon::FlyweightString functionName);
	void setDataInt(lemon::StringRef name, int64 data);
	void triggerGoal();
	int64 getDataInt(lemon::StringRef name);
	bool isZoneAllowed(lemon::StringRef zone);
	bool isLocationChecked(uint64 id);
	bool isLocationAllowedForChar(uint64 id, uint8 character);
	lemon::StringRef getSeedName();

private:
	bool mConnecting = false;
	unsigned long mLastConnect = 0;
	std::map<int, APClient::NetworkItem> mItems;
	std::map<lemon::StringRef, lemon::StringRef> mGlobalStrings;
	nlohmann::json mSlotData;
	static unsigned long now()
    {
		#if defined WIN32 || defined _WIN32
		#if WINVER >= 0x0600 || _WIN32_WINNT >= 0x0600
		if (sizeof(unsigned long) > 4) {
			return static_cast<unsigned long>(GetTickCount64());
		}
		#endif
		return static_cast<unsigned long>(GetTickCount());
		#else
		timespec ts{};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		auto ms = static_cast<unsigned long>(
			static_cast<uint64_t>(ts.tv_sec) * 1000);
		ms += static_cast<unsigned long>(ts.tv_nsec / 1000000);
		return ms;
		#endif
    }

public:
	std::unique_ptr<APClient> mClient;
};
