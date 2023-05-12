#include <stdio.h>
#include <stdlib.h>

/**
 * form:
 * <program> ::= <block> END
 * <block> ::= [ <statement> ]*
 *
 * <expression> ::= <term> [<addop> <term>]*
 * <term> ::= <signed factor> [<mulop> factor]*
 * <signed factor>::= [<addop>] <factor>
 * <factor> ::= <integer> | <variable> | (<b-expression>)
 * 
 * <b-expression> ::= <b-term> [<orop> <b-term>]*
 * <b-term> ::= <not-factor> [AND <not-factor>]*
 * <not-factor> ::= [NOT] <b-factor>
 * <b-factor> ::= <b-literal> | <b-variable> | <relation>
 * <relation> ::= | <expression> [<relop> <expression]
*/

/**
 * Type Declarations
*/

typedef char Symbol[9];

/**
 * Constant Definitions
*/

#define MAX_AMOUNT_GLOBALS 26
#define MAX_AMOUNT_SUBROUT 26
#define MAX_LABEL_LEN 10

Symbol KWList[] = {"\0", "IF", "ELSE", "ENDIF", "END"};
char   KWCode[] = {'x',  'i' , 'l'   , 'e'    , 'e'  };

/**
 * Variable Declarations
*/

int inputLine = 1;

char Look;   // Lookahead Character
char Token;  // Encoded Token
char Value[17]; // Unencoded Token
int  LabelCount; // Label Counter
char Globals[1000] = {'\0'};
// Symbol Subroutines[MAX_AMOUNT_SUBROUT] = {};

// --------------------------------------------------------------
// Helper to convert a char to a char* ending in \0
char *charToStr(char c)
{
  char *ptr = malloc(2 * sizeof(char));
  ptr[0] = c;
  ptr[1] = '\0';
  return ptr;
}

// dangerous strcopy
void strcopy(char* from, char* to, int maxLen)
{
  for(int i = 0; i < maxLen; i++)
  {
    to[i] = from[i];
    if(from[i]=='\0') return;
  }
}

// dangerous strcomp
char strcomp(char* s1, char* s2)
{
  for(int i = 0; s1[i]!='\0' && s2[i]!='\0'; i++)
    if(s1[i]!=s2[i]) return 0;
  return 1;
}

// --------------------------------------------------------------
// Helper to see if char is in char array

char inArray(char c, char* a, int len)
{
  for(int i = 0; i < len; i++)
    if(a[i] == c) return 1;
  return 0;
}

// --------------------------------------------------------------
// Forward declaration
void Abort(char* s);

// Checking if a global exists and defining one if it doesn't
void declareGlobal(char* name)
{
  int i = 0, n = 0;
  char found = 1;
  for(;;)
  {
    if(Globals[i] == '\0')
    {
      if(n>0 && found) break;
      if(Globals[i+1] == '\0')
      {
        if(i>0)i++;
        found = 0;
        break;
      }
      found = 1;
      n = 0;
    } else {
      if(Globals[i] != name[n]) found = 0;
      n++;
    }
    i++; 
  }
  if(!found)
  {
    for(n=0; name[n] != '\0'; n++)
    {
      Globals[i] = name[n];
      i++;
    }
    Globals[i] = '\0';
  }
}

// --------------------------------------------------------------
// Checking if a subroutine exists and defining one if it doesn't
// void declareSubroutine(char name)
// {
//   int i = 0;
//   while((i-1)<MAX_AMOUNT_SUBROUT && Subroutines[i] != '\0') if(Subroutines[i++] == name) {return;}
//   if((i-1) == sizeof(Subroutines)) Abort("Max number of constants");
//   Subroutines[i] = name;
// }



// --------------------------------------------------------------
// Read New Character From Input Stream
void GetChar()
{
  Look = getc(stdin);
  if(Look == '\n') inputLine++;
}

// Report an Error
void Error(char *s)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Error: %s. On line %i\n", s, inputLine);
}

// Report Error and Halt
void Abort(char *s)
{
  Error(s);
  exit(1);
}

// Report What Was Expected
void Expected(char *s)
{
  // char *ender = " Expected";
  // char line[sizeof(s) - 1 + sizeof(ender)];
  // int i = -1;
  // while (s[++i] != '\0')
  //   line[i] = s[i];
  // for (int j = 0; j < sizeof(ender); j++)
  // {
  //   line[i++] = ender[j];
  // }
  char error[50];
  sprintf(error, "Expected: %s", s);
  Abort(error);
}

// --------------------------------------------------------------
// Recognize an Alpha Character
int IsAlpha(char c)
{
  return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// Recognize a Decimal Digit
char IsDigit(char c)
{
  return c >= '0' && c <= '9';
}

// Recognize an AlphaNumeric Character
char IsAlNum(char c)
{
  return IsAlpha(c) || IsDigit(c);
}

// Recognize an Addop
char IsAddop(char c)
{
  return c == '+' || c == '-';
}

// Recognize an Mulop
char IsMulop(char c)
{
  return c == '*' || c == '/';
}

// Recognize White Space
char IsWhite(char c)
{
  return c == ' ' || c == '\t';
}

// --------------------------------------------------------------
// Skip over Leading Whitespace
void SkipWhite()
{
  while (IsWhite(Look)) GetChar();
}

// Skip over EOL
void Fin()
{
  if(Look == '\n') GetChar();
  SkipWhite();
}

// --------------------------------------------------------------
// Table Lookup
int Lookup()
{
  int n = 3;
  while (n>0)
  {
    if(strcomp(Value, KWList[n]))
    {
      return n;
    }
    else n--;
  }
  return n;
}

// --------------------------------------------------------------
// Match a Specific Input Character
void Match(char x)
{
  if (Look != x) Expected(charToStr(x));
  GetChar();
  SkipWhite();
}

// --------------------------------------------------------------
// Get an Identifier
void GetName()
{
  while (Look == '\n') Fin();
  if(!IsAlpha(Look)) Expected("Name");
  Value[0] = '\0';
  int i = 0;
  while(IsAlNum(Look))
  {
    Value[i] = Look;
    Value[++i] = '\0';
    GetChar();
  }
  SkipWhite();
}

// Get a Number
void GetNum()
{
  if(!IsDigit(Look)) Expected("Integer");
  Value[0] = '\0';
  int i = 0;
  while(IsDigit(Look))
  {
    Value[i] = Look;
    Value[++i] = '\0';
    GetChar();
  }
  Token = '#';
  SkipWhite();
}

void Scan()
{
  GetName();
  Token = KWCode[Lookup()];
}

void MatchString(char* x, int maxlen)
{
  for(int i = 0; i<maxlen; i++)
    if(Value[i]!=x[i]) Expected(x);
}

// --------------------------------------------------------------
char Label[MAX_LABEL_LEN];
void clearLabel(){for(int i=0; i<MAX_LABEL_LEN; i++)Label[i]='\0';}
// Generate a Unique label
void NewLabel()
{
  clearLabel();
  sprintf(Label, "L%d", LabelCount++);
}

// Set l to new generated label
void MakeNewLabel(char *l)
{
  NewLabel();
  strcopy(Label, l, MAX_LABEL_LEN);
}

// --------------------------------------------------------------
// Post a label to output
void PostLabel(char* l)
{
  printf("%s:\n", l);
}

// --------------------------------------------------------------
// Output a String with Tab
void Emit(char *s)
{
  printf("\t%s", s);
}

// Output a String with Tab and CRLF
void EmitLn(char *s)
{
  Emit(s);
  printf("\n");
}

//---------------------------------------------------------------}
// Parse and Translate an Identifier }
void Ident()
{
  GetName();
  if(Look == '(') // We're dealing with a function call
  {
    Match('(');
    Match(')');
    char line[50];
    sprintf(line, "CALL __%s\t; Call subroutine", Value);
    EmitLn(line);
    // declareSubroutine(Value);
  } else {      // A variable call
    //             01234567890
    char line[50];
    sprintf(line, "MOV R8, [_var_%s]\t ; Retrieve var", Value);
    EmitLn(line);
    declareGlobal(Value);
  }
}

// --------------------------------------------------------------
// forward declaration
void Expression();

// Parse and Translate a Math Factor
void Factor()
{
  if (Look == '(')
  {
    Match('(');
    Expression();
    Match(')');
  }
  else if (IsAlpha(Look))
  { 
    Ident();
  }
  else
  {
    GetNum();
    char Line[50];
    sprintf(Line, "MOV R8, %s\t ; Move num to R8", Value);
    EmitLn(Line);
  }
}

// Parse and Translate the First Math Factor
// Result stored in R8
void SignedFactor()
{
  char s = Look == '-';
  if (IsAddop(Look))
  {
    GetChar();
    SkipWhite();
  }
  Factor();
  if(s) EmitLn("NEG R8");
}

// --------------------------------------------------------------
// Recognize and Translate a Multiply
void Multiply()
{
  Match('*');
  Factor();
  EmitLn("; *");
  EmitLn("POP R9");
  EmitLn("IMUL R8,R9");
}

// Recognize and Translate a Divide
void Divide()
{
  Match('/');
  Factor();
  EmitLn("; /");
  EmitLn("POP RAX       ; Top half");
  EmitLn("MOV RDX, 0    ; Bot half always 0");
  EmitLn("IDIV R8       ; Divide above by R8");
  EmitLn("MOV R8, RAX   ; Store result in R8");
}

// --------------------------------------------------------------
// Completion of Term Processing (Used by Term and FirstTerm)
void Term1()
{
  while(IsMulop(Look))
  {
    EmitLn("PUSH R8");
    switch (Look)
    {
      case '*': Multiply(); break;
      case '/': Divide(); break;
    }
  }
}

// Parse and Translate a Math Term
void Term()
{
  Factor();
  Term1();
}

// Parse and Translate a Math Term with Possible Leading Sign
void FirstTerm()
{
  SignedFactor();
  Term1();
}

// --------------------------------------------------------------
// Recognize and Translate an Add
void Add()
{
  Match('+');
  Term();
  EmitLn("; +");
  EmitLn("POP R9");
  EmitLn("ADD R8, R9");
}

// Recognize and Translate a Subtract
void Subtract()
{
  Match('-');
  Term();
  EmitLn("; -");
  EmitLn("POP R9");
  EmitLn("SUB R8, R9");
  EmitLn("NEG R8");
}

// --------------------------------------------------------------
// Recognize and Translate an Expression
void Expression()
{
  FirstTerm();
  while(IsAddop(Look))
  {
    EmitLn("PUSH R8");
    switch(Look)
    {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
  }
}

// --------------------------------------------------------------
// Parse and Translate a boolean condition
// Dummy for now
void Condition()
{
  EmitLn("cmp R8, 0\t ; FORNOWTEMPOART");
}

// Forward decl
void Block();

// Translate If Construct
void If()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  MakeNewLabel(l1);
  strcopy(l1, l2, MAX_LABEL_LEN);
  EmitLn("; IF");
  Condition();
  printf("\tJE %s\t ; Jump Equal (false) to endif or else\n", l1);
  Block();
  if(Token == 'l')// else
  {
    EmitLn("; ELSE");
    MakeNewLabel(l2);
    printf("\tJMP %s\t ; after this is else, so get out of if block \n", l2);
    PostLabel(l1);
    Block();
  }
  MatchString("ENDIF", 5);
  PostLabel(l2);
}

// Translate Switch Construct
// void Switch()
// {
//   char endS[MAX_LABEL_LEN], endC[MAX_LABEL_LEN];
//   Match('s');
//   EmitLn("; Switch");
//   MakeNewLabel(endS);
//   Expression();
//   EmitLn("PUSH R8\t ; Push expression to test against");
//   Block();
//   while(Look == 'c')
//   {
//     Match('c');
//     EmitLn("; Case");
//     MakeNewLabel(endC);
//     Expression();
//     EmitLn(" <Compare R8, [RSP]");
//     printf("\tJNE %s\t ; Go to next statement if not equal\n", endC);
//     Block();
//     PostLabel(endC);
//   }
//   PostLabel(endS);
//   EmitLn("POP R8\t ; Clean up stack");
// }

// --------------------------------------------------------------
// Parse and Translate an Assignment Statement }

void Assignment()
{
  char name[9];
  strcopy(Value, name, 9);
  Match('=');
  Expression();
  char line[50];
  sprintf(line, "MOV [_var_%s], R8\t ; Save result in memory", name);
  EmitLn(line);
  declareGlobal(name);
}

int blockCounter = 0;
// Recognize and translate a Block
void Block()
{
  int thisBlock = blockCounter++;
  printf("\t; START BLOCK %i\n", thisBlock);
  Scan();
  while (Token != 'e' && // End
        //  Look != 'c' && // Case
         Token != 'l')// && // Else
        //  Look != 'u')   // Untill
  {
    switch (Token)
    {
      case 'i': If(); break;
      // case 's': Switch(); break;
      // case 'w': While(); break;
      // case 'p': Loop(); break;
      // case 'r': Repeat(); break;
      // case 'f': For(); break;
      // case 'd': Do(); break;
      // case 'b': Break();
      // case '\n': while(Look == '\n') Fin(); break;
      default: Assignment(); break;
    }
    Scan();
  }
  printf("\t; END BLOCK %i\n", thisBlock);
}

// Parse and Translate a program
void DoProgram()
{
  SkipWhite();
  Block();
  MatchString("END", 3);
}

// Initialize
void Init()
{
  GetChar();
}

void emitPrintFunctions()
{
  FILE *fptr;
  fptr = fopen("../shared/printfunctions.s", "r");
  if(fptr != NULL)
  {
    char c;
    c = fgetc(fptr);
    while(c != EOF)
    {
      printf("%c", c);
      c = fgetc(fptr);
    }
    fclose(fptr);
  }
}

// --------------------------------------------------------------
// EmitHeader

void EmitHeader()
{
  printf("section     .text\n");
  printf("global      _start\n\n");
  emitPrintFunctions();
  printf("_start: \n");
  printf("; ============== THE GENERATED PROGRAM ==============\n");
}

// --------------------------------------------------------------
// EmitFooter

void EmitFooter()
{
  printf("; ============== END GENERATED PROGRAM ==============\n\n");
  // Magic to print R8
  EmitLn("; Print register R8");
  EmitLn("MOV RAX, R8\t ; Print R8");
  EmitLn("CALL _printi\t; As int");
  printf("\n");

  EmitLn("; Print \\n");
  EmitLn("MOV RAX, 10\t ; Print \\n");
  EmitLn("CALL _printc\t; As char");
  
  printf("\n");
  // stop program
  EmitLn("; Stop gracefully");
  EmitLn("mov rax, 60       ; exit(");
  EmitLn("mov rdi, 0        ;   EXIT_SUCCESS");
  EmitLn("syscall           ; );");

  // -----------------------------------------------------------------
  // Defined functions:
  /*for(int i=0; i<sizeof(Subroutines); i++)
    if(Subroutines[i]=='\0') break;
    else printf("__%c:\n\tret\n", Subroutines[i]);
*/

  // Reserve memory
  printf("section     .data\n");
  printf("\tmsg    db  0x00\n"); // Reserved for outputting on eof
  int n = 0;
  char ident[20];
  for(int i=0; i<1000; i++)
  {
    if(Globals[i]=='\0')
    {
      if(n==0)break;
      ident[n] = '\0';
      printf("\t_var_%s dw 0x0000\n", ident);
      n = 0;
    } else {
      ident[n++] = Globals[i];
    }
  }
/*
  // Reserver memory for the constants
  for(int i=0; i<sizeof(Globals); i++)
    if(Globals[i]=='\0') break;
    else printf("\t%c      dw 0x0000\n", Globals[i]);*/
}

// Main Program
int main(){
  EmitHeader();
  Init();
  DoProgram();
  EmitFooter();
  return 0;
}