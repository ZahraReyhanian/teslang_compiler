#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char c;
ifstream file;

char getChar(){
    if(file.is_open())
        if (!file.eof()) //eof check mikone ke be akhare file residim ya na
        {
            file >> c ;
            return c;
        }
    
    return '\0';
    
}

void ungetchar(){
    file.seekg(-1,ios::cur);
}

