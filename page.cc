

#include "page.hh"

#include <Wt/WText>
#include <Wt/WContainerWidget>

#include <fstream>
#include <sstream>

page::page(Wt::WContainerWidget *p)
  : Wt::WViewWidget(p)
{
}


void page::set_file(const std::string &name)
{
  filename_ = name;
  update();
}

Wt::WWidget *page::renderView()
{
  Wt::WContainerWidget *c = new Wt::WContainerWidget();
  c->setId("gap_page");
  Wt::WText *t = new Wt::WText(c);
  t->setText("Fix filename: " + filename_);
  if (filename_ == "")
    return c;

  std::ifstream f(filename_.c_str());
  if (!f.good())
    return c;
  std::ostringstream b;
  b << f.rdbuf();
  std::string s(b.str());
  t->setText(Wt::WString(s, Wt::UTF8));
  return c;
}
