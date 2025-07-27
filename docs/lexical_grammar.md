# Lexical Grammar
## Regexes
_{0,2}[a-z][a-zA-Z0-9_]*
[A-Z][a-zA-Z0-9]*
[\+-]?[0-9]+
0[xX][a-fA-F0-9]{1,16}
0[oO][0-7]{1,22}
0[bB][01]{1,64}
([\+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+))[eE]([\+-]?[0-9]+)
[\+-]?(?:[0-9]+\.[0-9]*|\.[0-9]+)
(r?)"((?:[^"\\]|\\")*)
#.*$

## Baxkus-Naur Form

<ident> ::= <lower> <ident?>
          | "_" <lower> <ident?>
          | "_" "_" <lower> <ident?>
<ident?> ::= <lower> <ident?>
           | <upper> <ident?>
           | <digit> <ident?>
           | "_" <ident?>
           | e
<type_ident> ::= <upper> <type_ident?>
<type_ident?> ::= <type_ident>
                | <lower> <type_ident?>
                | <digit> <type_ident?>
                | e
<float_const> ::= <decimal_const>
                | <mantissa>
                | <hex_const>
                | <octal_const>
                | <binary_const>
                | <scientific>
<integer_const> ::= <decimal_const>
                  | <hex_const>
                  | <octal_const>
                  | <binary_const>
