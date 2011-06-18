#ifndef PAGE_HH
#define PAGE_HH

#include <Wt/WViewWidget>

class page : public Wt::WViewWidget {
  private:
    std::string filename_;
  protected:
    Wt::WWidget *renderView();
  public:
    page(Wt::WContainerWidget *p);
    void set_file(const std::string &name);
};

#endif
