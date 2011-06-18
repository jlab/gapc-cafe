#ifndef GAP_APP_HH
#define GAP_APP_HH

#include <Wt/WApplication>

#include <map>

class form;

class gap_app : public Wt::WApplication {

  private:
    void init_header();
    void init_menu();
    void init_right();
    form *form_;

    std::map<std::string, std::string> url2src;

    void path_update(const std::string &p);
    bool update_file(const std::string &p);
  public:
    gap_app(const Wt::WEnvironment &e);
};

#endif
