#ifndef DEF_MAIN
#define DEF_MAIN

#include <string>
#include <map>

void getDirectory(std::string &directory, const char *path);
void convertPath(std::string &file, const char *path);
bool loadSettings(std::string const& directory, std::map <std::string, int> &settings);
bool saveSettings(std::string const& directory, std::map <std::string, int> &settings);

#endif
