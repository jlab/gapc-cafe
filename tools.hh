#ifndef TOOLS_HH
#define TOOLS_HH

#include <iostream>
#include <string>

void getline(std::istream &f, std::string &s);
std::string read_file(const std::string &name);
std::string mk_tempdir(const std::string &prefix);

#endif
