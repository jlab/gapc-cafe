
%{

#include "highlight.hh"

%}

/* specify 8bit input */
%option 8bit
/* no-fn no-fn non-interactive */
%option noyywrap nounput batch
/* warnings no-default-rule */
%option warn nodefault

%option reentrant
/* default global var/fn prefix is yy */
%option prefix="hl_"

%option extra-type="highlight *"


%x SKIP_LINE
%x SKIP_COMMENT
%x STRING_QUOTE
%x CHAR_QUOTE

%%

 /* ------------------------------------------------------------------------ */
 /* keywords */
 /* ------------------------------------------------------------------------ */

import|input|type|extern|choice|implements|extends|parameters|alphabet|uses|axiom|instance|tabulated|auto|with_overlay|with|suchthat_overlay|suchthat|void {
  yyextra->put_keyword(yytext);
}

signature {
  yyextra->put_keyword(yytext);
}

algebra {
  yyextra->put_keyword(yytext);
}

grammar {
  yyextra->put_keyword(yytext);
}


 /* ------------------------------------------------------------------------ */
 /* algebra types */
 /* ------------------------------------------------------------------------ */

synoptic|pretty|classify|scoring|kscoring {
  yyextra->put_keyword(yytext);
}

 /* ------------------------------------------------------------------------ */
 /* statements */
 /* ------------------------------------------------------------------------ */

if|else|for|return|continue { yyextra->put_keyword(yytext); }

 /* ------------------------------------------------------------------------ */
 /* comments */
 /* ------------------------------------------------------------------------ */

"//" { yyextra->put_comment_begin(yytext); BEGIN SKIP_LINE; }

<SKIP_LINE>{
\n { yyextra->put_comment_end(yytext); BEGIN INITIAL; }
[^\n]+ { yyextra->put_text(yytext); }
}

"/*" { if (!yyextra->first_comment) yyextra->put_comment_begin(yytext); BEGIN SKIP_COMMENT; }

<SKIP_COMMENT>{
"*/" {
if (!yyextra->first_comment)
yyextra->put_comment_end(yytext); yyextra->first_comment = false; BEGIN INITIAL; }
[^*\n]+ { if (!yyextra->first_comment) yyextra->put_text(yytext); }
"*" { if (!yyextra->first_comment) yyextra->put_text(yytext); }
\n { if (!yyextra->first_comment) yyextra->put_text(yytext); }
}

 /* ------------------------------------------------------------------------ */
 /* operators */
 /* ------------------------------------------------------------------------ */
==|!=|"<="|>=|&&|"||"|!|"++"|"+="|--|-= { yyextra->put_op(yytext); }


 /* ------------------------------------------------------------------------ */
 /* chars, strings */
 /* ------------------------------------------------------------------------ */

\" { yyextra->put_squote_begin(yytext); BEGIN STRING_QUOTE; }

<STRING_QUOTE>{
[^\"]+ {  yyextra->put_text(yytext); }
\" { yyextra->put_squote_end(yytext); BEGIN INITIAL; }
}

' { yyextra->put_cquote_begin(yytext); BEGIN CHAR_QUOTE; }

<CHAR_QUOTE>{
[^']+ { yyextra->put_text(yytext); }
' { yyextra->put_cquote_end(yytext); BEGIN INITIAL; }
}

 /* ------------------------------------------------------------------------ */
 /* special characters */
 /* ------------------------------------------------------------------------ */

[%$.<>'\"#:|=(),;\[\]*/+-] { yyextra->put_op(yytext); }

[{}] { yyextra->put_curly(yytext); }

\n+ { yyextra->put_text(yytext); }

[ ]+ { yyextra->put_ws(yytext); }

[\t]+ { yyextra->put_ws(yytext); }

 /* ------------------------------------------------------------------------ */
 /* sval aka string , number ... */
 /* ------------------------------------------------------------------------ */
[0-9]+ { yyextra->put_number(yytext); }

-?[0-9]+\.[0-9]+(e[+-]?[0-9]+)? { yyextra->put_number(yytext); }

[A-Za-z0-9_]+ { yyextra->put_text(yytext); }

 /* ignore carriage return - stupid windows users ... */
\15 ;

. { } /* std::cerr << "UNEXPECTED CHARACTER\n";  } */

%%


