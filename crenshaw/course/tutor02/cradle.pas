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
{ Parse and Translate a Term }

procedure Term;
begin
EmitLn('MOV RDX, ' + GetNum + TAB + ' ; Move num to RDX')
end;


{--------------------------------------------------------------}
{ Recognize and Translate an Add }

procedure Add;
begin
   Match('+');
   Term;
   EmitLn('POP RAX');
   EmitLn('ADD RDX, RAX');
end;


{-------------------------------------------------------------}
{ Recognize and Translate a Subtract }

procedure Subtract;
begin
   Match('-');
   Term;
   EmitLn('POP RAX');
   EmitLn('SUB RDX, RAX');
   EmitLn('NEG RDX');
end;


{---------------------------------------------------------------}
{
  form:
  <expression> ::= <term> [<addop> <term>]*
  <term> ::= 0-9
  <addop> ::= +|-
}
{ Parse and Translate an Expression }

procedure Expression;
begin
   Term;
   while Look in ['+', '-'] do begin
    EmitLn('PUSH RDX' + TAB);
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
{Magic to print RDX}
EmitLn('; Print register RDX');
EmitLn('ADD RDX, 48'+TAB+' ; convert to ascii');
EmitLn('MOV [output], RDX'+TAB+' ; store in output');
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


{--------------------------------------------------------------}
{ Main Program }

begin
WriteHeader;
Init;
Expression;
EmitFooter;
end.
{--------------------------------------------------------------}