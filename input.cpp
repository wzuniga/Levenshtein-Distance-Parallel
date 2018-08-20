#include "MODEL/Entry.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <cassert>
#include <omp.h>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <regex>

using namespace std;
/*
 * 0 = data
 * 1 = header
 * 2 = end
 */
int lineType(string line){
    return (line[0] == '<' ? (line[1] == 'd' ? 1 : 2 ): 0);
}

void getHeaderData(string line, int* id, string* title){  
    std::regex ws_re("\"+");
    std::vector<std::string> result{
        std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1), 
        std::sregex_token_iterator()
    };
    *id = atoi(result[1].c_str());
    *title = result[3];
}

int loadData(vector<Entry*>* dictionary)
{
    string line, dataBuffer;
    ifstream file ("DATA/spanishText_15000_20000");
    if (file.is_open()){
        int id_temp;
        string title_temp;
        while ( getline (file,line) ){
            if(lineType(line) == 1){
                getHeaderData(line, &id_temp, &title_temp);
                Entry *temp = new Entry(id_temp, title_temp);
                dictionary->push_back(temp);
            }
        }
        file.close();
    }
}

int main(int argc, char* argv[])
{
    vector<Entry*> dictionary;
    loadData(&dictionary);
    for(int i=0; i < dictionary.size(); i++)
        cout << dictionary[i]->toString() << endl;
    cout << dictionary.size() << endl;
}