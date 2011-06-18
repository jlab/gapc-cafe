
#include <Wt/WApplication>

#include "gap_app.hh"

Wt::WApplication *create_app(const Wt::WEnvironment& env)
{
  return new gap_app(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &create_app);
}
