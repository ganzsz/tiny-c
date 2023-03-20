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
    // ask();
    // openout();
    openin();
    // header();
    parse();
    // dumplits()
    // dumpglbs();
    // errorsummary();
    // trailer();
    // closeout();
    return;
  }

stripchar(str,ch) char* str; char ch;
  { int ptr = -1; // TODO: Not sure if this will continue to work
    while (str[++ptr] != ch) {;}
    str[ptr] = 0;
  }

parse()
  { while(eof==0)
      { if(0) ; /* TODO: we'll include things here later*/
        else newfunc();
      }
  }

  // dumplits()
  // dumpglbs()
  // errorsummary()
  // ask()
  // openout()

openin()
  {
    input=0;
    while(input==0)
      {
        kill();
        if(eof)break;
        pl("Input Filename? ");
        fgets(line, linesize, stdin);
        stripchar(line, '\n');
        if(ch()==0) {eof=1;break;}
        if((input=fopen(line,"r")) == null)
          { input=0;
            pl("Open failure");
          }
      }
    kill();
  }

  // doinclude()
  // closeout()
  // declglb(typ)
  // declloc(typ)
  // needsub()

newfunc()
  { 
    char n[namesize], *ptr;
    if (symname(n)==0)
      {
        error("illegal function or declaration");
        kill(); /* invalidate line */
        return;
      }
    if(ptr=findglb(n)) /* Already in global table ?*/
      {
        if(ptr[ident]!=function) multidef(n);
          /* already var by that name */
        else if(ptr[offset]==function) multidef(n);
          /* already function by that name */
        else ptr[offset] = function;
          /* otherwise we have what was earlier */
          /*  assumed to be a function */
      }
    /* if not in table, define as function now */
    else addglb(n, function, cint, function);
    /* we better see open paren for args... */
    if(match("(")==0)error("Missing open paren");
    outstr(n);col();nl();   /* print function name */
    argstk=0;               /* init arg count */
    while(match(")")==0)    /* then count args */
      { /* any legal name bumps arg count */
        if(symname(n))argstk=argstk+intsize;
        else{error("illegal arument name");junk();}
        blanks();
        /* if not clsing paren, should be comma */
        if(streq(line+lptr,")")==0)
          {
            if(match(",")==0) error("expected comma");
          }
          if(endst())break;
      }
    locptr=startloc;        /* "clear" local symbol table */
    sp=0;                   /* preset stack ptr */
    while(argstk) 
      { /* now let user declare what types of things */
        /*   those arguments were */
        if(amatch("char",4)){getarg(cchar);ns();}
        else if(amatch("int", 3)){getarg(cint);ns();}
        else {error("wrong number args");break;}
      }
    if(statement()!=streturn) /* do a statement, but if */
                              /*  it's a return, skip */
                              /* cleaning up the stack */
      {
        modstk(0);
        ret();
      }
    sp=0;                     /* reset stack ptr again */
    locptr=startloc;          /* deallocate all locals */
  }

  // getarg(t)
  // statement()
  // ns()
  // compound()
  // doif()
  // dowhile()
  // doreturn()
  // dobreak()
  // docont()
  // doasm()
  // callfunction(ptr)
  // junk()
  // endstr()
  // illname()
  // multidef(sname)
  // needbrack(str)
  // needlval()
  // findglb(sname)
  // findloc(sname)
  // addglb(sname, id, typ, value)
  // addloc(sname,id,typ,value)

symname(sname)
  char *sname;
  {int k;char c;
    blanks();
    if(alpha(ch())==0)return 0;
    k=0;
    while(an(ch()))sname[k++]=gch();
    sname[k]=0;
    return 1;
  }

  // getlabel()
  // printlabel(label)
  // alpha(c)
  // numeric(c)
  // an(c)

pl(str) char* str;
  {
    printf("\n%s", str); /* TODO: fix to recompilable implementation */
  }

  // addwhile(ptr)
  // delwhile()
  // readwhile()
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
kill()
  {
    lptr=0;
    line[lptr]=0;
  }

bytein()
  {
    while(ch()==0)
      {
        if(eof) return 0;
        linein();
        preprocess();
      }
    return gch();
  }

charin()
  {
    if(ch()==0)linein();
    if(eof)return 0;
    return gch();
  }
linein()
  {
    int k,unit;
    while(1)
      {
        if(input==0)openin();
        if(eof)return;
        if((unit=input2)==0)unit=input;
        kill();
        while((k=getc(unit))>0)
          {
            if((k==eol)|(lptr>=linemax))break;
            line[lptr++]=k;
          }
        line[lptr]=0;   /* terminate string */
        if(k<=0)
          {
            fclose(unit);
            if(input2)input2=0;
            else input=0;
          }
        if(lptr)
          {
            if((ctext)&(cmode))
              {
                // comment();
                // outstr(line);
                // nl();
              }
              lptr=0;
              return;
          }
      }
  }

keepch(c)
  char c;
  {
    mline[mptr]=c;
    if(mptr<mpmax)mptr++;
    return c;
  }

preprocess()
  {
    int k;
    char c,sname[namesize];
    if(cmode==0)return;
    mptr=lptr=0;
    while(ch())
      {
        if(ch()==' ')
          {
            keepch(ch());
            gch();
            while(ch()==" "|ch()==9)gch();
          }
        else if(ch()=='"')
          {
            while(ch()!='"')
              {
                if(ch()==0)
                  {
                    error("mising quote");
                    break;
                  }
                keepch(gch());
              }
            gch();
            keepch('"');
          }
        else if(ch()==39) /* ' */
          {
            keepch(39);
            gch();
            while(ch()!=39)
              {
                if(ch()==0)
                {
                  error("Missing single quote");
                  break;
                }
                keepch(gch());
              }
          }
        else if((ch()=='/')&(nch()=='*'))
          {
            charin();charin();
            while(((ch()=='*')&(nch()=='/'))==0)
              {
                if(ch()==0)linein();
                  else charin();
                if(eof)break;
              }
            charin();charin();
          }
        else if(an(ch()))
          {
            k=0;
            while(an(ch()))
              {
                if(k<namemax)sname[k++]=ch();
                gch();
              }
            sname[k]=0;
            if(k=findmac(sname))
              while(c=macq[k++])
                keepch(c);
            else
              {
                k=0;
                while(c=sname[k++])
                  keepch(c);
              }
          }
      else keepch(gch());
      }
    keepch(0);
    if(mptr>=mpmax)error("line too long");
    lptr=mptr=0;
    while(line[lptr++]=mline[mptr++]);
    lptr=0;
  }
  // addmac()
  // putmac(c)
  // findmac(sname)
  // outbyte(c)
  // outstr(ptr)
  // nl()
  // tab()
  // error(ptr)
  // ol(ptr)
  // ot(ptr)
  // streq(str1,str2)
  // astreq(str1,str2,len)
  // match(lit)
  // amatch(lit, len)
blanks()
  {
    while(1)
      {
        while(ch()==0)
          {
            linein();
            preprocess();
            if(eof)break;
          }
        if(ch()==' ')gch();
        else if(ch()==9)gch(); /* tab */
        else return;
      }
  }
  // outdec()
  // expression()
  // heir1(lval)
  // heir2(lval)
  // heir3(lval)
  // heir4(lval)
  // heir5(lval)
  // heir6(lval)
  // heir7(lval)
  // heir8(lval)
  // heir9(lval)
  // heir10(lval)
  // heir11(lval)
  // primary(lval)
  // store(lval)
  // rvalue(lval)
  // test(label)
  // constant(val)
  // number(val)
  // pstr(val)
  // qstr(val)
  // comment(){outbyte(';');}
  // header()
  // trailer()
  // getmem(sym)
  // getloc(sym)
  // putmem(sym)
  // putstk(typeobj)
  // indirect(typeobj)
  // swap()
  // immed()
  // push()
  // pop()
  // swapstk()
  // call(sname)
  // ret()
  // callstk()
  // jump(label)
  // testjumt(label)
  // defbyte()
  // defstorage()
  // defword()
  // modstk(newsp)
  // doublereg()
  // add()
  // sub()
  // mult()
  // div()
  // mod()
  // or()
  // xor()
  // and()
  // asr()
  // asl()
  // neg()
  // com()
  // inc()
  // dec()
  // eq()
  // ne()
  // lt()
  // le()
  // gt()
  // ge()
  // ult()
  // ule()
  // ugt()
  // uge()