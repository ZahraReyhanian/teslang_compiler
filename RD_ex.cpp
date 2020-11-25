/*
startgrammer -> word ':=' anything '^;' startgrammer
startgrammer -> ~
anything -> anything '|' anything1
anything -> anything1
anything1 -> anyword | ~
anyword -> word anything
anyword -> number anything
anyword -> signs anything
signs -> [^\w]*
word -> [a-zA-Z_][a-zA-Z_0-9]*
number -> [0-9]+
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
void startgrammer();
void anything();
void anything1();
void anyword();
bool isWord(string tok);
bool isNumber(string tok);
bool isSigns(string tok);
void word();
void number();
void signs();

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
    if(ch == '[' || ch == ']' || ch == '(' || ch == ')' ||
                 ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || 
                 ch == '<' || ch == '>' || ch == '!' || ch == '|' || ch == '&' || ch == ';')
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
    while (!file.eof() && (ch == ' ' || ch == '\n' || ch == '\r' )){
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
        if (answer == " " || answer == "\n" || answer == "\r"){
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

void syntax_error(string err){
    cout << err << endl;
    num_err ++;
}

void startgrammer(){
    while(getToken() != "\0" && isWord(getToken())){
        word();
        if(nextToken() != ":=")
            syntax_error("expected :=");
        dropToken();
        anything();
        if(nextToken() != "^;")
            syntax_error("expected ^;");
        dropToken();
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

void anything(){
    anything1();
    while(getToken() == "|"){
        dropToken();
        anything1();           
    }
}

void anything1(){
    anyword();
}

void anyword(){
    if(isWord(getToken())){
        word();
        anything();
    }
    else if(isNumber(getToken()))
    {
        number();
        anything();
    }
    else if(isSigns(getToken())) {
        signs();
        anything();  
    }
    else {
        syntax_error("expected word or number or signs");
    }
}

bool isSigns(string tok){
    if(regex_match(tok, regex("[^a-zA-Z0-9]*"))) return true;
    else return false;  
}

bool isWord(string tok){
    if(regex_match(tok, regex("[a-zA-Z_][a-zA-Z_0-9]*"))) return true;
    else return false;
}

bool isNumber(string tok){
    if(regex_match(tok, regex("[0-9]+"))) return true;
    else return false;
}

void signs(){
    string tok = getToken();
     if(isSigns(tok)){
         dropToken();
     }
     else {
         syntax_error("Invalid signs!");
     }
}

void word(){
    string tok = getToken();
    if(isWord(tok)){
         dropToken();
     }
     else {
         syntax_error("Invalid word!");
     }
}

void number(){
    string tok = getToken();
    if(isNumber(tok)){
         dropToken();
     }
     else {
         syntax_error("Invalid number!");
     }
}

