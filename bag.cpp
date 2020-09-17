#include "bag.hpp"
#include "disc.hpp"
#include "main.hpp"
#include "parsing.hpp"

#include <vector>
#include <algorithm>

using namespace SleepyDiscord;

// TODO remove
#include <iostream>

bag::bag(User owner, std::vector<disc> discs):
owner(owner),
discs(discs)
{
}

bag::~bag()
{
}

void bag::add_disc(disc d, uint8_t plasticID, uint8_t mass, WEAR wear)
{
	d.plasticID = plasticID;
	d.mass      = mass;
	d.wear      = wear;
	discs.push_back(d);
	save_bags();
}

void bag::remove_disc(size_t index)
{
	discs.erase(discs.begin() + index);
	save_bags();
}

void bag::sort()
{
	std::sort(discs.begin(), discs.end(),
			[](const disc& disc_first, const disc& disc_second)
	{
		if(disc_first.flight.speed < disc_second.flight.speed)
			return true;
		else if(disc_first.flight.speed == disc_second.flight.speed)
		{
			if(disc_first.flight.turn < disc_second.flight.turn)
				return true;
			else if(disc_first.flight.turn == disc_second.flight.turn)
			{
				return disc_first.flight.fade < disc_second.flight.fade;
			}
			return false;
		}
		return false;
	});
}

std::vector<EmbedField> bag::serialize()
{
	sort();

	std::vector<EmbedField> output;
	for(size_t i = 0; i < discs.size(); i++)
	{
		output.push_back(discs[i].serialize());
	}
	//for(size_t i = 0; i < discs.size(); i++)
	//{
	//	std::string disc_description = discs[i].serialize();
	//	std::vector<std::string> lines = split(disc_description,'\n');
	//	for(size_t j = 0; j < lines.size(); j++)
	//	{
	//		while(4 * (i / GRID_WIDTH) + j >= disc_out.size())
	//		{
	//			disc_out.emplace_back();
	//		}
	//		std::string spaces;
	//		for(int i = 0; i < max_length - lines[j].size(); i++)
	//		{
	//			spaces.push_back(' ');
	//		}
	//		disc_out[4 * (i / GRID_WIDTH) + j].append(lines[j]+spaces);
	//	}
	//}
	return output;
}

std::string bag::data_dump()
{
	auto wrap = [](std::string str){ return "\"" + str + "\"";};
	std::string out = wrap(owner.username);
	if(discs.size());
	for(disc d : discs)
	{
		auto trim = [](std::string value)->std::string
		{
			while(value.back() == '0')
				value.pop_back();
			if(value.back() == '.')
				value.pop_back();
			return value;
		};
		out.append(
				wrap(d.mold)+','+
				wrap(from_brand(d.brand))+','+
				wrap(disc::plastics[d.brand][d.plasticID])+','+
				wrap(std::to_string(d.mass))+','+
				wrap(from_wear(d.wear))+','+
				wrap(trim(std::to_string(d.flight.speed)))+','+
				wrap(trim(std::to_string(d.flight.glide)))+','+
				wrap(trim(std::to_string(d.flight.turn)))+','+
				wrap(trim(std::to_string(d.flight.fade))));
	}
	return out;
}
