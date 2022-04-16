// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "winmm.lib")
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include "arcdps_structs.h"
#include "imgui\imgui.h"
#include "mumble.h"
#include <comdef.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH: break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
    }
    return TRUE;
}

/* proto */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion);
extern "C" __declspec(dllexport) void* get_release_addr();

arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_imgui(uint32_t not_charsel_or_loading);
uintptr_t mod_options_end(); // Submenu under Extensions tab

void log(char* str);
void log_file(char* str);
void log_arc(char* str);

/* globals */
arcdps_exports arc_exports;
void* filelog;
void* arclog;

LinkedMem* p_Mumble = nullptr;
bool show_interface = false;
bool show_player = false;
bool show_camera = false;
bool show_game = false;
bool show_identity = false;

void log(char* str) /* log to arcdps.log and log window*/
{
	log_file(str);
	log_arc(str);
	return;
}
void log_file(char* str) /* log to arcdps.log, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))filelog;
	if (log) (*log)(str);
	return;
}
void log_arc(char* str) /* log to extensions tab in arcdps log window, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))arclog;
	if (log) (*log)(str);
	return;
}

/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion)
{
	ImGui::SetCurrentContext((ImGuiContext*)imguictx);
	ImGui::SetAllocatorFunctions((void *(*)(size_t, void*))mallocfn, (void(*)(void*, void*))freefn); // on imgui 1.80+

	filelog = (void*)GetProcAddress((HMODULE)arcdll, "e3");
	arclog = (void*)GetProcAddress((HMODULE)arcdll, "e8");

	return mod_init;
}
/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr()
{
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks. exports struct and strings are copied to arcdps memory only once at init */
arcdps_exports* mod_init()
{
	/* for arcdps */
	memset(&arc_exports, 0, sizeof(arc_exports));
	arc_exports.sig = 0xC8028A2E;
	arc_exports.imguivers = IMGUI_VERSION_NUM;
	arc_exports.size = sizeof(arc_exports);
	arc_exports.out_name = "Mumble";
	arc_exports.out_build = __DATE__ " " __TIME__;
	arc_exports.imgui = mod_imgui;
	arc_exports.options_end = mod_options_end;

	log((char*)"Mumble: MOD_INIT"); // if using vs2015+, project properties > c++ > conformance mode > permissive to avoid const to not const conversion error
	
	p_Mumble = mumble_link_create(get_mumble_name());

	return &arc_exports;
}
/* release mod -- return ignored */
uintptr_t mod_release()
{
	mumble_link_destroy();
	return 0;
}

uintptr_t UIInterfaceData()
{
	ImGui::Begin("Interface", &show_interface);

	if (ImGui::BeginTable("table_ui", 2))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("version");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->ui_version);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("tick");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->ui_tick);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mapopen");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsMapOpen ? "true" : "false");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("focus");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsGameFocused ? "true" : "false");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("pvp");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsCompetitive ? "true" : "false");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("txtfocus");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsTextboxFocused ? "true" : "false");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("combat");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsInCombat ? "true" : "false");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("cmploc");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p_Mumble->IsCompassTopRight ? "top" : "bottom");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("cmpwidth");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->compassWidth);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("cmpheight");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->compassHeight);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("cmprot");
		ImGui::TableSetColumnIndex(1);
		p_Mumble->IsCompassRotating ? ImGui::Text("%+1.4f", p_Mumble->compassRotation) : ImGui::Text("%s", "disabled");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mapx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%09.4f", p_Mumble->mapCenterX);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mapy");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%09.4f", p_Mumble->mapCenterY);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mapscale");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%1.4f", p_Mumble->mapScale);

		ImGui::EndTable();
	}

	ImGui::End();

	return 0;
}
uintptr_t UIPlayerData()
{
	ImGui::Begin("Player", &show_player);

	if (ImGui::BeginTable("table_player", 2))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_pos.x);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posy");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_pos.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posz");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_pos.z);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("frontx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_front.x);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("fronty");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_front.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("frontz");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->avatar_front.z);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("gx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%09.4f", p_Mumble->playerX);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("gy");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%09.4f", p_Mumble->playerY);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mount");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", mountLookup.at(p_Mumble->mountIndex).c_str());

		ImGui::EndTable();
	}

	ImGui::End();

	return 0;
}
uintptr_t UICameraData()
{
	ImGui::Begin("Camera", &show_camera);

	if (ImGui::BeginTable("table_camera", 2))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_pos.x);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posy");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_pos.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("posz");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_pos.z);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("frontx");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_front.x);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("fronty");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_front.y);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("frontz");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%+09.4f", p_Mumble->cam_front.z);

		ImGui::EndTable();
	}

	ImGui::End();

	return 0;
}
uintptr_t UIGameData()
{
	ImGui::Begin("Game", &show_game);

	if (ImGui::BeginTable("table_game", 2))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("mapid");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->mapId);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("maptype");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%s", mapTypeLookup.at(p_Mumble->mapType).c_str());
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("ip");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u.%u.%u.%u", p_Mumble->serverAddress[4], p_Mumble->serverAddress[5], p_Mumble->serverAddress[6], p_Mumble->serverAddress[7]);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("shard");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->shardId);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("instance");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->instance);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("build");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->buildId);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("pid");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%u", p_Mumble->processId);

		ImGui::EndTable();
	}

	ImGui::End();

	return 0;
}
uintptr_t UIIdentityData()
{
	ImGui::Begin("Identity", &show_identity);

	if (p_Mumble->identity[0])
	{
		json j = json::parse(p_Mumble->identity);

		if (ImGui::BeginTable("table_identity", 2))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("cmdr");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%s", j["commander"].get<bool>() ? "true" : "false");
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fov");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%1.4f", j["fov"].get<float>());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("name");
			ImGui::TableSetColumnIndex(1); ImGui::Text(j["name"].get<std::string>().c_str());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("prof");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%s", profLookup.at(j["profession"].get<unsigned>()).c_str());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("race");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%s", raceLookup.at(j["race"].get<unsigned>()).c_str());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("spec");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%s", specLookup.at(j["spec"].get<unsigned>()).c_str());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("team");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%u", j["team_color_id"].get<unsigned>());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("uisz");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%s", uiszLookup.at(j["uisz"].get<unsigned>()).c_str());
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("world");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%u", j["world_id"].get<unsigned>());

			ImGui::EndTable();
		}
	}
	else
	{
		ImGui::Text("Identity unitialised.");
	}

	ImGui::End();

	return 0;
}

uintptr_t mod_imgui(uint32_t not_charsel_or_loading)
{
	if (show_interface) { UIInterfaceData(); }
	if (show_player) { UIPlayerData(); }
	if (show_camera) { UICameraData(); }
	if (show_game) { UIGameData(); }
	if (show_identity) { UIIdentityData(); }

	return 0;
}

uintptr_t mod_options_end() {
	ImGui::Text("Pointer %p", p_Mumble);

	ImGui::Checkbox("Interface", &show_interface);
	ImGui::Checkbox("Player", &show_player);
	ImGui::Checkbox("Camera", &show_camera);
	ImGui::Checkbox("Game", &show_game);
	ImGui::Checkbox("Identity", &show_identity);

	return 0;
}