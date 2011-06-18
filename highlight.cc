#include "highlight.hh"

void highlight::put_curly(const std::string &s)
{
  *out << "<span style=\"color: #FF0000\">" << s << "</span>";
  if (s == "{")
    ++indent_;
  if (s == "}") {
    --indent_;
    if (last_indent_ == indent_) {
      out = adaptor_.create(NONE);
      last_indent_ = 0;
    }
  }
}

void highlight::put_number(const std::string &s)
{
  *out << "<span style=\"color: #993399\">" << s << "</span>";
}

void highlight::put_keyword(const std::string &s)
{
  if (s == "pretty" && last_kw == ALGEBRA) {
    put_text(s);
    return;
  }
  if (s == "algebra") {
    last_kw = ALGEBRA;
    last_indent_ = indent_;
    out = adaptor_.create(ALGEBRA);
  }
  if (s == "grammar") {
    last_kw = GRAMMAR;
    last_indent_ = indent_;
    out = adaptor_.create(GRAMMAR);
  }
  if (s == "signature") {
    last_kw = SIGNATURE;
    last_indent_ = indent_;
    out = adaptor_.create(SIGNATURE);
  }
  *out << "<span style=\"font-weight: bold; color: #0000FF\">"
    << s
    << "</span>";
}

void highlight::put_text(const std::string &s)
{
  static const char types[][7] = { "int", "char", "string", "Rope", "double", "single" };
  for (unsigned i = 0; i<6; ++i)
    if (types[i] == s) {
      *out << "<span style=\"color: #009900\">" << s << "</span>";
       return;
    }
  if (last_kw)
    adaptor_.set_name(s);
  *out << s;
  last_kw = NONE;
}

void highlight::put_comment_begin(const std::string &s)
{
  *out << "<span style=\"font-style: italic; color: #9A1900\">"
    << s;
}

void highlight::put_comment_end(const std::string &s)
{
  *out << s << "</span>";
}

void highlight::put_op(const std::string &s)
{
  *out << "<span style=\"color: #990000\">"; 
  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
    switch (*i) {
      case '<' :
        *out << "&lt;";
        break;
      case '>' :
        *out << "&gt;";
        break;
      default:
        *out << *i;
        break;
    }
  *out << "</span>";
}

void highlight::put_squote_begin(const std::string &s)
{
  *out << "<span style=\"color: #FF0000\">"
    << s;
}

void highlight::put_squote_end(const std::string &s)
{
  *out << s << "</span>";
}

void highlight::put_cquote_begin(const std::string &s)
{
  *out << "<span style=\"color: #FF0000\">"
    << s;
}

void highlight::put_cquote_end(const std::string &s)
{
  *out << s << "</span>";
}

void highlight::put_ws(const std::string &s)
{
  *out << s;
}

typedef void* yyscan_t;
int hl_lex_init (yyscan_t* scanner);
int hl_lex_destroy (yyscan_t yyscanner );
int hl_lex (yyscan_t yyscanner);
void hl_set_extra (highlight *user_defined ,yyscan_t yyscanner );
void hl_set_in  (FILE * in_str ,yyscan_t yyscanner );

#include <stdio.h>
#include <sstream>

void highlight::lex()
{
  FILE *f = fopen(filename_.c_str(), "r");
  if (!f)
    return;
  yyscan_t scanner;
  hl_lex_init(&scanner);
  hl_set_in(f, scanner);
  hl_set_extra(this, scanner);
  hl_lex(scanner);
  hl_lex_destroy(scanner);
  fclose(f);
}



