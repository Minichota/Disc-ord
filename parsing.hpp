#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>

#include "bag.hpp"
#include "disc.hpp"

extern disc search_disc(std::string);
extern bag* search_bag(std::string owner);

extern void load_discs();
extern void load_bags();

extern void save_bags();

extern std::vector<disc> g_discs;
extern std::vector<bag>  g_bags;

#endif
