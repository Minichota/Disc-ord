#ifndef DISC_HPP
#define DISC_HPP

#include <map>
#include <string>
#include <vector>

#include "sleepy_discord/sleepy_discord.h"

#include "main.hpp"

enum BRAND
{
	null,
	DGA,
	DISC_KING,
	DISCMANIA,
	DISCRAFT,
	DYNAMIC_DISCS,
	ELEMENT_DISCS,
	FLIGHT_HIGH_DISCS,
	FULL_TURN_DISCS,
	GALAXY_DISC_GOLF,
	GATEWAY_DISC_SPORTS,
	GURU_DISC_GOLF,
	INFINITE_DISCS,
	INNOVA,
	KASTAPLAST,
	KESTREL_OUTDOORS,
	LATITUDE_64,
	LEGACY_DISCS,
	LIGHTNING_DISCS,
	MILLENNIUM,
	HYZER_BOMB,
	MINT_DISCS,
	MVP,
	OBSIDIAN_DISCS,
	OZONE_DISCS,
	PLASTIC_ADDICTS,
	PRODIGY,
	PRODISCUS,
	REPTILIAN_DISC_GOLF,
	RPM,
	SKYQUEST_DISCS,
	STORM_DISC_GOLF,
	STREAMLINE,
	TOBU_DISCS,
	VIKING_DISCS,
	YIKUN_SPORTS,
	AXIOM,
	DISC_GOLF_UK,
	TOKYO_DISCS,
	ABOVE_GROUND_LEVEL,
	BLACK_ZOMBIE_DISC_GOLF,
	ABC_DISCS,
	AEROBIE,
	CHING_SPORTS,
	DEITY_DISCS,
	RIP_DISC_GOLF,
	SALIENT_DISCS,
	SKYIRON,
	UB_DISC_GOLF_HAND_CANDY,
	VIBRAM_DISC_GOLF,
	THOUGHT_SPACE_ATHLETICS,
	DAREDEVIL_DISCS,
	WESTSIDE
};

enum WEAR
{
	NEW,
	BEAT_IN,
	FLIPPY,
	NIL
};


extern size_t to_plasticID(BRAND brand, std::string plastic);
extern std::string from_brand(BRAND brand);
extern BRAND to_brand(std::string str);
extern std::string from_wear(WEAR wear);
extern WEAR to_wear(std::string str);


struct flight_path
{
	float speed;
	float glide;
	float turn;
	float fade;

	flight_path() = default;
	flight_path(const flight_path& path):
	speed{path.speed},
	glide{path.glide},
	turn{path.turn},
	fade{path.fade}
	{
	}
	flight_path(const float speed, const float glide, const float turn, const float fade):
	speed{speed},
	glide{glide},
	turn{turn},
	fade{fade}
	{
	}
};

class disc
{
	public:
	disc(BRAND brand, std::string mold, size_t plasticID, uint8_t mass, WEAR wear);
	disc() = default;
	~disc();

	SleepyDiscord::EmbedField serialize();
	SleepyDiscord::EmbedField serialize_simple();

	//
	std::string mold;
	flight_path flight;
	uint8_t mass;
	WEAR wear;


	BRAND brand = null;
	size_t plasticID;


	public:
	static
	std::map<BRAND, std::vector<std::string>> plastics;

	bool operator ==(const disc d)
	{

		bool check = d.mold == this->mold &&
			   d.flight.speed == this->flight.speed &&
			   d.flight.glide == this->flight.glide &&
			   d.flight.turn == this->flight.turn &&
			   d.flight.fade == this->flight.fade;
		if(d.brand != NULL)
		{
			check = check && plastics[d.brand][d.plasticID] == plastics[this->brand][this->plasticID];
		}
		return check;
	}
};

extern std::vector<disc> flight_search(const std::string& path);

#endif
