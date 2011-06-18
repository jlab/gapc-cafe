#ifndef FORM_HH
#define FORM_HH

#include <Wt/WContainerWidget>
#include <Wt/WTextArea>
#include <Wt/WPushButton>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/WTimer>
#include <Wt/WTabWidget>

#include <vector>
#include <fstream>
#include <boost/thread.hpp>

class cruncher {
  public:
    enum cstate { NONE, COMPILING, RUNNING, COMPILE_ERROR, RUN_ERROR, SUCCESS };
  private:
    cstate crunch_state_;
    std::string temp_dir_;
    std::ofstream log;

    bool compile(const std::string &src_name_, const std::string &prod,
        const std::string &exe_name, const std::string &cache_dir);
    void run(const std::string &prod, const std::string &exe_name,
        const std::vector<std::string> &seqs);
  public:
    cruncher();

    void crunch(const std::string &src_name,
        const std::string &prod, const std::string &exe_name,
        const std::vector<std::string> &seqs, const std::string &cache_dir);

    cstate state() const { return crunch_state_; }
    const std::string temp_dir() const { return temp_dir_; }
};


class form : public Wt::WContainerWidget {
  private:
    Wt::WTextArea *output_;
    Wt::WContainerWidget *input_box_, *algebra_box_, *control_box_;
    Wt::WText *status_;
    Wt::WText *header_;
    Wt::WContainerWidget *tabs_;

    std::vector<std::string> algebras_;
    std::vector<Wt::WComboBox*> combos_;

    std::vector<Wt::WLineEdit*> lines_;

    Wt::WTimer timer_;

    unsigned algebra_count_;
    unsigned tracks_;
    std::vector<std::string> example_;
    std::vector<std::string> files_;

    void update();
    void up_algebras();
    void up_seqs();
    void add_combo();
    void rm_combo();

    std::string src_name_;
    boost::thread *thread_;
    cruncher cruncher_;
    static const unsigned start_interval_;
    unsigned check_count_;

    std::string mk_prod(const std::vector<std::string> &v);
    std::string mk_exe_name(const std::vector<std::string> &v);
    void mk_sel(std::vector<std::string> &sel);
    void mk_input(std::vector<std::string> &seqs);
    void cp_files(const std::string &dest);
    void compute();


    void check_finished();
    void update_status();
    void clean_thread();
    void display_output();

    void create_code(const std::string &filename);

    void display_example();
  public:
    form(Wt::WContainerWidget *p = 0);
    ~form();
    void read_file(const std::string &name);
};

#endif
