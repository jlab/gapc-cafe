
#include "tools.hh"

void getline(std::istream &f, std::string &s)
{
  if (!f.good())
    return;
  char array[10 * 1024];
  f.getline(array, 10 * 1024);
  s = array;
}

#include <fstream>
#include <sstream>

std::string read_file(const std::string &name)
{
  std::ifstream f(name.c_str());
  if (!f.good())
    return "";
  std::ostringstream b;
  b << f.rdbuf();
  return b.str();
}

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

std::string mk_tempdir(const std::string &prefix)
{
  std::string a(prefix + "/XXXXXX");
  char t[a.size()+1];
  t[a.size()] = 0;
  strncpy(t, a.c_str(), a.size());
  char *r = mkdtemp(t);
  if (!r)
    throw std::runtime_error(std::string("mkdtemp failed: ") + strerror(errno));
  return r;
}
