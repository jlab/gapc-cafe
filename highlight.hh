

#ifndef HIGHLIGHT_HH
#define HIGHLIGHT_HH

#include <iostream>
#include <string>
#include <vector>



class highlight {
  public:
    enum Keyword { NONE, ALGEBRA, GRAMMAR, SIGNATURE };
    struct stream_adaptor {
      virtual std::ostream *create(Keyword k) = 0;
      virtual void set_name(const std::string &n) = 0;
    };
  private:
    Keyword last_kw;
    unsigned last_indent_, indent_;
    stream_adaptor &adaptor_;
    std::ostream *out;
    std::string filename_;

    void lex();
  public:
    highlight(stream_adaptor &s, const std::string &fn)
      : last_kw(NONE), last_indent_(0), indent_(0), adaptor_(s), out(0),
      first_comment(true), filename_(fn)
    {
      out = adaptor_.create(NONE);
      lex();
    }
    bool first_comment;
    void put_curly(const std::string &s);
    void put_number(const std::string &s);
    void put_keyword(const std::string &s);
    void put_text(const std::string &s);
    void put_comment_begin(const std::string &s);
    void put_comment_end(const std::string &s);
    void put_op(const std::string &s);
    void put_squote_begin(const std::string &s);
    void put_squote_end(const std::string &s);
    void put_cquote_begin(const std::string &s);
    void put_cquote_end(const std::string &s);
    void put_ws(const std::string &s);

};


#endif
