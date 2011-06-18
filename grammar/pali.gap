/*

EXAMPLE ababdcdaaadacda

HEADER
<h1>Palindromes</h1>

<p>
This algorithm computes approximative nested <a target="_blank"
href="http://en.wikipedia.org/wiki/Palindrome">palindromes</a> using a
simple scoring function (<i>exact</i> palindromes are strings
which read the same forward and backward).
</p>
<p>
This grammar resembles a simplified <a href="adpfold">RNA
secondary structure prediction grammar</a>.
</p>



HEADER

*/


signature paliS(alphabet, answer) {
  answer match(alphabet, answer, alphabet);
  answer turn(int);
  answer sl(char, answer);
  answer sr(answer, char);
  answer app(answer, answer);
  choice [answer] h([answer]);
}

algebra enum auto enum ;
algebra count auto count ;

algebra pretty implements paliS(alphabet = char, answer = string) {

  string match(char a, string x, char b)
  {
    string r;
    if (a == b) {
      append(r, '(');
      append(r, x);
      append(r, ')');
    } else {
      append(r, '{');
      append(r, x);
      append(r, '}');
    }
    return r;
  }

  string turn(int l)
  {
    string r;
    append(r, '+', l);
    return r;
  }

  string sl(char a, string x)
  {
    string r;
    append(r, '-');
    append(r, x);
    return r;
  }

  string sr(string x, char b)
  {
    string r;
    append(r, x);
    append(r, '-');
    return r;
  }

  string app(string x, string y)
  {
    string r;
    append(r, x);
    append(r, y);
    return r;
  }

  choice [string] h([string] x)
  {
    return x;
  }
  
}


algebra score implements paliS(alphabet = char, answer = int) {

  int match(char a, int b, char c)
  {
    if (a == c)
      return b + 3;
    else
      return b - 1;
  }

  int turn(int l)
  {
    return 0;
  }

  int sl(char c, int x)
  {
    return x;
  }

  int sr(int x, char c)
  {
    return x;
  }

  int app(int x, int y)
  {
    return x + y;
  }

  choice [int] h([int] x)
  {
    return list(maximum(x));
  }
  
}

algebra klass implements paliS(alphabet = char, answer = int) {

  int match(char a, int b, char c)
  {
    return b;
  }

  int turn(int l)
  {
    return 1;
  }

  int sl(char c, int x)
  {
    return x;
  }

  int sr(int x, char c)
  {
    return x;
  }

  int app(int x, int y)
  {
    return x + y;
  }

  choice [int] h([int] x)
  {
    return unique(x);
  }
  
}

grammar pali5 uses paliS(axiom = A)
{

  A = skipl | app(skipl, S) # h ;

  skipl = skipr |
          sl(CHAR, skipl) # h ;

  skipr = sr(skipr, CHAR) |
          S # h ;

  S = match(CHAR, S, CHAR) |
      match(CHAR, turn(SEQ), CHAR) |
      match(CHAR, sl(CHAR, A), CHAR) |
      match(CHAR, sr(A, CHAR), CHAR) # h ;

}


instance klasssp = pali5( klass * score * pretty ) ;
instance klassc = pali5( klass * count ) ;


instance scorepp = pali5 ( score * pretty ) ;

instance score = pali5 ( score ) ;

instance pretty = pali5 ( pretty ) ; 

instance enum = pali5 ( enum );

instance scoreen = pali5 ( score * enum ) ;


