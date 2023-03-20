/* TODO: Do i want this*/
#define null    0
#define eol     13

/* define the symbol table parameters */
#define symsiz  14
#define symtbsz 5040
#define numglbs 300
#define startglb symtab
#define endglb  startglb+numglbs+symsiz
#define startloc endglb+symsiz
#define endloc  symtab+symtbsz-symsiz
#define intsize 2

/* define symbol table entry format */
#define name    0
#define ident   9
#define type    10
#define storage 11
#define offset  12

/* system wide name size (for symbols) */
#define namesize 9
#define namemax  8

/* define possible entries for "ident" */
#define variable 1
#define array   2
#define pointer 3
#define function 4

/* define possible entries for "type" */
#define cchar   1
#define cint    2

/* define the "while" statement queue */
#define wqtabsz 100
#define wqsiz   4
#define wqmax   wq+wqtabsz-wqsiz

/* define entry offsets in while queue */
#define wqsym   0
#define wqsp    1
#define wqloop  2
#define wqlab   3

/* define the literal pool */
#define litabsz 2000
#define litmax  litabsz-1

/* define the input line */
#define linesize 120
#define linemax litabsz-1
#define mpmax   linemax

/* define the macro (define) pool */
#define macqsize 1000
#define macmax  macqsize-1

/* define statement types (tokens) */
#define stif    1
#define stwhile 2
#define streturn 3
#define stbreak 4
#define stcont  5
#define stasm   6 /*will probabely be unused*/
#define stexp   7

/* reserve storage words*/
char    symtab[symtbsz];    /* symbol table */
char    *glbptr,*locptr;    /* ptrs to next entries */
int     wq[wqtabsz];        /* while queue */
int     *wqptr;             /* ptr to next entry */

char    litq[litabsz];      /* literal pool */
int     litptr;             /* ptr to next entry*/

char    macq[macqsize];     /* macro string buffer */
int     macptr;             /* and its index */

char    line[linesize];     /* parsing buffer */
char    mline[linesize];    /* temp macro buffer */
int     lptr,mptr;          /* ptrs into each */

/* misc storage */
int     nxtlab,             /* next available label # */
        litlab,             /* label # assigned to literal pool */
        sp,                 /* compiler relative stk ptr */
        argstk,             /* function arg sp */
        ncmp,               /* # open compound statements */
        errcnt,             /* # errors in compilation */
        eof,                /* nonzero on final input eof */
        input,              /* iob # for input file */
        output,             /* iob # for output file (if any)*/
        input2,             /* iob # for "include" file */
        glbflag,            /* non-zero if internal globals */
        ctext,              /* non-zero to intermix c-source */
        cmode,              /* non-zero while parsing c-code*/
                            /* zero when passing asm code */
        lastst;             /* last executed statement type */

char    quote[2];           /* literal string for '"' */
char    *cptr;              /* work ptr to any char buffer */
char    *iptr;              /* work ptr to any int buffer */