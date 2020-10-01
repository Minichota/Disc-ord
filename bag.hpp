#ifndef BAG_HPP
#define BAG_HPP

#include "disc.hpp"

#include "sleepy_discord/sleepy_discord.h"

#include <string>
#include <vector>

class bag
{
	public:
	bag(SleepyDiscord::User owner, std::vector<disc> discs, std::string thumbnail_url);
	bag() = default;
	~bag();

	void add_disc(disc d, uint8_t plasticID, uint8_t mass, WEAR wear);
	void remove_disc(size_t index);
	void set_url(std::string url);

	void sort();

	std::vector<SleepyDiscord::EmbedField> serialize();

	std::string data_dump();

	SleepyDiscord::User owner;
	std::string thumbnail_url;

	private:
	std::vector<disc> discs;
};

#endif
