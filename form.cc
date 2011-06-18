

#include "form.hh"

#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WPushButton>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WTabWidget>
#include <Wt/WLengthValidator>
#include <Wt/WAnchor>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <fstream>

#include "highlight.hh"

#include "tools.hh"


form::form(Wt::WContainerWidget *p)
  : Wt::WContainerWidget(p)
  , header_(0)
  , tabs_(0)

  , tracks_(1)

  , thread_(0)
{
  setId("gap_app");
  Wt::WContainerWidget *header_box = new Wt::WContainerWidget(this);
  header_box->setId("gap_app_header");
  header_ = new Wt::WText(header_box);

  input_box_ = new Wt::WContainerWidget(this);

  Wt::WContainerWidget *aouter = new Wt::WContainerWidget(this);
  Wt::WPushButton *padd = new Wt::WPushButton("+", aouter);
  padd->clicked().connect(this, &form::add_combo);
  Wt::WPushButton *psub = new Wt::WPushButton("-", aouter);
  psub->clicked().connect(this, &form::rm_combo);
  algebra_box_ = new Wt::WContainerWidget(aouter);
  algebra_box_->setId("gap_algebra_box");

  control_box_ = new Wt::WContainerWidget(this);
  Wt::WPushButton *compute_ = new Wt::WPushButton("Compute", control_box_);
  compute_->clicked().connect(this, &form::compute);
  Wt::WPushButton *ex = new Wt::WPushButton("Example", control_box_);
  ex->clicked().connect(this, &form::display_example);

  status_ = new Wt::WText(this);
  status_->setText("<div>Nothing running</div>");
  
  output_ = new Wt::WTextArea(this);
  output_->setStyleClass("gap_input");


  timer_.timeout().connect(this, &form::check_finished);
}

form::~form()
{
  if (thread_) {
    thread_->detach();
    delete thread_;
  }
}

struct adaptor : public highlight::stream_adaptor {
  std::list<std::pair<highlight::Keyword, std::ostringstream *> > streams;
  std::list<std::string> names;

  ~adaptor()
  {
    typedef std::pair<highlight::Keyword, std::ostringstream *> Pair;
    foreach (Pair p, streams) {
      delete p.second;
    }
  }

  std::ostream *create(highlight::Keyword k)
  {
    std::ostringstream *o = new std::ostringstream();
    streams.push_back(std::make_pair(k, o));
    return o;
  }
  void set_name(const std::string &n)
  {
    names.push_back(n);
  }
};

void form::create_code(const std::string &filename)
{
  adaptor a;
  highlight h(a, "grammar/" + filename);
  if (tabs_)
    removeWidget(tabs_);
  tabs_ = new Wt::WContainerWidget(this);
  Wt::WAnchor *link = new Wt::WAnchor("/grammar/" + filename,
      "Source Code: " + filename, tabs_);
  link->setTarget(Wt::TargetNewWindow);
  foreach (std::string f, files_) {
    new Wt::WText(" ", tabs_);
    std::string g(f + ".hh");
    Wt::WAnchor *l = new Wt::WAnchor("/grammar/" + g,
        g, tabs_);
    l->setTarget(Wt::TargetNewWindow);
  }
  Wt::WTabWidget *t = new Wt::WTabWidget(tabs_);
  typedef std::pair<highlight::Keyword, std::ostringstream *> Pair;
  std::list<std::string>::iterator i = a.names.begin();
  foreach (Pair p, a.streams) {
    if (!p.first)
      continue;
    std::string n;
    if (i != a.names.end())
      n = *i++;
    // XXX no memory leak because of WTabWidget adds them to 
    // an internal container, right?
    Wt::WText *text = new Wt::WText();
    std::ostringstream o;
    o << "<pre><tt>";
    o << p.second->str();
    o << "</tt></pre>";
    text->setText(o.str());
    t->addTab(text, n);
  }
}


void form::read_file(const std::string &name)
{
  src_name_ = name;
  algebras_.clear();
  example_.clear();
  files_.clear();
  header_->setText("");
  output_->setText("");
  std::string t = "grammar/" + name;
  std::ifstream f(t.c_str());
  if (!f.good())
    return;
  // XXX exception
  unsigned state = 0;
  unsigned tracks = 0;
  while (!f.eof()) {
    std::string t;
    f >> t;
    if (!f.good())
      break;
    if (t == "algebra") {
      state = 1;
      continue;
    } 
    if (t == "input") {
      state = 2;
      continue;
    }
    if (t == "EXAMPLE") {
      std::string e;
      getline(f, e);
      example_.push_back(e.substr(1));
      continue;
    }
    if (t == "import") {
      std::string e;
      getline(f, e);
      if (e.substr(1) == "rna")
        continue;
      files_.push_back(e.substr(1));
      continue;
    }
    if (t == "HEADER") {
      std::ostringstream o;
      while (!f.eof()) {
        std::string l;
        getline(f, l);
        if (!f.good())
          break;
        if (l.find("HEADER") != std::string::npos)
          break;
        o << l << '\n';
      }
      header_->setText(o.str());
      continue;
    }
    switch (state) {
      case 1 :
      algebras_.push_back(t);
      state = 0;
      break;
      case 2 :
      t = 1;
      state = 3;
      break;
      case 3 :
      if (t == ",")
        ++tracks;
      else if (t.find('>') != std::string::npos) {
        state = 0;
        tracks_ = tracks;
      }
    }
  }
  update();
  create_code(name);
}

void form::update()
{
  up_algebras();
  up_seqs();
}

void form::up_algebras()
{
  algebra_count_ = 3;
  combos_.clear();
  algebra_box_->clear();
  for (unsigned i = 0; i < algebra_count_; ++i)
    add_combo();
}

void form::up_seqs()
{
  lines_.clear();
  input_box_->clear();
  for (unsigned i = 0; i<tracks_; ++i) {
    Wt::WLineEdit *t = new Wt::WLineEdit(input_box_);
    t->setStyleClass("gap_input");
    t->setValidator(new Wt::WLengthValidator(0, 100));
    if (i<example_.size())
      t->setText(example_[i]);
    lines_.push_back(t);
  }
}

void form::add_combo()
{
  if (combos_.size() > 6)
    return;
  Wt::WComboBox *b = new Wt::WComboBox(algebra_box_);
  b->addItem("-");
  foreach(std::string alg, algebras_) {
    b->addItem(alg);
  }
  if (combos_.empty() && b->count() > 1)
    b->setCurrentIndex(1);
  combos_.push_back(b);
}

void form::rm_combo()
{
  if (combos_.empty())
    return;
  Wt::WComboBox *b = combos_.back();
  combos_.resize(combos_.size()-1);
  algebra_box_->removeWidget(b);
  delete b;
}



std::string form::mk_prod(const std::vector<std::string> &v)
{
  std::vector<std::string>::const_iterator i = v.begin();
  if (i == v.end())
    return "";
  std::ostringstream o;
  o << *i;
  ++i;
  for (; i != v.end(); ++i)
    o << " * " << *i;
  return o.str();
}

std::string form::mk_exe_name(const std::vector<std::string> &v)
{
  if (src_name_.size() < 5) {
    Wt::WApplication::instance()->log("error") << "src_name too short: "
      << src_name_;
    return "wrong_name";
  }
  std::ostringstream o;
  o << src_name_.substr(0, src_name_.size()-4);
  foreach(std::string i, v)
    o << '_' << i;
  return o.str();
}

inline bool seqsane(const std::string &p)
{
  foreach (char c, p)
    if (!( (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           c == ',' ||
           c == '+' ||
           c == '*' ||
           c == ' ' ||
           c == '_' ||
           c == '(' ||
           c == ')' 
           ) )
      return false;
  return true;
}

inline bool onlyascii(const std::string &p)
{
  foreach (char c, p)
    if (!( (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') 
           ) )
      return false;
  return true;
}

#include <boost/algorithm/string/trim.hpp>

#include <stdexcept>

void form::mk_input(std::vector<std::string> &seqs)
{
  foreach (Wt::WLineEdit* i, lines_) {
    std::string s(i->text().toUTF8());
    boost::algorithm::trim(s);
    if (s.size() > 100) {
      status_->setText("Sequence too long.");
      throw std::range_error("seq too long");
    }
    if (!seqsane(s)) {
      status_->setText("Invalid character in input.");
      throw std::range_error("invalid inp char");
    }
    seqs.push_back(s);
  }
}

void form::mk_sel(std::vector<std::string> &sel)
{
  foreach(Wt::WComboBox* i, combos_)
    if (i->currentText() == "-")
      break;
    else {
      std::string p(i->currentText().toUTF8());
      if (p.size() > 20) {
        status_->setText("Product name too long."); 
        throw std::range_error("prod too long");
      }
      if (!onlyascii(p)) {
        status_->setText("Product screwed up!");
        throw std::range_error("prod screw");
      }
      sel.push_back(p);
    }
  if (sel.empty()) {
    status_->setText("No algebras selected!");
    throw std::range_error("no sel");
  }
}

void form::cp_files(const std::string &dest)
{
  foreach(std::string f, files_) {
    try {
      fs::copy_file(fs::path("grammar/" + f + ".hh"),
          fs::path(dest + "/" + f + ".hh"),
        fs::copy_option::overwrite_if_exists);
    } catch (const std::exception &e) {
      throw std::range_error("no sel");
    }
  }
}

const unsigned form::start_interval_ = 200;

void form::compute()
{
  if (thread_) {
    status_->setText("Already computing!");
    return;
  }
  std::vector<std::string> sel;
  std::vector<std::string> seqs;
  std::string cache_dir;
  try {
    mk_sel(sel);
    mk_input(seqs);
    cache_dir = mk_tempdir("cached");
    cp_files(cache_dir);
  } catch (const std::runtime_error &e) {
    Wt::WApplication::instance()->log("error") << "comp error: " << e.what();
    status_->setText("Copy error. Inform Webmaster.");
    return;
  }
  std::string prod = mk_prod(sel);
  std::string exe_name = mk_exe_name(sel);

  thread_ = new boost::thread(&cruncher::crunch, &cruncher_, 
      src_name_, prod, exe_name, seqs, cache_dir);
  bool finished = thread_->timed_join(boost::posix_time::milliseconds(400));
  if (finished) {
    clean_thread();
    return;
  }
  timer_.setInterval(start_interval_);
  check_count_ = 0;
  timer_.start(); // -> check_finished()
}


void form::check_finished()
{
  assert(thread_);
  ++check_count_;
  update_status();
  bool finished = thread_->timed_join(boost::posix_time::milliseconds(100));
  if (finished) {
    timer_.stop();
    clean_thread();
    return;
  }
  if (check_count_ > 40) {
    Wt::WApplication::instance()->log("error") << "Check count too high";
    thread_->detach();
    delete thread_;
    thread_ = 0;
    status_->setText("Internal Error (exec), please report it to the webmaster");
  }
  timer_.setInterval(timer_.interval()*2);
}

void form::update_status()
{
  switch (cruncher_.state()) {
    case cruncher::COMPILING:
      status_->setText("Compiling.");
      break;
    case cruncher::RUNNING:
      status_->setText("Running.");
      break;
    case cruncher::COMPILE_ERROR:
      status_->setText("Compile Error (perhaps wrong product?)");
      break;
    case cruncher::RUN_ERROR:
      status_->setText("Runtime error.");
      break;
    case cruncher::SUCCESS:
      status_->setText("Execution complete.");
      break;
  }
}

void form::clean_thread()
{
  update_status();
  display_output();
  thread_->detach();
  delete thread_;
  thread_ = 0;
}


void form::display_output()
{
  output_->setText("");
  std::string n(cruncher_.temp_dir() + "/out");
  std::ifstream f(n.c_str());
  if (!f.good())
    return;
  std::ostringstream o;
  for (unsigned i = 0; i<100; ++i) {
    std::string l;
    getline(f, l);
    if (!f.good())
      break;
    o << l << '\n';
    if (f.eof())
      break;
  }
  if (f.good() && !f.eof())
    o << "<<more output cut off>>\n";
  output_->setText(o.str());

  fs::remove_all(cruncher_.temp_dir());
}


void form::display_example()
{
  up_seqs();
}

cruncher::cruncher()
  : crunch_state_(NONE), log("log/crunch.log", std::ios_base::app)
{
  if (!log.good()) {
    std::cerr << "Could not open log file.\n";
  }
}

void cruncher::crunch(const std::string &src_name, const std::string &prod, const std::string &exe_name,
    const std::vector<std::string> &seqs,
    const std::string &cache_dir)
{
  bool b = compile(src_name, prod, exe_name, cache_dir);
  if (!b)
    return;
  try {
    fs::remove_all(cache_dir);
  } catch (const std::exception &e) {
    log << "Filesystem rm error: " << e.what() << std::endl;
    crunch_state_ = COMPILE_ERROR;
    return;
  }
  run(prod, exe_name, seqs);
}

#include <process.hh>
using namespace gs;

static const char gapc_name[] = "/usr/bin/gapc";

bool cruncher::compile(const std::string &src_name_, const std::string &prod, const std::string &exe_name, const std::string &cache_dir)
{
  try {
  if (fs::exists("cached/"+ exe_name)) {
    if (fs::last_write_time(gapc_name) <= fs::last_write_time("cached/" + exe_name)
        && fs::last_write_time("grammar/" + src_name_) <= fs::last_write_time("cached/" + exe_name))
      return true;
  }
  } catch (const fs::filesystem_error &e) {
    log << "Filesystem error: " << e.what() << std::endl;
  }
  process p, m;
  p.cmd(gapc_name).alarm(60).cwd(cache_dir);
  p.arg("../../grammar/" + src_name_).arg("-p").arg(prod).arg("-t").arg("-o").arg(exe_name + ".cc");
  m.cmd("make").alarm(60).cwd(cache_dir).arg("-f").arg(exe_name + ".mf");
  crunch_state_ = COMPILING;
  try {
    p.run();
    m.run();
  } catch (const std::exception &e) {
    log << "Compile error: " << e.what() << std::endl;
    crunch_state_ = COMPILE_ERROR;
    return false;
  }
  try {
    fs::rename(fs::path(cache_dir + "/" + exe_name),
        fs::path("cached/" + exe_name));
  } catch (const std::exception &e) {
    log << "Filesystem ren error: " << e.what() << std::endl;
    crunch_state_ = COMPILE_ERROR;
    return false;
  }
  return true;
}


void cruncher::run(const std::string &prod, const std::string &exe_name,
    const std::vector<std::string> &seqs)
{
  try {
  process p;
  temp_dir_ = mk_tempdir("output");
  std::string out_name = "../" + temp_dir_ + "/out";
  p.cwd("cached").cmd("./" + exe_name).alarm(60).out(out_name);
  foreach (std::string i, seqs)
    p.arg(i);
  crunch_state_ = RUNNING;
    p.run();
  } catch (const std::exception &e) {
    crunch_state_ = RUN_ERROR;
    log << "Run error " << e.what() << std::endl;
    return;
  }
  crunch_state_ = SUCCESS;
}

