// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char ch;
fstream fin("sample.txt", fstream::in);

char getChar(){
    if(fin >> noskipws >> ch){
        return ch ;
    }
    return 0;
}

string next

int main () {
    
    return 0;
}

