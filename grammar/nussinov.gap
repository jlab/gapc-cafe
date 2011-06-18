/*

EXAMPLE acguacgu

HEADER
<h1>Nussinov Algorithm</h1>

<p> The <a target="_blank"
href="http://de.wikipedia.org/wiki/Nussinov-Algorithmus">Nussinov algorithm
(Nussinov 1978)</a> is a historic dynamic programming algorithm that takes one
character string as input and computes the maximal number of character
pairings. Not every character pair can form a pairing, only certain
complementary ones can do this. Each character can only be part of one pairing
and two pairings are not allowed to cross each other.  </p>

<p> Since the Nussinov algorithm is a historical landmark  in the field of RNA
secondary structure prediction an RNA alphabet is used in this example, i.e. A
= {a, u, c, g}. In RNA sequence analysis a character from A is called base and
a string of characters is called sequence. An RNA secondary structure is
defined by the set of base pairings.</p>

<p>
This GAP-L program allows basepairings between complementary
characters, i.e. (a,t), (c,g), (u,a), (g,u). Choosing different
algebras, the resulting program does different computations,
e.g. it computes the maximal number of
basepairs (bpmax), enumerates the search space (pretty), counts
the search space (count) or prints the optimal structures
(bpmax and pretty).
</p>

HEADER

*/

signature Nuss(alphabet, answer) {

  answer nil(void);
  answer right(answer, alphabet);
  answer pair(alphabet, answer, alphabet);
  answer split(answer, answer);
  choice [answer] h([answer]);

}

algebra pretty implements Nuss(alphabet = char, answer = string)
{
  string nil(void)
  {
    string r;
    return r;
  }

  string right(string a, char c)
  {
    string r;
    append(r, a);
    append(r, '.');
    return r;
  }

  string pair(char c, string m, char d)
  {
    string r;
    append(r, '(');
    append(r, m);
    append(r, ')');
    return r;
  }

  string split(string l, string r)
  {
    string res;
    append(res, l);
    append(res, r);
    return res;
  }

  choice [string] h([string] l)
  {
    return l;
  }
  
}

algebra bpmax implements Nuss(alphabet = char, answer = int)
{
  int nil(void)
  {
    return 0;
  }

  int right(int a, char c)
  {
    return a;
  }

  int pair(char c, int m, char d)
  {
    return m + 1;
  }

  int split(int l, int r)
  {
    return l + r;
  }

  choice [int] h([int] l)
  {
    return list(maximum(l));
  }
  
}

algebra bpmax2 extends bpmax
{
  kscoring choice [int] h([int] l)
  {
    int x = maximum(l);
    [int] r;
    push_back(r, x);
    if (x > 0)
      push_back(r, x-1);
    return r;
  }
}

algebra count implements Nuss(alphabet = char, answer = int)
{
  int nil(void)
  {
    return 1;
  }

  int right(int a, char c)
  {
    return a;
  }

  int pair(char c, int m, char d)
  {
    return m;
  }

  int split(int l, int r)
  {
    return l * r;
  }

  choice [int] h([int] l)
  {
    return list(sum(l));
  }
  
}


grammar nussinov uses Nuss (axiom=start) {

  tabulated { start, bp }

  start = nil(EMPTY) |
          right(start, CHAR) |
          split(start, bp) # h ;

  bp = pair(CHAR, start, CHAR) with char_basepairing ;


}

instance pretty = nussinov ( pretty ) ;

instance bpmax = nussinov ( bpmax ) ;

instance bpmaxpp = nussinov ( bpmax * pretty ) ;

instance count = nussinov ( count ) ;

instance bpmaxcnt = nussinov ( bpmax * count ) ;

instance bpmax1pp = nussinov ( bpmax . pretty ) ;

instance kbpmaxpp = nussinov ( bpmax2 * pretty ) ;


