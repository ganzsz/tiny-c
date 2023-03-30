#include <stdio.h>
#include <stdlib.h>


/**
 * form:
 * <program> ::= <block> END
 * <block> ::= [ <statement> ]*
 * <expression>  ::= [<addop> <term>]+
 * <term>        ::= <factor> [<mulop> <factor>]*
 * <factor>      ::= <number> | (<expression>) | <variable>
 * <number>      ::= <digit>
 * <addop>       ::= +|-
 * <mulop>       ::= *|/
*/

/// -------------------------------------------------------------
// Type declarations


// --------------------------------------------------------------
// Constant Declarations
#define MAX_AMOUNT_GLOBALS 26
#define MAX_AMOUNT_SUBROUT 26

#define MAX_LABEL_LEN 10

// --------------------------------------------------------------
// Variable Declarations

char Look; // Lookahead Character
int LabelCount = 0;

// Store the names of globals (all int for now)
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
// Checking if a global exists and defining one if it doesn't
// Forward declaration
void Abort(char* s);

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

// --------------------------------------------------------------
// Report an Error

void Error(char *s)
{
  printf("\n");
  printf("Error: %s.\n", s);
}

// --------------------------------------------------------------
// Report Error and Halt

void Abort(char *s)
{
  Error(s);
  exit(1);
}

// --------------------------------------------------------------
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
// Recognize a Decimal Digit

char IsWhite(char c)
{
  return c == ' ' || c == '\t';
}


// --------------------------------------------------------------
// Recognize a Decimal Digit

void SkipWhite()
{
  while (IsWhite(Look)) GetChar();
}

// --------------------------------------------------------------
// Match a Specific Input Character

void Match(char x)
{
  if (Look == x)
    GetChar();
  else
    Expected(charToStr(x));
}

// --------------------------------------------------------------
// Recognize an Alpha Character

int IsAlpha(char c)
{
  return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// --------------------------------------------------------------
// Recognize a Decimal Digit

char IsDigit(char c)
{
  return c >= '0' && c <= '9';
}


// --------------------------------------------------------------
// Get an Identifier

char GetName()
{
  if (!IsAlpha(Look))
    Expected("Name");
  char l = Look;
  GetChar();
  return l;
}

// --------------------------------------------------------------
// Get a Number

char GetNum()
{
  if (!IsDigit(Look))
    Expected("Integer");
  char l = Look;
  GetChar();
  return l;
}



char Label[MAX_LABEL_LEN];
void clearLabel(){for(int i=0; i<MAX_LABEL_LEN; i++)Label[i]='\0';}
// --------------------------------------------------------------
// Generate a Unique label
void NewLabel()
{
  clearLabel();
  sprintf(Label, "L%d", LabelCount++);
}

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

// --------------------------------------------------------------
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
    char line[] = "MOV R8, [x]\t ; Retrive var";
    line[9] = name;
    EmitLn(line);
    declareGlobal(name);
  }
}



// --------------------------------------------------------------
// Parse and Translate a Math Factor

// forward declaration
void Expression();

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
  { //             0123456789 012345678901234567890
    char Line[] = "MOV R8, 0\t ; Move num to R8";
    char c = GetNum();
    Line[8] = c;
    EmitLn(Line);
  }
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

// --------------------------------------------------------------
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
// Parse and Translate a Math Term

void Term()
{
  Factor();
  while (Look == '*' || Look == '/')
  {
    EmitLn("PUSH R8");
    switch (Look)
    {
    case '*':
      Multiply();
      break;
    case '/':
      Divide();
      break;
    default:
      Expected("Mulop");
      break;
    }
  }
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

// --------------------------------------------------------------
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
// Recognize an Addop

char IsAddop(char c)
{
  return c == '+' || c == '-';
}
// --------------------------------------------------------------
// Parse and Translate an Expression

void Expression()
{
  if (IsAddop(Look))
    EmitLn("MOV R8,0        ;Found + or - so we start with 0 in case of negation");
  else
    Term();

  while (IsAddop(Look))
  {
    EmitLn("PUSH R8\t");
    switch (Look)
    {
    case '+':
      Add();
      break;
    case '-':
      Subtract();
      break;
    default:
      Expected("Addop");
    }
  }
}


//--------------------------------------------------------------}
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



//---------------------------------------------------------------}
// Recognize and Translate an "other" (used to abnstract non control instructions)

// forward dec
void Block();

void Other()
{
  EmitLn(charToStr(GetName()));
}

//---------------------------------------------------------------
// Parse and translate a boolean condition
// For now a dummy

void Condition()
{
  EmitLn("<condition>");
}

void If()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match('i');
  Condition();
  MakeNewLabel(l1);
  strcopy(l1, l2, MAX_LABEL_LEN);
  printf("\tJE %s\t ; Jump Equal (false) to endif\n", l1);
  Block();
  if(Look == 'l')// else
  {
    Match('l');
    MakeNewLabel(l2);
    printf("\tJMP %s\t ; after this is else, so get out of if block \n", l2);
    PostLabel(l1);
    Block();
  }
  Match('e');
  PostLabel(l2);
}

void While()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match ('w');
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  PostLabel(l1);
  Condition();
  printf("\tJE %s\t ; Done with while if equal (0)\n");
  Block();
  Match('e');
  printf("\tJMP %s\t ; Go back to begin of while\n");
  PostLabel(l2);
}

// loop ( while(1))
void Loop()
{
  char l[MAX_LABEL_LEN];
  Match('p');
  MakeNewLabel(l);
  PostLabel(l);
  Block();
  Match('e');
  printf("\tJMP %s\t ; Repeat\n", l);
}

// do while repeat untill
void Repeat()
{
  char l[MAX_LABEL_LEN];
  Match('r');
  PostLabel(l);
  Block();
  Match('u');
  Condition();
  printf("\tJNE %s\t ; Jump back while not 0\n");
}

// --------------------------------------------------------------
//. Recognize and translate a statement block

void Block()
{
  while (Look != 'e' && // End
         Look != 'l' && // Else
         Look != 'u')   // Untill
  {
    switch (Look)
    {
      case 'i': If(); break;
      case 'w': While(); break;
      case 'p': Loop(); break;
      case 'r': Repeat(); break;
      default: Other(); break;
    }
  }
}

// --------------------------------------------------------------
// Parse and Translate a Program

void Program()
{
  Block();
  if (Look != 'e') Expected("End");
  EmitLn("END");
}



// --------------------------------------------------------------

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
  for(int i=0; i<sizeof(Subroutines); i++)
    if(Subroutines[i]=='\0') break;
    else printf("__%c:\n\tret\n", Subroutines[i]);


  // Reserve memory
  printf("section     .data\n");
  printf("\tmsg    db  0x00\n"); // Reserved for outputting on eof

  // Reserver memory for the constants
  for(int i=0; i<sizeof(Globals); i++)
    if(Globals[i]=='\0') break;
    else printf("\t%c      dw 0x0000\n", Globals[i]);
}

// --------------------------------------------------------------
// Initialize

void Init()
{
  GetChar();
}

// --------------------------------------------------------------
// Main Program

int main(){
  // EmitHeader();
  Init();
  Program();
  // EmitFooter();
  return 0;
}

    // --------------------------------------------------------------