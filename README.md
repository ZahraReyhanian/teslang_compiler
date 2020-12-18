# teslang_compiler
```
a compiler for teslang with this bnf :
prog ::= func |
func prog
func ::= type iden ( flist ) { body } |
body ::= stmt |
stmt body
stmt ::= expr ; |
defvar ; |
if ( expr ) stmt |
if ( expr ) stmt else stmt |
while ( expr ) stmt |
for ( iden in expr ) stmt |
return expr ; |
{ body }
defvar ::= type iden
expr ::= iden ( clist ) |
expr [ expr ] |
expr = expr |
expr + expr |
expr - expr |
expr * expr |
expr / expr |
expr % expr |
expr < expr |
expr > expr |
expr == expr |
expr != expr |
expr <= expr |
expr >= expr |
expr || expr |
expr && expr |
! expr |
- expr |
+ expr |
( expr ) |
iden |
num
flist ::= |
type iden |
type iden , flist
clist ::= |
expr |
expr , clist
type ::= num |
list
num ::= [0-9]+
iden ::= [a-zA-Z_][a-zA-Z_0-9]*
```
