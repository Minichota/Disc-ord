#ifndef BAG_HPP
#define BAG_HPP

#include "disc.hpp"

#include "sleepy_discord/sleepy_discord.h"

#include <string>
#include <vector>

class bag
{
	public:
	bag(std::string owner, std::vector<disc> discs);
	bag() = default;
	~bag();

	void add_disc(disc d, uint8_t plasticID, uint8_t mass, WEAR wear);
	void remove_disc(size_t index);

	void sort();

	std::vector<SleepyDiscord::EmbedField> serialize();

	std::string data_dump();

	std::string owner;

	private:
	std::vector<disc> discs;
};

#endif
