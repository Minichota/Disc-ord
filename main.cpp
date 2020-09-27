#include <iostream>
#include <fstream>
#include <ctime>

#include "sleepy_discord/sleepy_discord.h"

#include "bag.hpp"
#include "disc.hpp"
#include "parsing.hpp"

#define TOKEN "."
#define LOG_FILE "../logs"

using namespace SleepyDiscord;

void log_data(std::string data)
{
	std::ofstream file(LOG_FILE, std::ios_base::app);
	static bool logged = false;
	if(!logged)
	{
		logged = true;
		// prepend log file with current time-stamp
		time_t now = time(0);
		std::string time = ctime(&now);

		// remove \n
		time.pop_back();
		file << "\n\n" << time;
	}
	file << '\n' << data;
	file.close();
}

std::vector<std::string> split(std::string str, const char delim)
{
	std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + 1;
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

std::string case_switch(std::string str) {
	for(char& c : str)
	{
		c = ::tolower(c);
	}

	str[0] = ::toupper(str[0]);
	return str;
};


enum command_ID
{
	DISC = 0,
	ADD_BAG,
	BAG,
	IBAG,
	UNBAG,
	FLIGHT_SEARCH,
	CRASH,
	GIT_GUD,
	BRANDS,
	PLASTICS
};

const std::vector<std::pair<command_ID,std::string>> commands {
	{ DISC, "disc" },
	{ ADD_BAG, "add_bag" },
	{ BAG, "bag" },
	{ IBAG, "ibag" },
	{ UNBAG, "unbag" },
	{ FLIGHT_SEARCH, "flight_search" },
	{ CRASH, "crash" },
	{ GIT_GUD, "git_gud" },
	{ BRANDS, "brands" },
	{ PLASTICS, "plastics" }
};

size_t parse_command(std::string text) {
	for (size_t i = 0; i < commands.size(); i++) {
		if(split(text, ' ')[0] == commands[i].second) {
			return i;
		}
	}
	return -1;
}

void handle_commands(Embed& output, User owner, std::pair<command_ID,std::string> command)
{
	auto trim = [](std::string value)->std::string
	{
		while(value.front() == ' ')
			value.erase(value.begin());
		return value;
	};
	auto trim_float = [](std::string value)->std::string
	{
		while(value.back() == '0')
			value.pop_back();
		if(value.back() == '.')
			value.pop_back();
		return value;
	};
	std::vector<std::string> words = split(command.second, ' ');
	switch(command.first)
	{
		case DISC:
		{
			// COMMAND, [DISCs]
			if(words.size() == 1)
			{
				log_data("attempt to query with ==1 words in command: DISC");
				output.fields = {EmbedField("you fool", "you must supply more than just command")};
				return;
			}

			// resplit data by comma TODO fix really hacky solution
			std::string disc_list;
			std::vector<std::string> discs(words.size() - 1);
			std::copy(words.begin() + 1, words.end(), discs.begin());
			for(std::string s : discs)
			{
				disc_list.append(s);
				disc_list.push_back(' ');
			}
			disc_list.pop_back();

			discs.clear();

			// splitting by comma
			discs = split(disc_list, ',');

			for(size_t i = 0; i < discs.size(); i++)
			{
				// remove leading space
				discs[i] = trim(discs[i]);

				disc d = search_disc(discs[i]);
				// check if not found
				if(d.mold == "")
				{
					log_data("attempt to query non existant disc"+discs[i]);
					output.fields.push_back(EmbedField("you fool", "disc not found, name: "+discs[i]));
					continue;
				}
				output.fields.push_back(d.serialize_simple());
			}
			return;
		}
		break;
		case ADD_BAG:
		{
			g_bags.push_back(bag(owner, {}));
			output.fields = {EmbedField("new bag", "your bag was created")};
			save_bags();
			return;
		}
		break;
		case BAG:
		{
			// COMMAND, USER
			bag* b = search_bag(owner);
			// check if not found
			if(b == nullptr)
			{
				output.fields = {EmbedField("you fool", "this bag doesn't exist")};
				return;
			}

			output.fields = b->serialize();
			output.title = b->owner.username;
			return;
		}
		break;
		case IBAG:
		{
			// COMMAND, MOLD, [PLASTIC], [FLIGHT, x/x/x/x], [WEAR], [MASS]

			// resplit data by comma TODO fix really hacky solution
			std::string field_list;
			std::vector<std::string> fields(words.size() - 1);
			std::copy(words.begin() + 1, words.end(), fields.begin());
			for(std::string s : fields)
			{
				field_list.append(s);
				field_list.push_back(' ');
			}
			field_list.pop_back();

			fields.clear();

			// splitting by comma
			fields = split(field_list, ',');

			disc d = search_disc(trim(fields[0]));
			if(d == disc())
			{
				// ERROR
				output.fields = {EmbedField("you fool", "that disc wasn't found")};
				return;
			}
			if(fields.size() >= 4)
			{
				std::vector<std::string> flight = split(trim(fields[2]),'/');
				d.flight = flight_path {
					std::stof(flight[0]),
					std::stof(flight[1]),
					std::stof(flight[2]),
					std::stof(flight[3])
				};
			}
			bag* b = search_bag(owner);
			if(b == nullptr)
			{
				// ERROR
				output.fields = {EmbedField("you fool", "you don't own a bag")};
				return;
			}
			// plastic, mass, wear
			if(d.brand != null && to_plasticID(d.brand, "null") == (size_t)-1)
			{
				// ERROR
				output.fields = {EmbedField("you fool", "that brand isn't implemented yet for bags.")};
				return;
			}
			b->add_disc(d, fields.size() >= 2 ? to_plasticID(d.brand, trim(fields[1])) : to_plasticID(d.brand, "null"),
						fields.size() == 5 ? std::stoi(trim(fields[4])) : NULL,
						fields.size() >= 4 ? to_wear(trim(fields[3])) : NIL);
			output.fields = {EmbedField("good job", "disc added to bag")};
			return;
		}
		break;
		case UNBAG:
		{
			// COMMAND, INDEX
			bag* b = search_bag(owner);
			b->remove_disc(std::stoi(words[1]));

			output.fields = {EmbedField("good job", "disc removed from bag")};
			return;
		}
		break;
		case FLIGHT_SEARCH:
		{
			// COMMAND, FLIGHT, [x/\x]/[x/\x]/[x/\x]/[x/\x]
			std::vector<disc> discs = flight_search(words[1]);
			for(disc d : discs)
			{
				output.fields.push_back(EmbedField(case_switch(d.mold),
							trim_float(std::to_string(d.flight.speed))+"/"+
							trim_float(std::to_string(d.flight.glide))+"/"+
							trim_float(std::to_string(d.flight.turn))+"/"+
							trim_float(std::to_string(d.flight.fade))));
			}
			if(discs.empty())
			{
				output.fields = {EmbedField("you fool", "are you sure any discs like that exist?")};
			}
			return;
		}
		break;
		case CRASH:
		{
			exit(0);
		}
		break;
		case GIT_GUD:
		{
			output.fields = {
				EmbedField(".disc <mold(s) (comma seperated)>","returns a discs properties."),
				EmbedField(".add_bag","creates a bag for you!"),
				EmbedField(".bag","shows you your very own bag and all of it's fanciness."),
				EmbedField(".ibag <mold>, <plastic>, <flight_numbers>,"
							"<wear and tear (new, beat_in, and flippy)>, <mass>","adds a disc to your bag."
							"The fields are optional so you can supply less than shown above."),
				EmbedField(".unbag <index>","unbags a disc from your bag by index. It starts at 0."),
				EmbedField(".flight_search x/x/x/x","returns all discs by supplied numbers. you can leave an x"
							"in one of the spaces: x/x/0/0 to have any number fit."),
				EmbedField(".crash","restarts the bot and is only used by me to update it really."),
				EmbedField(".brands","lists all currently implemented brands for bags."),
				EmbedField(".plastics <brand>","lists all plastics for a brand.")
			};
		}
		break;
		case BRANDS:
		{
			std::string brand_list;
			for(std::map<BRAND,std::vector<std::string>>::iterator it = disc::plastics.begin(); it != disc::plastics.end(); ++it)
			{
				brand_list.append(from_brand(it->first)+", ");
			}
			brand_list.erase(brand_list.end() - 2, brand_list.end());
			output.fields.push_back(EmbedField("brands",brand_list));
		}
		break;
		case PLASTICS:
		{
			std::string plastic_list;
			if(disc::plastics.find(to_brand(words[1])) != disc::plastics.end())
			{
				for(std::string s : disc::plastics[to_brand(words[1])])
				{
					plastic_list.append(s+", ");
				}
				plastic_list.erase(plastic_list.end() - 2, plastic_list.end());
				output.fields.push_back(EmbedField("plastics for brand: "+words[1],plastic_list));
			}
			else
			{
				output.fields = {EmbedField("you fool", "that brand hasn't been implemented yet.")};
			}
		}
		break;
	}
}

class MyClientClass : public SleepyDiscord::DiscordClient {
	public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override {
		if(message.startsWith(TOKEN)) {
			std::string trimmed_content = message.content.substr(1, message.content.size());
			size_t command = parse_command(trimmed_content);
			if(command != -1)
			{
				Embed e;
				handle_commands(e, message.author, std::pair<command_ID,std::string>{(command_ID)command, trimmed_content});
				sendMessage(message.channelID, "", e);
			}
			else
			{
				log_data("couldn't parse command: "+trimmed_content);
			}
		}
	}
};

int main() {
	// init
	load_discs();
	load_bags();

	// BOT
	// I'm going to leave the bot token as the one it was before and not commit this part,
	// I don't know the best way to do this so whatever
	client.run();

	return 0;
}
