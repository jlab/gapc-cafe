

#include "gap_app.hh"

#include <Wt/WContainerWidget>
#include <Wt/WText>

#include <Wt/WAnchor>
#include <Wt/WTemplate>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "tools.hh"

gap_app::gap_app(const Wt::WEnvironment &e)
  : Wt::WApplication(e)
{
  useStyleSheet("/resources/gap.css");
  setTitle(Wt::WString("Bellman's GAP Café", Wt::UTF8));
  //setCssTheme("polished");
  init_header();
  init_menu();
  init_right();

  internalPathChanged().connect(this, &gap_app::path_update);
}

void gap_app::init_header()
{
  Wt::WContainerWidget *h = new Wt::WContainerWidget(root());
  h->setId("gap_header");
  Wt::WText *t = new Wt::WText(h);
  t->setText(Wt::WString("<a href=\"/\">Bellman's GAP Café</a>", Wt::UTF8));
}

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>


void getsplit(std::istream &f, std::vector<std::string> &v)
{
  std::string line;
  getline(f, line);
  split(v, line, boost::is_any_of(";"));
}

void gap_app::init_menu()
{
  Wt::WTemplate *t = new Wt::WTemplate(root());
  std::string s = read_file("pages/menu.html_");
  t->setTemplateText(s, Wt::XHTMLUnsafeText);

  Wt::WContainerWidget *menu = new Wt::WContainerWidget(); // XXX parent
  Wt::WContainerWidget *sub = new Wt::WContainerWidget(menu);

  std::ifstream f("menu.rc");
  // XXX exception
  while (f.good() && !f.eof()) {
    std::vector<std::string> s;
    getsplit(f, s);
    if (s.size() != 3)
      continue;
    std::string name(s[1]), url(s[2]);
    if (s.front() == "*") {
      sub = new Wt::WContainerWidget(menu);
      Wt::WContainerWidget *head = new Wt::WContainerWidget(sub);
      head->setStyleClass("menu_h2");
      new Wt::WText(name, head);
      continue;
    }
    Wt::WAnchor *a = new Wt::WAnchor(sub);
    a->setStyleClass("menu_link");
    a->setRefInternalPath("/" + url);
    url2src["/" + url] = url + ".gap";
    a->setText(name);
  }
  t->bindWidget("apps", menu);
}


#include "form.hh"

void gap_app::init_right()
{
  form_ = new form(root());
  form_->setId("gap_rbox");

  if (!update_file(internalPath())) {
    form_->read_file("elmamun.gap");
  }
}

void gap_app::path_update(const std::string &p)
{
  update_file(p);
}

inline bool nospacesane(const std::string &p)
{
  foreach (char c, p)
    if (!( (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') || c == '/' ) )
      return false;
  return true;
}

bool gap_app::update_file(const std::string &p)
{
  if (!form_)
    return false;
  if (p.size() > 15)
    return false;
  if (!nospacesane(p))
    return false;

  std::map<std::string, std::string>::iterator i =
      url2src.find(p);
  if (i != url2src.end()) {
      form_->read_file(i->second);
      return true;
  }

  return false;
}

