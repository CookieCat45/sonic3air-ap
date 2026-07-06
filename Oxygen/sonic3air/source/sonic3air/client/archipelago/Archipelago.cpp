/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#ifndef AP_NO_SCHEMA
#define AP_NO_SCHEMA
#endif
#include <nlohmann/json.hpp>
#include "sonic3air/pch.h"
#include "sonic3air/client/archipelago/Archipelago.h"
#include "oxygen/application/Application.h"
#include "oxygen/helper/JsonHelper.h"
#include "oxygen/simulation/Simulation.h"
#include "oxygen/simulation/CodeExec.h"
#include "oxygen/simulation/LogDisplay.h"
#include <imgui.h>

using json = nlohmann::json;
static char serverAddress[512] = "localhost:38281";
static char slotName[512] = "";
static char password[512] = "";
static std::string errorMessage = "";
static std::string socketError = "";

void Archipelago::setupHandlers()
{
	mClient->set_room_info_handler([this]() {
        mClient->ConnectSlot(slotName, password, 7);
    });
	mClient->set_slot_connected_handler([this](const json& slotData) {
        Simulation& sim = Application::instance().getSimulation();
		sim.mDisableInput = false;
		mConnecting = false;
		socketError = "";
		mSlotData = slotData;
		Archipelago::callScriptFunction("Archipelago.OnConnected");
    });
	mClient->set_socket_error_handler([this](const std::string& msg) {
		// don't immediately fail if this is the case - this always happens on localhost
		if (msg != "TLS handshake failed") {
			socketError = msg;
		}
	});
	mClient->set_slot_refused_handler([this](const std::list<std::string>& msgs) {
		socketError = "Rejected by Archipelago server -";
		for (const auto& element : msgs) {
			socketError += " " + element;
		}
	});
	mClient->set_socket_disconnected_handler([this]() {
        if (!mConnecting) {
			Archipelago::callScriptFunction("Archipelago.OnDisconnected");
		}
    });
	mClient->set_print_json_handler([this](const std::list<APClient::TextNode> &msg) {
		bool containsSelf;
		for (const auto& node: msg) {
			if (node.type == "player_id") {
				if (mClient->slot_concerns_self(std::stoi(node.text))) {
					containsSelf = true;
					break;
				}
			}
		}
		
		if (!containsSelf) {
			// since the log can only display one message at a time, only show messages about ourselves
			return;
		}
		
		std::string text = mClient->render_json(msg, APClient::RenderFormat::TEXT).c_str();
        printf("%s\n", text);
		LogDisplay::instance().setLogDisplay(String(text), 6.0f); 
	});
	mClient->set_items_received_handler([this](const std::list<APClient::NetworkItem> &items){
		int highestIndex = 0;
		bool hasProg = false;
		CodeExec& codeExec = Application::instance().getSimulation().getCodeExec();
		LemonScriptRuntime& runtime = codeExec.getLemonScriptRuntime();
		for (const auto& item: items) {
			if (mItems.find(item.index) == mItems.end()) {
				if (item.flags & APClient::ItemFlags::FLAG_ADVANCEMENT)
				{
					hasProg = true;
				}
				
				CodeExec::FunctionExecData execData;
				execData.mParams.mReturnType = &lemon::PredefinedDataTypes::VOID;
				execData.mParams.mParams.emplace_back(lemon::PredefinedDataTypes::STRING, runtime.getInternalLemonRuntime()
					.addString(mClient->get_item_name(item.item, GAME_NAME)));
				execData.mParams.mParams.emplace_back(lemon::PredefinedDataTypes::INT_32, item.index);
				codeExec.executeScriptFunction("Archipelago.OnNewItem", false, &execData);
			}
			
			mItems[item.index] = item;
			if (item.index > highestIndex)
			{
				highestIndex = item.index;
			}
		}
		
		CodeExec::FunctionExecData execData;
		execData.mParams.mReturnType = &lemon::PredefinedDataTypes::VOID;
		execData.mParams.mParams.emplace_back(lemon::PredefinedDataTypes::INT_32, highestIndex);
		execData.mParams.mParams.emplace_back(lemon::PredefinedDataTypes::BOOL, hasProg);
		codeExec.executeScriptFunction("Archipelago.OnReceivedItems", false, &execData);
	});
}

void Archipelago::stopConnection()
{
	mClient.reset();
}

bool Archipelago::isConnected()
{
	return mClient && mClient->get_state() == APClient::State::SLOT_CONNECTED;
}

void Archipelago::updateConnection(float timeElapsed)
{
	Simulation& sim = Application::instance().getSimulation();
	if (!mClient || sim.mDisableInput)
	{
		sim.mDisableInput = true;
		ImGui::Begin("Connection Input");
		ImGui::InputText("Server address", serverAddress, sizeof(serverAddress), ImGuiInputTextFlags_CharsNoBlank);
		ImGui::InputText("Slot name", slotName, sizeof(slotName));
		ImGui::InputText("Password", password, sizeof(password));
		bool connectClicked = ImGui::Button(mConnecting ? "Connecting..." : "Connect");
		if (connectClicked && !mConnecting)
		{
			if (std::strlen(serverAddress) <= 0)
			{
				errorMessage = "Please enter a server address";
				ImGui::OpenPopup("Error");
			}
			else if (std::strlen(slotName) <= 0)
			{
				errorMessage = "Please enter a slot name";
				ImGui::OpenPopup("Error");
			}
			else
			{
				mConnecting = true;
				mLastConnect = now();
				mClient.reset();
				printf("Connecting to AP...\n");
				mClient.reset(new APClient("", GAME_NAME, serverAddress));
				setupHandlers();
			}
		}
		else if (mConnecting && mClient)
		{
			mClient->poll();
			bool timeOut = static_cast<unsigned long>(now() - mLastConnect) > 11000;
			if (timeOut)
			{
				socketError = "Connection timed out";
			}
			
			if (socketError.length() > 0)
			{
				mConnecting = false;
				mClient.reset();
				errorMessage = "Connection failed: " + socketError;
				socketError = "";
				ImGui::OpenPopup("Error");
			}
		}
		
		if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) 
		{
			ImGui::Text("%s", errorMessage.c_str());
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter)) 
			{
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
		
		ImGui::End();
		return;
	}
	
	mClient->poll();
	APClient::State state = mClient->get_state();
	mConnecting = (state > APClient::State::DISCONNECTED && state < APClient::State::SLOT_CONNECTED);
}

void Archipelago::callScriptFunction(lemon::FlyweightString functionName)
{
	Application::instance().getSimulation().getCodeExec().getLemonScriptRuntime().callFunctionByName(functionName);
}

void Archipelago::setDataInt(lemon::StringRef name, int64 data)
{
	mSlotData[name.getString()] = data;
}

int64 Archipelago::getDataInt(lemon::StringRef name)
{
	if (mSlotData.is_null() || !mSlotData.contains(name.getString()) || mSlotData[name.getString()].is_null())
	{
		return 0;
	}

	return int64(mSlotData.value(name.getString(), 0));
}

bool Archipelago::isZoneAllowed(lemon::StringRef zone)
{
	if (mSlotData.is_null() || !mSlotData.contains("ZonesAllowed"))
		return false;
		
	auto zones = mSlotData["ZonesAllowed"].get<std::vector<std::string>>();
	return std::find(zones.begin(), zones.end(), zone.getString()) != zones.end();
}

void Archipelago::sendLocation(uint64 id)
{
	if (!Archipelago::isConnected())
		return;

	std::set<int64_t> checkedLocs = mClient->get_checked_locations();
	if (checkedLocs.find(id) != checkedLocs.end())
	{
		return;
	}
	
	std::list<int64_t> idList;
	idList.push_front(id);
	mClient->LocationChecks(idList);
}

int Archipelago::getItem(lemon::StringRef name) 
{
	int count = 0;
	for (const auto& [index, item]: mItems) {
        if (mClient->get_item_name(item.item, GAME_NAME) == name.getString()) {
			count++;
		}
    }

	return count;
}

lemon::StringRef Archipelago::getSeedName()
{
	return lemon::StringRef(lemon::Runtime::getActiveRuntime()->addString(std::string_view(mClient->get_seed())));
}
