#include <stdio.h>
#include <stdlib.h>

/**
 * form:
 * <assignment>   ::= <ident> = <expression>
 * <expression>   ::= <first term> ( <addop> <term> )*
 * <first term>   ::= <first factor> <rest>
 * <term>         ::= <factor> <rest>
 * <rest>         ::= ( <mulop> <factor> )*
 * <first factor> ::= [ <addop> ] <factor>
 * <factor>       ::= <var> | <number> | ( <expression> )
 * 
 * <bool-expr> ::= <bool-term> ( <orop> <bool-term> )*
 * <bool-term> ::= <not-factor> ( <andop> <not-factor> )*
 * <not-factor> ::= [ '!' ] <relation>
 * <relation> ::= <expression> [ <relop> <expression> ]
*/

/**
 * Type Declarations
*/

typedef char Symbol[9];

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

// Is a global defined
char InTable(char name)
{
  for(int i=0; i<MAX_AMOUNT_GLOBALS; i++)
    if(Globals[i]==name) return 1;
  return 0;
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

void Undefined(char s) {
  char ender[] = "  Undefined";
  ender[0] = s;
  Abort(ender);
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

// Recognize a relative operator
char IsRelop(char c) {
  char relLiterals[] = {'=', '#', '<', '>'};
  return inArray(c, relLiterals, sizeof(relLiterals));
}

// Recognise a multiplication operator
char IsMulop(char c) {
  char muloperators[] = {'*', '/'};
  return inArray(c, muloperators, sizeof(muloperators));
}

// Recognise an add operator
char IsAddop(char c) {
  char addoperators[] = {'+', '-'};
  return inArray(c, addoperators, sizeof(addoperators));
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

void LineHead(char* s) { Emit(s); }
void LinePart(char* s) { printf("%s", s); }
void LineTail(char* s) { printf("%s\n", s); }
void EmitChar(char c) { printf("%c", c); }

// Emit an integer as characters
void EmitInt(int n) {
  if(n<10) {
    EmitChar(n+'0');
  } else {
    EmitInt(n / 10);
    EmitChar(n % 10);
  }
}

// ==========================================
// ASSEMBLY SECTION

// Clear primary register
void Clear() {EmitLn("MOV R8, 0\t ; Clear primary register");}
// Negate Primary register
void Negate() {EmitLn("NEG R8\t ; Negate primary register");}
// Load constant value to primary register
void LoadConst(int n) {
  LineHead("MOV R8, ");
  if(n < 0) {
    EmitChar('-');
    n = n * -1;
  }
  EmitInt(n);
  LineTail("\t ; Load constant in primary register");
}
// Load a Variable to Primary Register
void LoadVar(char name) {
  if(!InTable(name)) Undefined(name);
  LineHead("MOV R8, [");
  EmitChar(name);
  LineTail("]\t ; Load var into primary register");
}
// Push primary register to stack
void Push() { EmitLn("PUSH R8"); }
// Add top of stack to primary register
void PopAdd() {
  EmitLn("; +");
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("ADD R8, r9\t ; Add item to primary reg");
}
// subtrackt primary from top of stack
void PopSub() {
  EmitLn("; -");
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("SUB R8, R9\t ; Subtract");
  EmitLn("NEG R8");
}
// Multiply top of stack by primary
void PopMul() {
  EmitLn("; *");
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("IMUL R8, R9\t ; Multiply item with primary");
}
// Divide top of stack by primary
void PopDiv() {
  EmitLn("; /");
  EmitLn("POP RAX\t ; Pop to top half");
  EmitLn("MOV RDX, 0\t ; Clear bot half");
  EmitLn("IDIV R8\t ; Divide RDX,RAX by Primary");
  EmitLn("MOV R8, RAX\t ; Store result in primary");
}
// Store primary to variable
void Store(char name) {
  if(!InTable(name)) Undefined(name);
  LineHead("MOV [");
  EmitChar(name);
  LineTail("], R8\t ; Store primary in variable");
}
// Complement the Primary Register
void NotIt() {
  EmitLn("NOT R8\t ; Ones Complement primary");
}
// And top of stack with Primary
void PopAnd() {
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("AND R8b, R9b\t ; AND byte with primary register");
}
// Or top of stack with Primary
void PopOr() {
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("OR R8b, R9b\t ; OR byte with primary register");
}
// Xor top of stack with Primary
void PopXor() {
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("XOR R8b, Rb9\t ; XOR byte with primary register");
}
// Compare top o fstack with primary
void PopCompare() {
  EmitLn("POP R9\t ; Pop to working register");
  EmitLn("CMP R8b, R9b\t ; Modify status reg as an SUB");
}
// Set primary if Compare was =
void SetEqual() {
  EmitLn("; =");
  EmitLn("SETNE R8b\t ; Set primary if last was true");
  EmitLn("DEC R8b\t ; Inverse result to get desired result");
}
// Set primary if Compare was !=
void SetNEqual() {
  EmitLn("; !=");
  EmitLn("SETE R8b\t ; Set primary if last was false");
  EmitLn("DEC R8b\t ; Inverse result to get desired result");
}
// Set primary if Compare was >
void SetLess() {
  EmitLn("; <");
  EmitLn("SETGE R8b\t ; Set primary if last was false");
  EmitLn("DEC R8b\t ; Inverse result to get desired result");
}
// Set primary if Compare was <
void SetGreater() {
  EmitLn("; >");
  EmitLn("SETLE R8b\t ; Set primary if last was false");
  EmitLn("DEC R8b\t ; Inverse result to get desired result");
}



void Expression();

// Parse and translate a Math factor
void Factor() {
  if(Look == '(') {
    Match('(');
    Expression();
    Match(')');
  } else if (IsAlpha(Look)) {
    LoadVar(GetName);
  } else {
    LoadConst(GetNum);
  }
}

// Parse and translate a negative factor
void NegFactor() {
  Match('-');
  if(IsDigit(Look)) {
    LoadConst(-GetNum());
  } else {
    Factor();
    Negate();
  }
}

// Parse and translate a Leading Factor
void FirstFactor() {
  switch (Look) {
    case '+':
      Match('+');
      Factor();
      break;
    case '-':
      NegFactor();
      break;
    default:
      Factor();
  }
}

// Recognise and Translate a Multiply
void Multiply() {
  Match('*');
  Factor();
  PopMul();
}

// Recognise and Translate a Divide
void Divide() {
  Match('/');
  Factor();
  PopDiv();
}

// shared code used by Term and Firstterm
void Term1() {
  while(IsMulop(Look)) {
    Push();
    switch (Look) {
      case '*': Multiply(); break;
      case '/': Divide(); break;
    }
  }
}

// Parse and translate a Math term
void Term() {
  Factor();
  Term1();
}

// Parse and translate a Leading Term
void FirstTerm() {
  FirstFactor();
  Term1();
}

// Recognize and translate an Add
void Add() {
  Match('+');
  Term();
  PopAdd();
}


// Recognize and translate a Subtract
void Subtract() {
  Match('-');
  Term();
  PopSub();
}

// Parse and Translate an Expression
void Expression() {
  FirstTerm();
  while(IsAddop(Look)) {
    switch (Look) {
      case '+': Add(); break;
      case '-': Subtract(); break;
    }
  }
}

// Parse and Translate an Assignment Statement
void Assignment() {
  char name = GetName();
  Match('=');
  Expression();
  Store(name);
}