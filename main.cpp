#include <iostream>
#include <cstring>
#include <string>
#include <cassert>
#include <omp.h>
#include <fstream>
#include <vector>

using namespace std;

#define thread_num 4

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

size_t levenshtein_distance(string s1, size_t n, string s2, size_t m)
{
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = s1.length();
    s2len = s2.length();
    unsigned int column[s1len+1];
	int counter = 0;
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
			counter++;
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

size_t levenshtein_distance_parallel(string s1, size_t n, string s2, size_t m)
{
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = s1.length();
    s2len = s2.length();
    unsigned int column[s1len+1];

    for (y = 1; y <= s1len; y++)
        column[y] = y;

#   pragma omp parallel for default(none) private(lastdiag, olddiag, y, x) shared(s1, s2, s2len, column, s1len) num_threads(thread_num) schedule(guided)
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return column[s1len];
}

vector<vector<size_t>> compare2words(){

}

int main(int argc, char* argv[])
{
   if (argc > 2)
   {
        string str1 = argv[1];
        string str2 = argv[2];
        double start_s, stop_s, start_p, stop_p;
        cout << str1 << endl;
        cout << str2 << endl;
        start_s = omp_get_wtime();
        size_t ld_s = levenshtein_distance(str1, str1.length(), str2, str2.length());
        stop_s = omp_get_wtime();
        start_p = omp_get_wtime();
        size_t ld_p = levenshtein_distance_parallel(str1, str1.length(), str2, str2.length());
        stop_p = omp_get_wtime();

        cout << "Dinstancia Levenshtein " << str1 << " and " << str2 << ": " << ld_s << " " << ld_p << endl;
        printf("Serial:   %.10f\n", stop_s - start_s);
        printf("Parallel: %.10f\n", stop_p - start_p);
        //assert(ld_s == ld_p);
   }
   else
   {
        vector<string> dictionary;
        string line;
        ifstream file ("input2.txt");
        if (file.is_open()){
            while ( getline (file,line) )
                dictionary.push_back(line);
            file.close();
        }
        //string find_word = "EGEO";

        //vector<Entry*> dictionary;
        //loadData(&dictionary);
        //for(int i=0; i < dictionary.size(); i++)
        //    cout << dictionary[i]->toString() << endl;
        //cout << dictionary.size() << endl;

        //vector<vector<size_t>> nearest(dictionary.size(), vector<size_t>(dictionary.size()));
        ////vector<vector<size_t>> nearest(dictionary.size(), vector<size_t>(2));

        int i;
        double start_total, end_total, start_local, end_local;
        size_t dist;
        start_total = omp_get_wtime();
//#       pragma omp parallel for default(none) private(i,dist,start_local,end_local) shared(dictionary, nearest) num_threads(thread_num) schedule(guided)
#       pragma omp parallel for default(none) private(i,dist,start_local,end_local) shared(dictionary) num_threads(thread_num) schedule(guided)
        for(i = 0; i<dictionary.size(); i++){
            for(int e=0; e<i; e++){
                /*dist = levenshtein_distance_parallel(dictionary[i], dictionary[i].length(), dictionary[e], dictionary[e].length());
                if(dist < nearest[i][0]){
                    nearest[i][0] = dist;
                    nearest[i][1] = dist;
                }
                nearest[i][e] = */
                start_local = omp_get_wtime();
                levenshtein_distance_parallel(dictionary[i], dictionary[i].length(), dictionary[e], dictionary[e].length());
                end_local = omp_get_wtime();
                printf("%i %i => %.10f %i\n", i, e, end_local - start_local, thread_num);
                start_local = omp_get_wtime();
                levenshtein_distance(dictionary[i], dictionary[i].length(), dictionary[e], dictionary[e].length());
                end_local = omp_get_wtime();
                printf("%i %i => %.10f %i\n", i, e, end_local - start_local, thread_num);
            }
        }
        end_total = omp_get_wtime();
        
        /*
        for(int i=0; i<nearest.size(); i++){
            for(int e=0; e<i; e++)
                cout << nearest[i][e] << " ";
            cout << endl;
        }
        */

        printf("Total:   %.10f  %i\n", end_total - start_total, thread_num);
                //if (i != e)
                    //nearest[i][0] = MIN(nearest[i][1], levenshtein_distance(dictionary[i], dictionary[i].length(), dictionary[e], dictionary[e].length()), i, e);
            

   }

   return 0;
}

