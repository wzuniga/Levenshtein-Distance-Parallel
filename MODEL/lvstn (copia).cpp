#include "input.cpp"
#include <iostream>
#include <cstring>
#include <string>
#include <cassert>
#include <omp.h>
#include <fstream>
#include <vector>
#include <climits>

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

#   pragma omp parallel for default(none) private(lastdiag, olddiag, y, x) shared(s1, s2, s2len, column, s1len) num_threads(thread_num) schedule(static)
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

int kmp(string T, string P) {
    vector<int> pi(P.size(), 0);
    for (int i = 1, k = 0; i < P.size(); ++i) {
        while (k && P[k] != P[i]) 
            k = pi[k - 1];
        if (P[k] == P[i]) 
            ++k;
        pi[i] = k;
    }

    for (int i = 0, k = 0; i < T.size(); ++i) {
        while (k && P[k] != T[i]) 
            k = pi[k - 1];
        if (P[k] == T[i]) 
            ++k;
        if (k == P.size()) 
            return i - k + 1;
    }
    return -1;
}

int findNerarWord(vector<Entry*>* dictionary, string find_word ){
    int i, pos, pos_ans = -1;
    double start_total, end_total, start_local, end_local;
    size_t serial, paral;
    
    int min[thread_num][2];

    for(int i=0; i < thread_num; i++){
        min[i][0] = INT_MAX;
    }

    start_total = omp_get_wtime();
#   pragma omp parallel for default(none) private(pos,i,serial,paral,start_local,end_local) shared(pos_ans,cout, min, dictionary, find_word) num_threads(thread_num) schedule(static)
    for(i = 0; i<dictionary->size(); i++){
//        #pragma omp parallel sections
//        {
//            #pragma omp section
//            {
//                for(i = 0; i<dictionary->size(); i++){
                paral = levenshtein_distance_parallel(find_word, find_word.length(), (*dictionary)[i]->get_title(), (*dictionary)[i]->get_title().length());   
                if (min[omp_get_thread_num()][0] > paral){
                    min[omp_get_thread_num()][0] = paral;
                    min[omp_get_thread_num()][1] = i;
                }
//                }
//            }
//            #pragma omp section
//            {
//                for(i = 0; i<dictionary->size(); i++){
                pos = kmp(find_word, (*dictionary)[i]->get_title());
                pos_ans = pos != -1 ? i : pos_ans;
//                }
//            }
//        }
    }
    end_total = omp_get_wtime();


    int min_i, min_v = INT_MAX;
    //cout << "----" << endl;
    for(int i=0; i < thread_num; i++){
        //cout << min[i][1] << " " << min[i][0] << endl;
        if(min_v > min[i][0]){
            min_v = min[i][0];
            min_i = min[i][1];
        }       
    }

    printf("Tiempo:   %.10f  %i\n", end_total - start_total, thread_num);
    printf("Cercania: %s %i\n", (*dictionary)[min_i]->get_title().c_str(), min_i);
    if(pos_ans != -1)
        printf("Exactitud: %s %i\n", (*dictionary)[pos_ans]->get_title().c_str(), pos_ans);
    printf("\n");
    return min_i;
}

int main(int argc, char* argv[])
{
    // = "Provincia de Bus";
    vector<Entry*> dictionary;

    vector<string> dataset {
        "DATA/spanishText_10000_15000", 
        "DATA/spanishText_15000_20000",
        "DATA/spanishText_20000_25000",
        "DATA/spanishText_25000_30000",
        "DATA/spanishText_40000_45000",
        "DATA/spanishText_45000_50000",
        "DATA/spanishText_70000_75000",
        "DATA/spanishText_90000_95000",
        "DATA/spanishText_110000_115000",
        "DATA/spanishText_120000_125000",
        "DATA/spanishText_180000_185000",
        "DATA/spanishText_185000_190000",

        "DATA/spanishText_200000_205000",
        "DATA/spanishText_205000_210000",
        "DATA/spanishText_210000_225000",
        "DATA/spanishText_225000_230000",
        "DATA/spanishText_230000_235000",
        "DATA/spanishText_260000_265000",
        "DATA/spanishText_265000_270000",
        "DATA/spanishText_270000_275000",
        "DATA/spanishText_285000_290000",
        "DATA/spanishText_305000_310000",
        "DATA/spanishText_310000_315000",
        "DATA/spanishText_315000_320000",
        "DATA/spanishText_320000_325000",
        "DATA/spanishText_325000_330000",
        "DATA/spanishText_330000_335000",
        "DATA/spanishText_335000_340000",
        "DATA/spanishText_340000_345000",
    };

    for(int i = 0; i < dataset.size(); i++)
        loadData(&dictionary, dataset[i]);

    printf("Loaded: %i words\n", dictionary.size());

    while(true){
        string word;
        //cin >> word;
        getline(cin, word);
        int min_i = findNerarWord(&dictionary, word);
        //printf("Busqueda: %s %i\n", dictionary[min_i]->get_title().c_str(), min_i);
    }
    
   return 0;
}

//paralelizar la busqueda de a palabra con la busqueda entre todos