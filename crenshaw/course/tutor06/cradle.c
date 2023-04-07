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
// Helper to see if char is in char array

char inArray(char c, char* a, int len)
{
  for(int i = 0; i < len; i++)
    if(a[i] == c) return 1;
  return 0;
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
// Recognize a Boolean Literal

char IsBoolean(char c)
{
  char boolliterals[] = {'T', 'F', 't', 'f'};
  return inArray(c, boolliterals, sizeof(boolliterals));
}

char IsTrueLiteral(char c)
{
  char trueLiterals[] = {'T', 't'};
  return inArray(c, trueLiterals, sizeof(trueLiterals));
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

// --------------------------------------------------------------
// Get Boolean Value

char GetBoolean()
{
  if(!IsBoolean(Look)) Expected("Boolean Literal");
  char c = IsTrueLiteral(Look);
  GetChar();
  return c;
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

// void Expression()
// {
//   if (IsAddop(Look))
//     EmitLn("MOV R8,0        ;Found + or - so we start with 0 in case of negation");
//   else
//     Term();

//   while (IsAddop(Look))
//   {
//     EmitLn("PUSH R8\t");
//     switch (Look)
//     {
//     case '+':
//       Add();
//       break;
//     case '-':
//       Subtract();
//       break;
//     default:
//       Expected("Addop");
//     }
//   }
// }


// -------------------------------------------------------------
// Recognaze a relational equals (= -> ==)
void Equals()
{
  Match('=');
  EmitLn("; ==");
  Expression();
  EmitLn("POP R9");
  EmitLn("CMP R8, R9");
  EmitLn("CMOVE R8, -1\t ; Make 1 on equal");
  EmitLn("CMOVNE R8, 0\t ; Make 0 on not equal");
}

// Recognaze a relational Notequals (# -> !=)
void NotEquals()
{
  Match('#');
  EmitLn("; !=");
  Expression();
  EmitLn("POP R9");
  EmitLn("CMP R8, R9");
  EmitLn("CMOVE R8, 0\t ; Make 0 on equal");
  EmitLn("CMOVNE R8, -1\t ; Make 1 on not equal");
}

// Recognaze a relational Less (<)
void Less()
{
  Match('<');
  EmitLn("; <");
  Expression();
  EmitLn("POP R9");
  EmitLn("CMP R8, R9");
  EmitLn("CMOVL R8, -1\t ; Make 1 on <");
  EmitLn("CMOVGE R8, 0\t ; Make 0 on >= ");
}

// Recognaze a relational Greater
void Greater()
{
  Match('>');
  EmitLn("; >");
  Expression();
  EmitLn("POP R9");
  EmitLn("CMP R8, R9");
  EmitLn("CMOVG R8, -1\t ; Make 1 on >");
  EmitLn("CMOVLE R8, 0\t ; Make 0 on <= ");
}

// Recognise relop
char IsRelop(char c)
{
  char relops[] = {'=', '#', '<', '>'};
  return inArray(c, relops, sizeof(relops));
}

// Parse and translate Relation (1<2)
void Relation()
{
  Expression();
  if(IsRelop(Look))
  {
    switch (Look)
    {
      case '=': Equals(); break;
      case '#': NotEquals(); break;
      case '<': Less(); break;
      case '>': Greater(); break;
    }
  }
}

// -------------------------------------------------------------
// Parse and Translate a Boolean Factor with NOT
void BoolFactor()
{
  if(IsBoolean(Look))
  {
    if(GetBoolean()) EmitLn("MOV R8, -1\t ; True literal found");
    else EmitLn("MOV R8, 0\t ; False literal found");
  } else {
    Relation();
  }
}

void NotFactor()
{
  if(Look == '!')
  {
    Match('!');
    EmitLn("; !");
    BoolFactor();
    EmitLn("XOR R8, -1\t ; Inverse boolean factor because of !");
  }
  else
  {
    BoolFactor();
  }
}

// -------------------------------------------------------------
// parse and translate Boolean Term
void BoolTerm()
{
  NotFactor();
  while (Look == '&')
  {
    EmitLn("; &");
    EmitLn("PUSH R8");
    Match('&');
    NotFactor();
    EmitLn("POP R9");
    EmitLn("AND R8, R9");
  }
}

// -------------------------------------------------------------
// Recognize and Translate a Boolean OR.
void BoolOr()
{
  Match('|');
  BoolTerm();
  EmitLn("; |");
  EmitLn("POP R9\t ; Get other b-term from stack");
  EmitLn("OR R8, R9\t ; Perform or operation");
}

// Recognize and Translate an Exclusive Or
void BoolXor()
{
  Match('~');
  BoolTerm();
  EmitLn("; |");
  EmitLn("POP R9\t ; Get other b-term from stack");
  EmitLn("XOR R8, R9\t ; Perform or operation");
}

// Recognize an Or Operator
char IsOrOp(char c)
{
  char orOperators[] = {'|', '~'};
  return inArray(c, orOperators, sizeof(orOperators));
}

// -------------------------------------------------------------
// Parse and Translate Boolean Expression
void BoolExpression()
{
  BoolTerm();
  while (IsOrOp(Look))
  {
    EmitLn("PUSH R8\t");
    switch (Look)
    {
      case '|': BoolOr(); break;
      case '~': BoolXor(); break;
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
void Block(char * breakLabel);

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

// Parse and translate an expression
// for now a dummy

void Expression()
{
  EmitLn("<expression>");
}

void If(char* breakLabel)
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match('i');
  Condition();
  MakeNewLabel(l1);
  strcopy(l1, l2, MAX_LABEL_LEN);
  EmitLn("; IF");
  printf("\tJE %s\t ; Jump Equal (false) to endif or else\n", l1);
  Block(breakLabel);
  if(Look == 'l')// else
  {
    Match('l');
    EmitLn("; ELSE");
    MakeNewLabel(l2);
    printf("\tJMP %s\t ; after this is else, so get out of if block \n", l2);
    PostLabel(l1);
    Block(breakLabel);
  }
  Match('e');
  PostLabel(l2);
}

void While()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match ('w');
  EmitLn("; WHILE");
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  PostLabel(l1);
  Condition();
  printf("\tJE %s\t ; Done with while if equal (0)\n", l2);
  Block(l2);
  Match('e');
  printf("\tJMP %s\t ; Go back to begin of while\n", l1);
  PostLabel(l2);
}

// loop ( while(1))
void Loop()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match('p');
  EmitLn("; WHILE");
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  PostLabel(l1);
  Block(l2);
  Match('e');
  printf("\tJMP %s\t ; Repeat\n", l1);
  PostLabel(l2);
}

// do while repeat untill
void Repeat()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  Match('r');
  EmitLn("; REPEAT");
  PostLabel(l1);
  Block(l2);
  Match('u');
  Condition();
  printf("\tJNE %s\t ; Jump back while not 0\n", l1);
  PostLabel(l2);
}

// for i = l to h (for i in range(l, h))
void For()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN];
  Match('f');
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  char name = GetName();
  declareGlobal(name);
  Match('=');
  EmitLn("; FOR");
  Expression();
  EmitLn("DEC R8\t ; We want to start one lower bc we increment in the check");
                    //0213456789
  char storeLine[] = "MOV [x], R8\t ; Save counter var for For";
  storeLine[5] = name;
  EmitLn(storeLine);
  Expression();
  EmitLn("PUSH R8\t ; Store the to on the stack");
  PostLabel(l1);
                   //01234567890123
  char loadLine[] = "MOV R8, [x]\t ; Load counter var for For";
  loadLine[9] = name;
  EmitLn(loadLine);
  EmitLn("INC R8\t; Increment counter var for For");
  EmitLn("POP R9\t ; Load max For num in R9");
  EmitLn("PUSH R9\t ; And store it");
  EmitLn("CMP R9, R8\t ; See if we are ready to leave the loop");
  printf("\tJG %s\t ; Jump if counter > max\n", l2);
  Block(l2);
  Match('e');
  printf("\tJMP %s \t ; Return to start For loop\n", l1);
  PostLabel(l2);
}

// Do x (repeat block x amount of times)
void Do()
{
  char l1[MAX_LABEL_LEN], l2[MAX_LABEL_LEN], out[MAX_LABEL_LEN];
  Match('d');
  MakeNewLabel(l1);
  MakeNewLabel(l2);
  MakeNewLabel(out);
  EmitLn("; DO");
  Expression();
  EmitLn("MOV CX, R8\t; CX is the register we compare");
  PostLabel(l1);
  EmitLn("DEC CX");
  EmitLn("PUSH CX");
  Block(out);
  Match('e');
  EmitLn("POP CX");
  printf("\tJCXZ %s \t ; Jump out of loop if CX=zero \n", l2);
  printf("\tJMP %s \t ; Repaet do\n", l1);
  PostLabel(out);
  EmitLn("POP CX\t ; Only hit when we break out of the loop");
  PostLabel(l2);
}

// Break statement
void Break(char* label)
{
  Match('b');
  if(label[0] != '\0')
    printf("\tJMP %s\t; Encountered a break\n", label);
  else Abort("Encountered break outside of loop");
}


// --------------------------------------------------------------
// Recognize and translate a statement block
// <argument> breakLabel The label to jump to when encountering a break
int blockCounter = 0;
void Block(char * breakLabel)
{
  int thisBlock = blockCounter++;
  printf("\t; START BLOCK %i\n", thisBlock);
  while (Look != 'e' && // End
         Look != 'l' && // Else
         Look != 'u')   // Untill
  {
    switch (Look)
    {
      case 'i': If(breakLabel); break;
      case 'w': While(); break;
      case 'p': Loop(); break;
      case 'r': Repeat(); break;
      case 'f': For(); break;
      case 'd': Do(); break;
      case 'b': Break(breakLabel);
      default: Other(); break;
    }
  }
  printf("\t; START BLOCK %i\n", thisBlock++);
}

// --------------------------------------------------------------
// Parse and Translate a Program

void Program()
{
  Block("");
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
  EmitHeader();
  Init();
  // Program();
  BoolExpression();
  EmitFooter();
  return 0;
}

    // --------------------------------------------------------------