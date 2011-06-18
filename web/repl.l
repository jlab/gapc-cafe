
%{

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <boost/algorithm/string.hpp>

struct replace {
  std::map<std::string, std::string> files;
};

%}

/* specify 8bit input */
%option 8bit
/* no-fn no-fn non-interactive */
%option noyywrap nounput batch
/* warnings no-default-rule */
%option warn nodefault

%option reentrant
/* default global var/fn prefix is yy */
%option prefix="repl_"

%option extra-type="replace *"



%%


\$\{[a-zA-Z_]+\} {
  std::string s(yytext);
  s = s.substr(2, s.size()-3);
  std::map<std::string, std::string>::iterator i = yyextra->files.find(s);
  if (i != yyextra->files.end()) {
    yyin = fopen(i->second.c_str(), "r");
    if (yyin) {
      if (yyg->yy_buffer_stack_top > 10) {
        std::cerr << "Strange loop?\n";
        return 1;
      }
        yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE,
yyscanner), yyscanner);
    } else {
      std::cerr << "Cannot open for " << s << ": "
         << i->second.c_str() << " " << strerror(errno);
    }
  } else {
    std::cout << "${" << s << "}";
    //std::cerr << "Can't find variable: " << s << '\n';
  }
}

. ECHO;

\n ECHO;

<<EOF>> {
  fclose(yyin);
  yypop_buffer_state(yyscanner);
  if (!YY_CURRENT_BUFFER)
    yyterminate();
}

%%

int main(int argc, char **argv)
{
  replace *r = new replace();
  int o = 0;
  while ((o = getopt(argc, argv, ":t:")) != -1)
    switch (o) {
      case 't' :
        {
          std::string s(optarg);
          std::vector<std::string> v;
          boost::split(v, s, boost::is_any_of("="));
          if (v.size() != 2) {
            std::cerr << "Option error: " << s << '\n';
            return 2;
          }
          r->files[v.front()] = v.back();
          std::cerr << "ADD " << v.front() << ' ' << v.back() << '\n';
        }
        break;
      case '?' :
      case ':' :
        std::cerr << "Missing argument of " << char(optopt) << '\n';
        return 2;
      default:
        std::cerr << "Unknown Option: " << o << ' ' << char(o) << '\n';
        return 2;
    }
  std::string filename;
  if (argc && optind == argc-1)
    filename = argv[optind];
  else {
    std::cerr << "No filename specified!\n";
    return 3;
  }
  FILE *f = fopen(filename.c_str(), "r");
  if (!f) {
    std::cerr << "Can't open: " << filename << '\n';
    return 1;
  }
  yyscan_t scanner;
  repl_lex_init(&scanner);
  repl_set_in(f, scanner);
  repl_set_extra(r, scanner);
  int x = repl_lex(scanner);
  if (x)
    return 3;
  repl_lex_destroy(scanner);
  delete r;
  //fclose(f); already in EOF
  return 0;
}

