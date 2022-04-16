#pragma once
#include <D3D9.h>
#include <d3d9types.h>
#include <map>
#include <string>

const std::map<int, std::string> mapTypeLookup{
	{0, "AutoRedirect"},
	{1, "CharacterCreation"},
	{2, "PvP"},
	{3, "GvG"},
	{4, "Instance"},
	{5, "Public"},
	{6, "Tournament"},
	{7, "Tutorial"},
	{8, "UserTournament"},
	{9, "WvW_EBG"},
	{10, "WvW_BBL"},
	{11, "WvW_GBL"},
	{12, "WvW_RBL"},
	{13, "WVW_FV"},
	{14, "WvW_OS"},
	{15, "WvW_EOTM"},
	{16, "Public_Mini"},
	{17, "BIG_BATTLE"},
	{18, "WvW_Lounge"},
	{19, "WvW"} // silently removed in a later patch (new wvw map?)
};
const std::map<int, std::string> mountLookup{
	{0, "Unmounted"},
	{1, "Jackal"},
	{2, "Griffon"},
	{3, "Springer"},
	{4, "Skimmer"},
	{5, "Raptor"},
	{6, "RollerBeetle"},
	{7, "Warclaw"},
	{8, "Skyscale"},
	{9, "Skiff"},
	{10, "SiegeTurtle"}
};
const std::map<int, std::string> profLookup{
	{1, "Guardian"},
	{2, "Warrior"},
	{3, "Engineer"},
	{4, "Ranger"},
	{5, "Thief"},
	{6, "Elementalist"},
	{7, "Mesmer"},
	{8, "Necromancer"},
	{9, "Revenant"}
};
const std::map<int, std::string> specLookup{
	{0, "None"},

	{27, "Dragonhunter"},
	{18, "Berserker"},
	{43, "Scrapper"},
	{5, "Druid"},
	{7, "Daredevil"},
	{48, "Tempest"},
	{40, "Chronomancer"},
	{34, "Reaper"},
	{52, "Herald"},

	{62, "Firebrand"},
	{61, "Spellbreaker"},
	{57, "Holosmith"},
	{55, "Soulbeast"},
	{58, "Deadeye"},
	{56, "Weaver"},
	{59, "Mirage"},
	{60, "Scourge"},
	{63, "Renegade"},

	{65, "Willbender"},
	{68, "Bladesworn"},
	{70, "Mechanist"},
	{72, "Untamed"},
	{71, "Specter"},
	{67, "Catalyst"},
	{66, "Virtuoso"},
	{64, "Harbinger"},
	{69, "Vindicator"}
};
const std::map<int, std::string> raceLookup{
	{0, "Asura"},
	{1, "Charr"},
	{2, "Human"},
	{3, "Norn"},
	{4, "Sylvari"}
};
const std::map<int, std::string> uiszLookup{
	{0, "Small"},
	{1, "Normal"},
	{2, "Large"},
	{3, "Larger"}
};

typedef struct LinkedMem
{
	unsigned ui_version;
	unsigned ui_tick;
	_D3DVECTOR avatar_pos;
	_D3DVECTOR avatar_front;
	_D3DVECTOR avatar_top;
	wchar_t name[256];
	_D3DVECTOR cam_pos;
	_D3DVECTOR cam_front;
	_D3DVECTOR cam_top;
	wchar_t identity[256];
	unsigned context_len;
	unsigned char serverAddress[28]; // contains sockaddr_in or sockaddr_in6
	unsigned mapId;
	unsigned mapType;
	unsigned shardId;
	unsigned instance;
	unsigned buildId;
	unsigned IsMapOpen : 1;
	unsigned IsCompassTopRight : 1;
	unsigned IsCompassRotating : 1;
	unsigned IsGameFocused : 1;
	unsigned IsCompetitive : 1;
	unsigned IsTextboxFocused : 1;
	unsigned IsInCombat : 1;
	unsigned UNUSED1 : 1;
	unsigned short compassWidth; // pixels
	unsigned short compassHeight; // pixels
	float compassRotation; // radians
	float playerX; // continentCoords
	float playerY; // continentCoords
	float mapCenterX; // continentCoords
	float mapCenterY; // continentCoords
	float mapScale;
	unsigned processId;
	unsigned char mountIndex;
	wchar_t description[2048];
} LinkedMem;

std::wstring get_mumble_name();
LinkedMem* mumble_link_create(std::wstring);
void mumble_link_destroy();