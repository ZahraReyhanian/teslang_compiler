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

    expr -> assign-expr
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
                iden ( clist ) |
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
void semicol();
void if_stmt();
void while_stmt();
void for_stmt();
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
    /*while (! isFileEnd())
    {
        cout << getToken() << '\n';
        dropToken();
    }*/
    //type();
    //iden();
    prog();
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
    if(ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == ',' || ch == '{' || ch == '}' ||
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
    if (ch == '\n')
    {
        count_line ++;
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
    count_line++;
    getline(file, temp);
}

char skipSpace(char ch){
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\r'  || ch == '\t')){
        if(ch == '\n') count_line ++;
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
    cout << count_line << ": " << err << "but get : "<< getToken() <<endl;
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
    while (true)
    {
        if(isFileEnd()) break;
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
                dropToken();
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
    string tok = getToken();
    if(tok == NUM || tok == LIST)
    {
        defvar();
        semicol();
    }
    else if(tok == IF){
        if_stmt();
    }
    else if(tok == WHILE)
    {
        while_stmt();
    }
    else if(tok == FOR)
    {
        for_stmt();
    }
    else if(tok == RETURN){
        dropToken();
        expr();
        semicol();
    }
    else if(tok == "{")
    {
        dropToken();
        body();
        if(getToken() == "}"){
            dropToken();
            return;
        }
        else
        {
            syntax_error("expected }");
        }
        
    }
    else
    {
        expr();
        semicol();
    }
    
}

void if_stmt(){
    if (getToken() == IF)
    {
        dropToken();
        if(getToken() == "("){
            dropToken();
            expr();
            if (getToken() == ")")
            {
                dropToken();
                stmt();
                if(getToken() == ELSE)
                {
                    dropToken();
                    stmt();
                }
                return;
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
    
}

void while_stmt(){
    if(getToken() == WHILE){
        dropToken();
        if(getToken() == "("){
            dropToken();
            expr();
            if (getToken() == ")")
            {
                dropToken();
                stmt();
                return;
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
}

void for_stmt(){
     if(getToken() == FOR)
     {
         dropToken();
         if(getToken() == "("){
            dropToken();
            iden();
            if(getToken() == IN){
                 dropToken();
                 expr();
                 if(getToken() == ")"){
                     dropToken();
                     stmt();
                 }
                 else
                 {
                     syntax_error("expected )");
                 }
                 
            }
            else
            {
                syntax_error("expected in");
            }
         }
         else
         {
             syntax_error("expected (");
         }
         
     }
}

void semicol(){
    if(getToken() == ";"){
        dropToken();
    }
    else{
        syntax_error("expected ;");
        if(isOpChar(getToken()[0]))
        {
            dropToken();
        }
    }
}

void defvar(){
    type();
    if (isKey(getToken()))
    {
        syntax_error(getToken() + "is a key word");
        dropToken();
        return;
    }
    iden();
}
//expr -> assign-expr
void expr(){
    assign_expr();
}

//assign-expr -> cond-expr |
     //               unary-expr  = assign-expr
void assign_expr(){
    unary_expr();
    if (getToken() == "=")
    {
        while (getToken() == "=")
        {
            dropToken();
            cond_expr();
        }
    }
    else
    {
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
    //cout << "gggggggggggggg " << getToken();
    prim_expr();
    //cout << "hhhhhhhhhhhhhhhhhh";
    while (getToken() == "[")
    {
        dropToken();
        expr();
        if(getToken() == "]"){
            dropToken();
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
        //cout << getToken() << " 11111111111\n";
        iden();
        //cout << getToken() << " 22222222222\n";
        if(getToken() == "("){
            dropToken();
            clist();
            //cout << getToken() << " kkkkkkkkkkkkkkkkkkk\n";
            if (getToken() == ")")
            {
                dropToken();
            }
            else
            {
                syntax_error("expected )");
            }
        
        }
        
    }
    else if(isNum(getToken()))
    {
        num();
        return;
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
    else if(getToken() == ")" || getToken() == "]" || getToken() == ";")
    {
        return;
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
    if (getToken() == ")")
    {
        return;
    }
    
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
    if (getToken() == ")")
    {
        return;
    }
    //cout << getToken() << " eeeeeeeeeeeeeeeeeeeeee\n";
    expr();
    //cout << getToken() << " after\n";
    while(! isFileEnd() && getToken() != ")" && getToken() == "," )
    {
        dropToken();
        expr();
    }
    //cout << getToken() << " 3333333333333333\n";
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
        //cout << getToken() << " nnnnnnnnnnnnnnnnnnnnnn\n";
         dropToken();
     }
     else {
         syntax_error("Invalid number!");
     }
}

void iden(){
    string tok = getToken();
    if(isIdentifier(tok)){
        //if(isKey(tok)){
           // syntax_error(tok + " is a reserved token!");
       // }
        //else{
            dropToken();
       // }
         
     }
     else {
         syntax_error("Invalid identifier!");
     }
}
