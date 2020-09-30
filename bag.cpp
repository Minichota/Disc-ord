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

	auto insert_index = [&](disc d, EmbedField field)
	{
		auto index = std::find(discs.begin(), discs.end(), d);
		field.name = std::to_string(std::distance(discs.begin(), index)) + " " + field.name;
		return field;
	};

	std::vector<EmbedField> output;
	std::vector<disc> putters;
	std::vector<disc> mids;
	std::vector<disc> fairways;
	std::vector<disc> distance;
	for(size_t i = 0; i < discs.size(); i++)
	{
		if(0 <= discs[i].flight.speed && discs[i].flight.speed <= 3) { putters.push_back(discs[i]); }
		else if(4 <= discs[i].flight.speed && discs[i].flight.speed <= 6) { mids.push_back(discs[i]); }
		else if(7 <= discs[i].flight.speed && discs[i].flight.speed <= 10) { fairways.push_back(discs[i]); }
		else if(11 <= discs[i].flight.speed) { distance.push_back(discs[i]); }
	}
	if(putters.size() > 0)
	{
		output.push_back(EmbedField("PUTTERS","--------------------"));
		for(disc d : putters)
		{
			output.push_back(insert_index(d, d.serialize()));
		}
	}
	if(mids.size() > 0)
	{
		output.push_back(EmbedField("MID-RANGES","--------------------"));
		for(disc d : mids)
		{
			output.push_back(insert_index(d, d.serialize()));
		}
	}
	if(fairways.size() > 0)
	{
		output.push_back(EmbedField("FAIRWAYS","--------------------"));
		for(disc d : fairways)
		{
			output.push_back(insert_index(d, d.serialize()));
		}
	}
	if(distance.size() > 0)
	{
		output.push_back(EmbedField("DRIVERS","--------------------"));
		for(disc d : distance)
		{
			output.push_back(insert_index(d, d.serialize()));
		}
	}
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
