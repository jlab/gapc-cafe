/*

EXAMPLE 10,100,100,5,5,50,

HEADER
<h1>Matrix chain multiplication</h1>

<p>
<a
target="_blank" href="http://en.wikipedia.org/wiki/Matrix_chain_multiplication">Matrix
chain multiplication</a> (<a
target="_blank" href="http://de.wikipedia.org/wiki/Matrix-Kettenmultiplikation">Matrix-Kettenmultiplikation
- German</a>) is associative and depending on the multiplication
  order the number of elementary operations or highwater memory
usage varies. Using dynamic programming the optimal bracketing
under some objective is easy to compute.
</p>

<p> In this GAP-L program, the answer datatype <tt>tuple</tt> has three
components, where the first one stores the number of elementary
operations, the second one stores the number of rows of the
sub-expression and the last one stores the number of columns of the
sub-expression. </p>

HEADER

*/

type tuple = (int ops, int rows, int cols)

signature Sig(alphabet, answer) {
  answer single(int, char, int, char);
  answer mult(answer, answer);
  choice [answer] h([answer]);
}

algebra minmult implements Sig(alphabet = char, answer = tuple)
{
  tuple single(int r, char a, int c, char b)
  {
    tuple x;
    x.ops = 0;
    x.rows = r;
    x.cols = c;
    return x;
  }
  tuple mult(tuple a, tuple b)
  {
    tuple x;
    x.ops = a.ops + b.ops + a.rows*a.cols*b.cols;
    x.rows = a.rows;
    x.cols = b.cols;
    return x;
  }
  choice [tuple] h([tuple] l)
  {
    return list(minimum(l));
  }
}

algebra count auto count ;

algebra maxmult extends minmult {
  choice [tuple] h([tuple] l)
  {
    return list(maximum(l));
  }
}

grammar mopt uses Sig(axiom = matrix) {

  // just a test case for the ys fixpoint limit
  // the looping for single could be eliminated
  // using a different alphabet
  matrix = single(INT, CHAR(','), INT, CHAR(',')) |
           mult(matrix, matrix)                    # h ;

}

instance minmult = mopt(minmult) ;
instance maxmult = mopt(maxmult) ;
instance count = mopt(count) ;
