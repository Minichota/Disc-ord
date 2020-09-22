#include "disc.hpp"
#include "main.hpp"
#include "parsing.hpp"

#include <algorithm>

std::map<BRAND, std::vector<std::string>> disc::plastics = {
	{ INNOVA,         { "null","star","starlite","echo_star","gstar","champion","metal_flake","blizzard","xt","driver_pro","kc_pro","r_pro","jk_pro","dx" } },
	{ PRODIGY,        { "null","200","300","350","350g","400","400g","450","500","750","750g" } },
	{ DISCRAFT,       { "null","x","z","elite_z","big_z","pro_d","jawbreaker","x_soft","crazy_tuff","z_flx","titanium","z_lite","esp" } },
	{ DISCMANIA,      { "null","d_line","c_line","s_line","p_line","g_line","x_line","active","active_premium","exo_soft","exo_hard","neo","lux" } },
	{ INFINITE_DISCS, { "null","s_blend","c_blend","g_blend","i_blend","d_blend","x_blend","p_blend"} },
	{ MVP,            { "null","neutron","cosmic_neutron","proton","plasma","eclipse","fission","electron","cosmic_electron" } },
	{ AXIOM,          { "null","neutron","cosmic_neutron","proton","plasma","eclipse","fission","electron","cosmic_electron" } },
	{ RPM,            { "null","atomic","cosmic","magma","magma_soft","strata","platinum" } },
	{ DYNAMIC_DISCS,  { "null","moon_shine","fuzion","lucid","lucid_air","fuzion_bio","prime","classic","classic_blend","classic_soft","classic_ss" } },
	{ LATITUDE_64,    { "null","moon_shine","gold_line","opto_line","opto_air","reprocessed","mega_soft","zero_soft","zero_medium","zero_hard","retro_line" } },
	{ WESTSIDE,       { "null","moon_shine","tournament","vip","vip_air","elasto","tournament_r","mega_soft","bt_soft","bt_medium","bt_hard","origio" } }
	{ KASTAPLAST,     { "null","k1","k1_soft","k3","k1_glow" } }
};

disc::disc(BRAND brand, std::string mold, size_t plasticID, uint8_t mass, WEAR wear):
	brand(brand),
	mold(mold),
	plasticID(plasticID),
	mass(mass),
	wear(wear)
{
}

disc::~disc()
{
}

using namespace SleepyDiscord;
EmbedField disc::serialize()
{
	auto trim = [](std::string value)->std::string
	{
		while(value.back() == '0')
			value.pop_back();
		if(value.back() == '.')
			value.pop_back();
		return value;
	};
	auto replace_underscore = [](std::string value)->std::string
	{
		auto index = std::find(value.begin(), value.end(), '_');
		while(index != value.end())
		{
			*index.base() = ' ';
			index = std::find(value.begin(), value.end(), '_');
		}
		return value;
	};

	EmbedField output(case_switch(mold)+'\n',

		trim(std::to_string(flight.speed))+'/'+
		trim(std::to_string(flight.glide))+'/'+
		trim(std::to_string(flight.turn))+'/'+
		trim(std::to_string(flight.fade))+'\n'+
		replace_underscore(case_switch((wear == NIL ? "" : from_wear(wear) + " ")))+
		replace_underscore((plasticID != 0 ? case_switch(plastics[brand][plasticID]) + ' ' : ""))+
		(mass == 0 ? "" : std::to_string(mass)+'g'+" "),
		true
	);
	return output;
}

size_t to_plasticID(BRAND brand, std::string plastic)
{
	std::vector<std::string> plastic_list = disc::plastics[brand];
	for(std::string& str : plastic_list)
	{
		str = case_switch(str);
	}
	auto idx = std::find(plastic_list.begin(), plastic_list.end(), case_switch(plastic));
	if(idx == plastic_list.end())
	{
		log_data("failed to find a plastic");
		return -1;
	}
	return std::distance(plastic_list.begin(), idx);
}

std::string from_brand(BRAND brand)
{
	static std::map<BRAND, std::string> brand_s_map =
	{
		{DGA, "DGA"},
		{DISC_KING, "disc king"},
		{DISCMANIA, "discmania"},
		{DISCRAFT, "discraft"},
		{DYNAMIC_DISCS, "dynamic discs"},
		{ELEMENT_DISCS, "element discs"},
		{FLIGHT_HIGH_DISCS, "flight high discs"},
		{FULL_TURN_DISCS, "full turn discs"},
		{GALAXY_DISC_GOLF, "galaxy disc golf"},
		{GATEWAY_DISC_SPORTS, "gateway disc sports"},
		{GURU_DISC_GOLF, "guru disc golf"},
		{INFINITE_DISCS, "infinite discs"},
		{INNOVA, "innova"},
		{KASTAPLAST, "kastaplast"},
		{KESTREL_OUTDOORS, "kestrel outdoors"},
		{LATITUDE_64, "latitude 64"},
		{LEGACY_DISCS, "legacy discs"},
		{LIGHTNING_DISCS, "lightning discs"},
		{MILLENNIUM, "millennium"},
		{HYZER_BOMB, "hyzer bomb"},
		{MINT_DISCS, "mint discs"},
		{MVP, "MVP"},
		{OBSIDIAN_DISCS, "obsidian discs"},
		{OZONE_DISCS, "ozone discs"},
		{PLASTIC_ADDICTS, "plastic addicts"},
		{PRODIGY, "prodigy"},
		{PRODISCUS, "prodiscus"},
		{REPTILIAN_DISC_GOLF, "reptilian disc golf"},
		{RPM, "RPM"},
		{SKYQUEST_DISCS, "skyquest discs"},
		{STORM_DISC_GOLF, "storm disc golf"},
		{STREAMLINE, "streamline"},
		{TOBU_DISCS, "tobu discs"},
		{VIKING_DISCS, "viking discs"},
		{WESTSIDE, "westside"},
		{YIKUN_SPORTS, "yikun sports"},
		{AXIOM, "axiom"},
		{DISC_GOLF_UK, "disc golf uk"},
		{TOKYO_DISCS, "tokyo discs"},
		{ABOVE_GROUND_LEVEL, "above ground level"},
		{BLACK_ZOMBIE_DISC_GOLF, "black zombie disc golf"},
		{ABC_DISCS, "abc discs"},
		{AEROBIE, "aerobie"},
		{CHING_SPORTS, "ching sports"},
		{DEITY_DISCS, "deity discs"},
		{RIP_DISC_GOLF, "rip disc golf"},
		{SALIENT_DISCS, "salient discs"},
		{SKYIRON, "skyiron"},
		{UB_DISC_GOLF_HAND_CANDY, "ub disc golf - hand candy"},
		{VIBRAM_DISC_GOLF, "vibram disc golf"},
		{THOUGHT_SPACE_ATHLETICS, "Thought Space Athletics"},
		{WESTSIDE, "Westside"},
		{DAREDEVIL_DISCS, "Daredevil Discs"},
	};
	return brand_s_map[brand];
}

BRAND to_brand(std::string str)
{
	static std::map<std::string, BRAND> brand_map =
	{
		{"DGA", DGA},
		{"disc king", DISC_KING},
		{"discmania", DISCMANIA},
		{"discraft", DISCRAFT},
		{"dynamic discs", DYNAMIC_DISCS},
		{"element discs", ELEMENT_DISCS},
		{"flight high discs", FLIGHT_HIGH_DISCS},
		{"full turn discs", FULL_TURN_DISCS},
		{"galaxy disc golf", GALAXY_DISC_GOLF},
		{"gateway disc sports", GATEWAY_DISC_SPORTS},
		{"guru disc golf", GURU_DISC_GOLF},
		{"infinite discs", INFINITE_DISCS},
		{"innova", INNOVA},
		{"kastaplast", KASTAPLAST},
		{"kestrel outdoors", KESTREL_OUTDOORS},
		{"latitude 64", LATITUDE_64},
		{"legacy discs", LEGACY_DISCS},
		{"lightning discs", LIGHTNING_DISCS},
		{"millennium", MILLENNIUM},
		{"hyzer bomb", HYZER_BOMB},
		{"mint discs", MINT_DISCS},
		{"MVP", MVP},
		{"obsidian discs", OBSIDIAN_DISCS},
		{"ozone discs", OZONE_DISCS},
		{"plastic addicts", PLASTIC_ADDICTS},
		{"prodigy", PRODIGY},
		{"prodiscus", PRODISCUS},
		{"reptilian disc golf", REPTILIAN_DISC_GOLF},
		{"RPM", RPM},
		{"skyquest discs", SKYQUEST_DISCS},
		{"storm disc golf", STORM_DISC_GOLF},
		{"streamline", STREAMLINE},
		{"tobu discs", TOBU_DISCS},
		{"viking discs", VIKING_DISCS},
		{"westside", WESTSIDE},
		{"yikun sports", YIKUN_SPORTS},
		{"axiom", AXIOM},
		{"disc golf uk", DISC_GOLF_UK},
		{"tokyo discs", TOKYO_DISCS},
		{"above ground level", ABOVE_GROUND_LEVEL},
		{"black zombie disc golf", BLACK_ZOMBIE_DISC_GOLF},
		{"abc discs", ABC_DISCS},
		{"aerobie", AEROBIE},
		{"ching sports", CHING_SPORTS},
		{"deity discs", DEITY_DISCS},
		{"rip disc golf", RIP_DISC_GOLF},
		{"salient discs", SALIENT_DISCS},
		{"skyiron", SKYIRON},
		{"ub disc golf - hand candy", UB_DISC_GOLF_HAND_CANDY},
		{"vibram disc golf", VIBRAM_DISC_GOLF},
		{"Thought Space Athletics", THOUGHT_SPACE_ATHLETICS},
		{"Westside", WESTSIDE},
		{"Daredevil Discs", DAREDEVIL_DISCS},
		{"INNOVA", INNOVA}
	};
	return brand_map[str];
}

std::string from_wear(WEAR wear)
{
	std::map<WEAR, std::string> map =
	{
		{ NEW,     "new" },
		{ BEAT_IN, "beat_in" },
		{ FLIPPY,  "flippy" },
		{ NIL,    "null" }
	};
	return map[wear];
}

WEAR to_wear(std::string str)
{
	std::map<std::string, WEAR> map =
	{
		{ "new",     NEW },
		{ "beat_in", BEAT_IN },
		{ "flippy",  FLIPPY },
		{ "null",    NIL }
	};
	return map[str];
}

#include <iostream>

std::vector<disc> flight_search(const std::string& path)
{
	auto check_fits = [&path](const disc& disc) -> bool
	{
		std::vector<std::string> paths = split(path,'/');
		if(paths[0] != "x")
		{
			if(std::stof(paths[0]) != disc.flight.speed)
				return false;
		}
		if(paths[1] != "x")
		{
			if(std::stof(paths[1]) != disc.flight.glide)
				return false;
		}
		if(paths[2] != "x")
		{
			if(std::stof(paths[2]) != disc.flight.turn)
				return false;
		}
		if(paths[3] != "x")
		{
			if(std::stof(paths[3]) != disc.flight.fade)
				return false;
		}
		return true;
	};
	std::vector<disc> fitting_discs = {};
	for(const disc& d : g_discs)
	{
		if(check_fits(d))
		{
			fitting_discs.push_back(d);
		}
	}
	return fitting_discs;
}
