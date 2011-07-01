
/*

EXAMPLE darling
EXAMPLE airline

HEADER
<h1>Pairwise Sequence Alignment with Affine Gap-Costs</h1>

<p>
An <a target="_blank" href="http://en.wikipedia.org/wiki/Sequence_alignment">alignment</a> is a sequence of edit-operations which transforms one seqeuence
of characters into another one. The alignment problem is to compute the
alignment which is optimal under a given scoring scheme.
</p>

<p>
This example combines the local alignment variant (<a target="_blank"
href="http://en.wikipedia.org/wiki/Smith-Waterman_algorithm">Smith-Waterman
algorithm</a>) with affine gap-costs (<a target="_blank"
href="http://de.wikipedia.org/wiki/Gotoh-Algorithmus">Gotoh algorithm
(german)</a>), i.e. it models as edit-operations match, deletion-start,
insertion-start, deletion-other, insertion-other and the skipping of characters
for scoring.

</p>
HEADER

*/


input < raw, raw >

type spair = ( string first, string second )

signature Align(alphabet, answer) {
  answer match( < alphabet, alphabet >, answer);
  answer del( < alphabet, void >, answer);
  answer ins( < void, alphabet >, answer);
  answer delx( < alphabet, void >, answer);
  answer insx( < void, alphabet >, answer);

  answer nil( <int, int> );
  answer sl( <char, void>, answer);
  answer sr( <void, char>, answer);
  choice [answer] h([answer]);
}

algebra count auto count ;

algebra pretty implements Align(alphabet = char, answer = spair ) {
  spair match(< char a, char b>, spair m)
  {
    spair r;
    append(r.first, a);
    append(r.first, m.first);
    append(r.second, b);
    append(r.second, m.second);
    return r;
  }

  spair del(<char a, void>,  spair m)
  {
    spair r;
    append(r.first, a);
    append(r.first, m.first);
    append(r.second, '=');
    append(r.second, m.second);
    return r;
  }

  spair ins(<void , char b>, spair m)
  {
    spair r;
    append(r.first, '=');
    append(r.first, m.first);
    append(r.second, b);
    append(r.second, m.second);
    return r;
  }

  spair delx(<char a, void>, spair m)
  {
    spair r;
    append(r.first, a);
    append(r.first, m.first);
    append(r.second, '-');
    append(r.second, m.second);
    return r;
  }

  spair insx(<void, char b>, spair m)
  {
    spair r;
    append(r.first, '-');
    append(r.first, m.first);
    append(r.second, b);
    append(r.second, m.second);
    return r;
  }


  spair nil(<int l, int m>)
  {
    spair r;
    return r;
  }

  spair sl(<char a, void>, spair m)
  {
    return m;
  }

  spair sr(<void, char b>, spair m)
  {
    return m;
  }

  choice [spair] h([spair] l)
  {
    return l;
  }

}

algebra affine implements Align(alphabet = char, answer = int ) {
  int match(<char a, char b>, int m)
  {
    if (a == b)
      return m + 4;
    else
      return m - 3;
  }

  int del(<char a, void>, int m)
  {
    return m - 15 - 1;
  }

  int ins(<void, char b>, int m)
  {
    return m - 15 - 1;
  }

  int delx(<char a, void>, int m)
  {
    return m - 1;
  }

  int insx(<void, char b>, int m)
  {
    return m - 1;
  }


  int nil(<int l, int m>)
  {
    return 0;
  }

  int sl(<char a, void>, int m)
  {
    return m;
  }

  int sr(<void, char b>, int m)
  {
    return m;
  }

  choice [int] h([int] l)
  {
    return list(maximum(l));
  }

}
grammar affinelocsim uses Align(axiom = skipR)
{

  //tabulated { alignment, xDel, xIns }

  skipR = sr( < EMPTY, CHAR >, skipR) |
          skipL # h ;

  skipL = sl( < CHAR, EMPTY >, skipL) |
          alignment # h ;

  alignment = nil( < SEQ, SEQ> )   |
              del( < CHAR, EMPTY >, xDel) |
              ins( < EMPTY, CHAR > , xIns ) |
              match( < CHAR, CHAR >, alignment) # h ;

  xDel = alignment |
         delx( < CHAR, EMPTY >, xDel) # h ;

  xIns = alignment |
         insx( < EMPTY, CHAR >, xIns) # h ;

}

instance affine = affinelocsim ( affine ) ;

instance pretty = affinelocsim(pretty);

instance count = affinelocsim ( count ) ; 

instance affinecnt = affinelocsim ( affine * count ) ;

instance affinepp = affinelocsim ( affine * pretty ) ;

