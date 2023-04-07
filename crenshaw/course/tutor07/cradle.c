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
Symbol SymTab[1000];
Symbol* TabPtr = SymTab;

/**
 * Constant variables
*/

#define MAX_AMOUNT_GLOBALS 26
#define MAX_AMOUNT_SUBROUT 26

#define MAX_LABEL_LEN 10

/**
 * Variable Declarations
*/

char Look;   // Lookahead Character
int  LabelCount; // Label Counter
char Globals[MAX_AMOUNT_GLOBALS] = {'\0'};
char Subroutines[MAX_AMOUNT_SUBROUT] = {'\0'};

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
void declareGlobal(char name)
{
  int i = 0;
  while((i-1)<MAX_AMOUNT_GLOBALS && Globals[i] != '\0') if(Globals[i++] == name) {return;}
  if((i-1) == sizeof(Globals)) Abort("Max number of constants");
  Globals[i] = name;
}

// --------------------------------------------------------------
// Checking if a subroutine exists and defining one if it doesn't
void declareSubroutine(char name)
{
  int i = 0;
  while((i-1)<MAX_AMOUNT_SUBROUT && Subroutines[i] != '\0') if(Subroutines[i++] == name) {return;}
  if((i-1) == sizeof(Subroutines)) Abort("Max number of constants");
  Subroutines[i] = name;
}



// --------------------------------------------------------------
// Read New Character From Input Stream
void GetChar()
{
  Look = getc(stdin);
}

// Report an Error
void Error(char *s)
{
  printf("\n");
  printf("Error: %s.\n", s);
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
  char *ender = " Expected";
  char line[sizeof(s) - 1 + sizeof(ender)];
  int i = -1;
  while (s[++i] != '\0')
    line[i] = s[i];
  for (int j = 0; j < sizeof(ender); j++)
  {
    line[i++] = ender[j];
  }
  Abort(line);
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
  if(Look == '\r') getchar(); // MS safer
  if(Look == '\n') GetChar();
  SkipWhite();
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
char GetName()
{
  while (Look == '\n') Fin();
  if(!IsAlpha(Look)) Expected("Name");
  char n = Look;
  GetChar();
  SkipWhite();
  return n;
}

// Get a Number
char GetNum()
{
  if(!IsDigit(Look)) Expected("Integer");
  char n = Look;
  GetChar();
  SkipWhite();
  return n;
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
  char name = GetName();
  if(Look == '(') // We're dealing with a function call
  {
    Match('(');
    Match(')'); 
    //             01234567890123456789
    char line[] = "CALL __x\t; Call subroutine";
    line[7] = name;
    EmitLn(line);
    declareSubroutine(name);
  } else {      // A variable call
    //             01234567890
    char line[] = "MOV R8, [x]\t ; Retrieve var";
    line[9] = name;
    EmitLn(line);
    declareGlobal(name);
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
  { //             01234567899012345678901234567890
    char Line[] = "MOV R8, 0\t ; Move num to R8";
    char c = GetNum();
    Line[8] = c;
    EmitLn(Line);
  }
}

// Parse and Translate the First Math Factor
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
  EmitLn("<Condition>");
}

// Forward decl
void Block();

// Translate If Construct
void If()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match('i');
  Condition();
  MakeNewLabel(l1);
  strcopy(l1, l2, MAX_LABEL_LEN);
  EmitLn("; IF");
  printf("\tJE %s\t ; Jump Equal (false) to endif or else\n", l1);
  Block();
  if(Look == 'l')// else
  {
    Match('l');
    EmitLn("; ELSE");
    MakeNewLabel(l2);
    printf("\tJMP %s\t ; after this is else, so get out of if block \n", l2);
    PostLabel(l1);
    Block();
  }
  Match('e');
  PostLabel(l2);
}

// --------------------------------------------------------------
// Parse and Translate an Assignment Statement }

void Assignment()
{
  char name = GetName();
  Match('=');
  Expression();
  //             01234567890
  char line[] = "MOV [x], R8\t ; Save result in register";
  line[5] = name;
  EmitLn(line);
  declareGlobal(name);
}

int blockCounter = 0;
// Recognize and translate a Block
void Block()
{
  int thisBlock = blockCounter++;
  printf("\t; START BLOCK %i\n", thisBlock);
  while (Look != 'e' && // End
         Look != 'l')// && // Else
        //  Look != 'u')   // Untill
  {
    switch (Look)
    {
      case 'i': If(); break;
      // case 'w': While(); break;
      // case 'p': Loop(); break;
      // case 'r': Repeat(); break;
      // case 'f': For(); break;
      // case 'd': Do(); break;
      // case 'b': Break();
      case '\n': while(Look == '\n') Fin(); break;
      default: Assignment(); break;
    }
  }
  printf("\t; END BLOCK %i\n", thisBlock);
}

// Parse and Translate a program
void DoProgram()
{
  Block();
  if(Look != 'e') Expected("END");
}

// Initialize
void Init()
{
  GetChar();
}

// Main Program
int main(){
  // EmitHeader();
  Init();
  DoProgram();
  // EmitFooter();
  return 0;
}