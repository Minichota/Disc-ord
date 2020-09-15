#include "main.hpp"
#include "parsing.hpp"

#define DISC_FILE "../discs_1.csv"
#define BAG_FILE "../bags.csv"

#include <algorithm>
#include <fstream>
#include <streambuf>

// TODO remove
#include <iostream>

std::vector<disc> g_discs;
std::vector<bag> g_bags;

disc search_disc(std::string name)
{
	auto result = std::find_if(g_discs.begin(), g_discs.end(),
			[name](const disc& x){ return case_switch(x.mold) == case_switch(name); });
	if(result != g_discs.end())
	{
		return *result.base();
	}
	else
	{
		log_data("couldn't find disc by: " + name);
	}
	return disc();
}

bag* search_bag(std::string owner)
{
	auto result = std::find_if(g_bags.begin(), g_bags.end(),
			[owner](const bag& x){ return case_switch(x.owner) == case_switch(owner); });
	if(result != g_bags.end())
	{
		return result.base();
	}
	else
	{
		log_data("couldn't find bag owned by: " + owner);
	}
	return nullptr;
}

void load_discs()
{
	std::ifstream file(DISC_FILE);
	std::string file_data((std::istreambuf_iterator<char>(file)),
					 std::istreambuf_iterator<char>());

	char* location = &file_data[0];
	auto next_string = [&location]() -> std::string
	{
		std::string result;
		// runs into two quotes then breaks
		bool counter = false;
		while(*location != '\0')
		{
			if(*location == '"' && !counter)
				counter = true;
			else if(*location == '"' && counter)
			{
				// increment past last quote
				location++;
				break;
			}
			else if(counter)
			{
				result.push_back(*location);
			}
			location++;
		}
		return result;
	};

	while(location != &file_data.back())
	{
		// next string must be called 9 times
		next_string();
		BRAND b = to_brand(next_string());
		std::string mold = next_string();
		disc d(b, mold, 0, 0, NIL);

		d.flight = flight_path { std::stof(next_string()),
					 std::stof(next_string()),
					 std::stof(next_string()),
					 std::stof(next_string()) };
		g_discs.push_back(d);
		next_string();
		next_string();
	}
}

void load_bags()
{
	std::ifstream file(BAG_FILE);
	std::string file_data((std::istreambuf_iterator<char>(file)),
					 std::istreambuf_iterator<char>());

	char* location = &file_data[0];
	auto next_string = [&location]() -> std::string
	{
		std::string result;
		// runs into two quotes then breaks
		bool counter = false;
		while(*location != '\0')
		{
			if(*location == '"' && !counter)
				counter = true;
			else if(*location == '"' && counter)
			{
				// increment past last quote
				location++;
				break;
			}
			else if(counter)
			{
				result.push_back(*location);
			}
			location++;
		}
		return result;
	};

	while(location != &file_data.back())
	{
		std::string owner = next_string();
		std::vector<disc> discs;
		while(*location != '\n')
		{
			std::string mold  = next_string();
			std::string brand = next_string();
			size_t plastic    = to_plasticID(to_brand(brand), next_string());
			uint8_t mass      = std::stoi(next_string());
			WEAR wear         = to_wear(next_string());
			disc d(to_brand(brand), mold, plastic, mass, wear);
			d.flight = flight_path { std::stof(next_string()),
						 std::stof(next_string()),
						 std::stof(next_string()),
						 std::stof(next_string()) };
			discs.push_back(d);
		}
		g_bags.push_back(bag(owner, discs));
	}
}

void save_bags()
{
	std::string out;
	for(bag b : g_bags)
	{
		out.append(b.data_dump()+'\n');
	}
	std::ofstream file(BAG_FILE);
	file << out;
}
