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
	BAG,
	IBAG,
	UNBAG,
	FLIGHT_SEARCH
};

const std::vector<std::pair<command_ID,std::string>> commands {
	{ DISC, "disc" },
	{ BAG, "bag" },
	{ IBAG, "ibag" },
	{ UNBAG, "unbag" },
	{ FLIGHT_SEARCH, "flight_search" }
};

size_t parse_command(std::string text) {
	for (size_t i = 0; i < commands.size(); i++) {
		if(split(text, ' ')[0] == commands[i].second) {
			return i;
		}
	}
	return -1;
}

void handle_commands(Embed& output, std::pair<command_ID,std::string> command)
{
	auto trim = [](std::string value)->std::string
	{
		while(value.front() == ' ')
			value.erase(value.begin());
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
				output.fields.push_back(d.serialize());
			}
			return;
		}
		break;
		case BAG:
		{
			// COMMAND, USER
			if(words.size() <= 1)
			{
				output.fields = {EmbedField("you fool", "don't forget the owner's name")};
				return;
			}
			bag* b = search_bag(words[1]);
			// check if not found
			if(b == nullptr)
			{
				output.fields = {EmbedField("you fool", "this bag doesn't exist")};
				return;
			}

			output.fields = b->serialize();
			output.title = b->owner;
			return;
		}
		break;
		case IBAG:
		{
			// COMMAND, USER, MOLD, [PLASTIC], [MASS], [WEAR], [FLIGHT, x/x/x/x]
			disc d = search_disc(words[2]);
			if(words.size() == 7)
			{
				std::vector<std::string> flight = split(words[6],'/');
				d.flight = flight_path {
					std::stof(flight[0]),
					std::stof(flight[1]),
					std::stof(flight[2]),
					std::stof(flight[3])
				};
			}
			bag* b = search_bag(words[1]);
			b->add_disc(d, words.size() == 3 ? to_plasticID(d.brand, "null"): to_plasticID(d.brand, words[3]),
						words.size() >= 4 ? std::stoi(words[4]) : NULL, words.size() >= 5 ? to_wear(words[5]) : NIL);
			//return "bagged that shit homie";
		}
		break;
		case UNBAG:
		{
			// COMMAND, USER, INDEX
			bag* b = search_bag(words[1]);
			b->remove_disc(std::stoi(words[2]));
			//return "get that shit out of here";
		}
		break;
		case FLIGHT_SEARCH:
		{
			// COMMAND, FLIGHT, [x/\x]/[x/\x]/[x/\x]/[x/\x]
			std::vector<disc> discs = flight_search(words[1]);
			std::string out = "```\n";
			for(disc d : discs)
			{
				out.append(case_switch(d.mold)+'\n');
			}
			if(discs.empty())
			{
				out = "are you sure any discs like that exist?";
			}
			else
			{
				out.append("```");
			}
			//return out;
		}
		break;
	}
	//return "command found but not handled!";
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
				handle_commands(e, std::pair<command_ID,std::string>{(command_ID)command, trimmed_content});
				sendMessage(message.channelID, "", e);
				//if(output.size() >= 2000)
				//{
				//	sendMessage(message.channelID, "the command you sent returned an output beyond Discord's messaging limit, you god damn genius");
				//}
				//else
				//{
				//	sendMessage(message.channelID, output);
				//}
			}
			else
			{
				sendMessage(message.channelID, "command not found :(");
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
