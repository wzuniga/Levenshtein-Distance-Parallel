#ifndef __ENTRY
#define __ENTRY

#include <string>
#include <iostream>

using namespace std;

class Entry{
    private:
        int ID;
        string TITLE;
        string DATA;

    public:
        Entry(int id, string title, string data): ID(id), TITLE(title), DATA(data){};
        Entry(int id, string title): ID(id), TITLE(title) {};

        int get_id(){return this->ID;}
        string get_title(){return this->TITLE;}
        string get_data(){return this->DATA;}

        string toString() {
            return "Entry(" + to_string(ID) + ", " +  TITLE + ")";
        }
};
#endif
