/*
    prog -> func | func prog
    func -> type iden ( flist ) { body } |
    body -> stmt |
            stmt body
    stmt -> expr ; |
            defvar ; |
            if ( expr ) stmt |
            if ( expr ) stmt else stmt |
            while  ( expr ) stmt |
            for (iden in expr ) stmt |
            return expr; |
            { body }
    defvar -> type iden

    expr -> iden ( clist ) |
            assign-expr
    assign-expr -> cond-expr |
                    unary-expr  = assign-expr
    cond-expr -> or-expr
    or-expr -> and-expr |
                or-expr || and-expr
    and-expr -> equ-expr |
                and-expr && equ-expr
    equ-expr -> relational-expr |
                equ-expr == relational-expr
                equ-expr != relational-expr
    relational-expr -> add-expr |
                        relational-expr < add-expr |
                        relational-expr > add-expr |
                        relational-expr <= add-expr |
                        relational-expr >= add-expr
    add-expr -> mul-expr |
                add-expr + mul-expr
                add-expr - mul-expr
    mul-expr -> unary-expr |
                mul-expr * unary-expr |
                mul-expr / unary-expr |
                mul-expr % unary-expr 

    unary-expr -> postfix-expr |
                    unary-oprator unary-expr
    postfix-expr -> prim-expr  |
                    postfix-expr [expr]
    prim-expr -> iden |
                num |
                (expr)
    unary-oprator -> + |
                    - |
                    !

    flist ->  |
            type iden |
            type iden , flist
    clist -> |
            expr |
            expr , clist
    type -> num |
            list
    num -> [0-9]+
    iden -> [a-zA-Z_][a-zA-Z_0-9]
*/

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

char c;
ifstream file;
string token;
int num_err;
int count_line;
string IF = "if", ELSE = "else", IN = "in", WHILE = "while", FOR = "for", RETURN = "return", LIST = "list", NUM = "num",
NUMREAD = "numread", NUMPRINT = "numprint", LISTLEN = "listlen", EXIT = "exit";

char getChar();
void ungetchar();
int isOpChar(char ch);
int next(char expected);
string checkOp(char ch);
string getString(string answer, char ch);
void skipComment();
char skipSpace(char ch);
string nextToken();
string getToken();
void syntax_error(string err);
void dropToken();
bool isFileEnd();
bool isKey(string tok);
bool isIdentifier(string tok);
bool isNum(string tok);
void prog();
void func();
void body();
void stmt();
void defvar();
void expr();
void assign_expr();
void cond_expr();
void or_expr();
void and_expr();
void equ_expr();
void relational_expr();
void add_expr();
void mul_expr();
void unary_expr();
void postfix_expr();
void prim_expr();
void unary_oprator();
void flist();
void clist();
void type();
void num();
void iden();

struct table
{
    string var;
    string type;
    int value;
    bool isFunc;
    int numberOfparam;
    vector<table> list;

};
vector<table> symbolTable;

int main () {
    file.open ("grammer.txt");
    if(! file){
        cout << "file didn't open successfully !";
        exit(1);
    }
    else
    {
        cout << "file opened successfully !\n";
    }

    count_line = 1;
    token = nextToken();
    num_err = 0;
    startgrammer();
    return 0;
}


char getChar(){
    if(file.is_open())
        if(file.get(c)){
            return c ;
        }
        
    return '\0';
    
}

void ungetchar(){
    file.seekg(-1,ios::cur);
}

int isOpChar(char ch){
    if(ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == ',' ||
                 ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || 
                 ch == '<' || ch == '>' || ch == '!' || ch == '|' || ch == '&' || ch == ';' )
                 return 1;
    return 0;
}

int next(char expected){
    if(getChar() == expected) return 1;
    ungetchar();
    return 0;
}

string checkOp(char ch){
    string a = string(1,ch);
    switch(ch){
        case '!': next('=') == 1 ? a = "!=" : a = "!"; break;
        case '=': next('=') == 1 ? a = "==" : a = "="; break;
        case '>': next('=') == 1 ? a = ">=" : a = ">"; break;
        case '<': next('=') == 1 ? a = "<=" : a = "<"; break;
        case '|': next('|') == 1 ? a = "||" : a = "|"; break;
        case '&': next('&') == 1 ? a = "&&" : a = "&"; break;
    }
    return a;
}


string getString(string answer, char ch){
    //todo check string is valid or not
    while (! file.eof() && ch != '\0' && ! isOpChar(ch) && ch != ' ' && ch != '\n' && ch != '#' )
    {
            answer += ch;
            ch = getChar();
    }
    if (isOpChar(ch) || ch == '#' || ch == ' ')
    {
        ungetchar();
    }
    return answer;
}

void skipComment(){
    string temp;
    ungetchar();
    getline(file, temp);
}

char skipSpace(char ch){
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\r'  || ch == '\t')){
        ch = getChar();
    }
    return ch;
}

string nextToken(){
    string answer;
    char ch ;
    ch = getChar();
    if (file.eof()){
        return "";
    }
    if (ch == '#'){
        skipComment();
        ch = getChar();
    }

    ch = skipSpace(ch);
    //////////////////////////////////
    if(isOpChar(ch)){
        return checkOp(ch);
    }
    else {
        answer = getString(answer, ch);
        if (answer == " " || answer == "\n" || answer == "\r" || answer == "\t"){
            nextToken();
        }
        return answer;
    }
    

}

string getToken(){
    return token;
}

void dropToken(){
    token = nextToken();
}

bool isFileEnd(){
    return file.eof();
}

void syntax_error(string err){
    cout << count_line << ": " << err << endl;
    num_err ++;
}

bool isKey(string tok){
    if (tok == IF || tok == ELSE || tok == IN || tok == WHILE || tok == FOR || tok == RETURN || tok == LIST || tok == NUM ||
    tok == NUMREAD || tok == NUMPRINT || tok == LISTLEN || tok == EXIT)
        return true;
    else return false;   
    
}

bool isIdentifier(string tok){
    if(!isKey(tok))
        if(regex_match(tok, regex("[a-zA-Z_][a-zA-Z_0-9]*"))) return true;
    else return false;
}

bool isNum(string tok){
    if(regex_match(tok, regex("[0-9]+"))) return true;
    else return false;
}

void prog(){
    while (! isFileEnd() && getToken() != "\0")
    {
        func();
    }
    if(getToken() == "\0"){
        if (num_err == 0)
        {
            cout << "compiled successfully !\n";
            exit(0); 
        }
        else
        {
            cout << "number of errors : " << num_err << endl;
            cout << "compilation was failed !";
            exit(1); 
        }        
       
    }
    else {
        syntax_error("Unexcepted Token " + getToken());
        cout << "number of errors : " << num_err << endl;
        cout << "compilation was failed !";
        exit(1); 
    }
}

void func(){
    type();
    iden();
    if(getToken() == "("){
        dropToken();
        flist();
        if(getToken() == ")"){
            dropToken();
            if (getToken() == "{"){
                body();
                if(getToken() == "}"){
                    dropToken();
                    return;
                }
                else syntax_error("expected }");
            }
            else syntax_error("expected {");
        }
        else syntax_error("expected )");
    }
    else syntax_error("expected (");
}

void body(){
    while (! isFileEnd() && getToken() != "}"){
        stmt();
    }
}

void stmt(){

}

void defvar(){
    type();
    iden();
}
//expr -> iden ( clist ) |
  //          assign-expr
void expr(){
    if(isIdentifier(getToken())){
        iden();
        if(getToken() == "("){
            dropToken();
            clist();
            if(getToken() == ")"){
                dropToken();
            }
            else
            {
                syntax_error("expected )");
            }
        }
        else
        {
            syntax_error("expected (");
        }
    }
    else
    {
        assign_expr();
    }
    
}

//assign-expr -> cond-expr |
     //               unary-expr  = assign-expr
void assign_expr(){
    unary_expr();
    while (getToken() == "=")
    {
        dropToken();
        cond_expr();
    }
    
}

void cond_expr(){
    or_expr();
}

    
void or_expr(){
    and_expr();
    while (getToken() == "||")
    {
        dropToken();
        and_expr();
    }  
}

void and_expr(){
    equ_expr();
    while (getToken() == "&&")
    {
        dropToken();
        equ_expr();
    }
    
}

void equ_expr(){
    relational_expr();
    while (getToken() == "==" || getToken() == "!=")
    {
        dropToken();
        relational_expr();
    }
    
}

void relational_expr(){
    add_expr();
    while (getToken() == ">" || getToken() == "<" || getToken() == ">=" || getToken() == "<=")
    {
        dropToken();
        add_expr();
    }
}

void add_expr(){
    mul_expr();
    while (getToken() == "+" || getToken() == "-")
    {
        dropToken();
        mul_expr();
    }
    
}

void mul_expr(){
    unary_expr();
    while (getToken() == "*" || getToken() == "/" || getToken() == "%")
    {
        dropToken();
        unary_expr();
    }
}

void unary_expr(){
    unary_oprator();
    while (getToken() == "+" || getToken() == "-" || getToken() == "!")
    {
        unary_oprator();
    }
    postfix_expr();
}

void postfix_expr(){
    prim_expr();
    while (getToken() == "[")
    {
        dropToken();
        expr();
        if(getToken() == "]"){
            dropToken();
            continue();
        }
        else
        {
            syntax_error("expected ]");
        }
        
    }
    
}

void prim_expr(){
    if (isIdentifier(getToken()))
    {
        iden();
    }
    else if(isNum(getToken()))
    {
        num();
    }
    else if(getToken() == "("){
        dropToken();
        expr();
        if (getToken() == ")")
        {
            dropToken();
        }
        else
        {
            syntax_error("expected )");
        }
        
    }
    else
    {
        syntax_error("primary expression is not ok!");
    }
    
    
}

void unary_oprator(){
    if(getToken() == "+" || getToken() == "-" || getToken() == "!") dropToken();        
}

void flist(){
    type();
    iden();
    while (! isFileEnd() && getToken() == ",")
    {
        dropToken();
        type();
        iden();
    }
    
}

void clist(){
    expr();
    while(! isFileEnd() && getToken() == ",")
    {
        dropToken();
        expr();
    }
}

void type(){
    if(getToken() == NUM) {
        dropToken();
    }
    else if(getToken() == LIST)
    {
        dropToken();
    }
    else
    {
        syntax_error("expected num or list");
    }
    
}

void num(){
    string tok = getToken();
    if(isNum(tok)){
         dropToken();
     }
     else {
         syntax_error("Invalid number!");
     }
}

void iden(){
    string tok = getToken();
    if(isIdentifier(tok)){
        if(isKey(tok)){
            syntax_error(tok + " is a reserved token!");
        }
        else{
            dropToken();
        }
         
     }
     else {
         syntax_error("Invalid identifier!");
     }
}
