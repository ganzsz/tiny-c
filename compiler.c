#include <stdio.h>

#include "constants.c"

main()
  { glbptr=startglb;
    locptr=startloc;
    wqptr=wq;

    /* zero these */
    macptr=
    litptr=
    sp =
    errcnt=
    eof=
    input=
    input2=
    output=
    ncmp=
    lastst=
    quote[1]=
    0;

    quote[0] = '"';
    cmode = 1;

    /* The real start */
    openin();
    parse();
  }

openin()
  {
    input=0;
    while (input==0)
      {
        clearInputLine();
        if(eof)break;
        pl("Input Filename? ");
        fgets(line, linesize, stdin);
        stripChar(line, '\n');
        if(ch()==0) {eof=1;break;}
        if((input=fopen(line,"r")) == null)
          { input=0;
            pl("Open failure");
          }
      }
    clearInputLine();
  }

stripChar(str, ch) char* str; char ch;
  { int ptr = -1; // TODO: Not sure if this will continue to work
    while (str[++ptr] != ch) {;}
    str[ptr] = 0;
  }

parse()
  { while (eof==0)
      { if(0) ; /* TODO: we'll include things here later*/
        else newfunc();
      }
  }

newfunc() {eof=1;}
//   { char n[namesize], *ptr;
//     if (symname(n)==0) return; /* TODO: Error handling*/
//     if(ptr=findglb(n)) /* Already in global table ?*/
//   }

symname()
  { 
    ;
  }

pl(str) char* str;
  {
    printf("\n%s", str); /* TODO: fix to recompilable implementation */
  }

ch()
  { return(line[lptr]&127);}
nch()
  { if(ch() == 0) return 0;
    else return(line[lptr+1]&127);
  }
gch()
  { if(ch() == 0) return 0;
    else return(line[lptr++]&127);
  }

clearInputLine()
  {
    lptr=0;
    line[lptr]=0;
  }