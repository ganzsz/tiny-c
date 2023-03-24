{--------------------------------------------------------------}
program Cradle;

{--------------------------------------------------------------}
{ Constant Declarations }

const TAB = ^I;

{--------------------------------------------------------------}
{ Variable Declarations }

var Look: char; { Lookahead Character }

{--------------------------------------------------------------}
{ Read New Character From Input Stream }

procedure GetChar;
begin
Read(Look);
end;

{--------------------------------------------------------------}
{ Report an Error }

procedure Error(s: string);
begin
WriteLn;
WriteLn(^G, 'Error: ', s, '.');
end;


{--------------------------------------------------------------}
{ Report Error and Halt }

procedure Abort(s: string);
begin
Error(s);
Halt;
end;


{--------------------------------------------------------------}
{ Report What Was Expected }

procedure Expected(s: string);
begin
Abort(s + ' Expected');
end;

{--------------------------------------------------------------}
{ Match a Specific Input Character }

procedure Match(x: char);
begin
if Look = x then GetChar
else Expected('''' + x + '''');
end;


{--------------------------------------------------------------}
{ Recognize an Alpha Character }

function IsAlpha(c: char): boolean;
begin
IsAlpha := upcase(c) in ['A'..'Z'];
end;

{--------------------------------------------------------------}

{ Recognize a Decimal Digit }

function IsDigit(c: char): boolean;
begin
IsDigit := c in ['0'..'9'];
end;


{--------------------------------------------------------------}
{ Get an Identifier }

function GetName: char;
begin
if not IsAlpha(Look) then Expected('Name');
GetName := UpCase(Look);
GetChar;
end;


{--------------------------------------------------------------}
{ Get a Number }

function GetNum: char;
begin
if not IsDigit(Look) then Expected('Integer');
GetNum := Look;
GetChar;
end;


{--------------------------------------------------------------}
{ Output a String with Tab }

procedure Emit(s: string);
begin
Write(TAB, s);
end;


{--------------------------------------------------------------}
{ Output a String with Tab and CRLF }

procedure EmitLn(s: string);
begin
Emit(s);
WriteLn;
end;


{---------------------------------------------------------------}
{ Parse and Translate a Math Factor }

{forward declaration}
procedure Expression; Forward;

procedure Factor;
begin
  if Look = '(' then begin
    Match('(');
    Expression;
    Match(')');
    end
  else
    EmitLn('MOV R8, ' + GetNum + TAB + ' ; Move num to R8');
end;


{--------------------------------------------------------------}
{ Recognize and Translate a Multiply }

procedure Multiply;
begin
   Match('*');
   Factor;
   EmitLn('POP R9');
   EmitLn('IMUL R8,R9');
end;


{-------------------------------------------------------------}
{ Recognize and Translate a Divide }

procedure Divide;
begin
   Match('/');
   Factor;
   EmitLn('POP RAX       ; Top half');
   EmitLn('MOV RDX, 0    ; Bot half');
   EmitLn('IDIV R8       ; Divide above by R8');
   EmitLn('MOV R8, RAX   ; Store result in R8')
end;


{---------------------------------------------------------------}
{ Parse and Translate a Math Term }

procedure Term;
begin
   Factor;
   while Look in ['*', '/'] do begin
      EmitLn('PUSH R8');
      case Look of
       '*': Multiply;
       '/': Divide;
      else Expected('Mulop');
      end;
   end;
end;


{--------------------------------------------------------------}
{ Recognize and Translate an Add }

procedure Add;
begin
   Match('+');
   Term;
   EmitLn('POP R9');
   EmitLn('ADD R8, R9');
end;


{-------------------------------------------------------------}
{ Recognize and Translate a Subtract }

procedure Subtract;
begin
   Match('-');
   Term;
   EmitLn('POP R9');
   EmitLn('SUB R8, R9');
   EmitLn('NEG R8');
end;


{--------------------------------------------------------------}
{ Recognize an Addop }

function IsAddop(c: char): boolean;
begin
   IsAddop := c in ['+', '-'];
end;
{--------------------------------------------------------------}
{ Parse and Translate an Expression }

procedure Expression;
begin
   if IsAddop(Look) then
      EmitLn('MOV R8,0        ;Found + or - so we start with 0 in case of negation')
   else
      Term;
   while IsAddop(Look) do begin
    EmitLn('PUSH R8' + TAB);
    case Look of
      '+': Add;
      '-': Subtract;
    else Expected('Addop');
    end;
  end;
end;
{--------------------------------------------------------------}


{---------------------------------------------------------------}
{ WriteHeader }

procedure WriteHeader;
begin
Write('section     .text');
WriteLn;
Write('global      _start ');
WriteLn;
WriteLn;
Write('_start: ');
WriteLn;
Write('; ============== THE GENERATED PROGRAM ==============');
WriteLn;
end;


{---------------------------------------------------------------}
{ WriteFooter }

procedure EmitFooter;
begin
Write('; ============== END GENERATED PROGRAM ==============');
WriteLn;
WriteLn;
{Magic to print R8}
EmitLn('; Print register R8');
EmitLn('ADD R8, 48'+TAB+' ; convert to ascii');
EmitLn('MOV [output], R8'+TAB+' ; store in output');
EmitLn('MOV RDX,1'+TAB+' ; length to print');
EmitLn('MOV RCX, output'+TAB+' ; point to output');
EmitLn('MOV RBX,1');
EmitLn('MOV RAX,4');
EmitLn('INT 0x80');
EmitLn('MOV RAX,1');
EmitLn('INT 0x80');
WriteLn;

{stop program}
EmitLn('; Stop gracefully');
EmitLn('mov rax, 60       ; exit(');
EmitLn('mov rdi, 0        ;   EXIT_SUCCESS');
EmitLn('syscall           ; );');

{Reserve memory}
Write('section     .data');
WriteLn;
Write('output     db  0,0xa'); { Reserved for outputting on eof}
end;


{---------------------------------------------------------------}
{ Initialize }

procedure Init;
begin
GetChar;
end;

{
  form:
  <expression>  ::= <term> [<addop> <term>]*
  <term>        ::= <factor> [<mulop> <factor>]*
  <factor>      ::= [+|-](<expression>) | [+|-]0-9
  <addop>       ::= +|-
  <mulop>       ::= *|/
}

{--------------------------------------------------------------}
{ Main Program }

begin
WriteHeader;
Init;
Expression;
EmitFooter;
end.
{--------------------------------------------------------------}