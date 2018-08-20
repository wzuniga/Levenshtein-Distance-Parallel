#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
int serial_levenshtein(char *s1, char *s2);
int levenshtein2(char *s1, char *s2);
int levenshtein3(char *s1, char *s2);
int parallel_levenshtein_1(char *s1, char *s2);
int levenshtein5(char *s1, char *s2);
int parallel_levenshtein_2(char *s1, char *s2);
void allocate2D(unsigned int** matrix, int rows, int cols);
void deallocate2D(unsigned int** arr2D, int rows);

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))

void main(int argc, char **argv){
	FILE *in = fopen("input.txt", "r");
	int len = atoi(argv[1]);
	//char s1[len];
	//char s2[len];
	char s1[] = "k";
	char s2[] = "rert";
	//fscanf(in, "%s %s", s1, s2);
	//s1 = {"k"};
	//s2 = {"rert"};
	clock_t start_s, stop_s, start_p, stop_p;
	double t_s = 0.0, t_p = 0.0;
	start_s = clock();
	int distance_s = levenshtein2(s1, s2);
	stop_s = clock();
	start_p = clock();
	int distance_p = parallel_levenshtein_1(s1, s2);
	stop_p = clock();
	t_s = (double)(stop_s - start_s)/CLOCKS_PER_SEC;
	t_p = (double)(stop_p - start_p)/CLOCKS_PER_SEC;
	printf("Edit Distance Serial is (%d),Takes %f time\n",distance_s, t_s);
	printf("Edit Distance Parallel is (%d),Takes %f time\n",distance_p, t_p);
} 

int serial_levenshtein(char *s1, char *s2) {
    unsigned int i, x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);

	unsigned int **matrix = malloc((s2len + 1) * sizeof(unsigned int *));
	for(i = 0; i < s2len + 1; i++)
		matrix[i] = malloc((s1len + 1) * sizeof(unsigned int));

    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));
 
    unsigned int return_val = matrix[s2len][s1len];
	deallocate2D(matrix, s2len + 1);
	return return_val;
}

int levenshtein2(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
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
	//printf("Counter:%d ", counter);  
    return(column[s1len]);
}

//Fills array in diagonally (slower than traditional implementations of levenshtein due to cache considerations)
int levenshtein3(char *s1, char *s2) {
	int x, y, k, i, s1len, s2len;
	s1len = strlen(s1);
	s2len = strlen(s2);

    unsigned int **matrix = malloc((s2len + 1) * sizeof(unsigned int *));
    for(i = 0; i < s2len + 1; i++)
        matrix[i] = malloc((s1len + 1) * sizeof(unsigned int));

	matrix[0][0] = 0;
	int counter = 0;
	for (x = 1; x <= s2len; x++)
		matrix[x][0] = matrix[x-1][0] + 1;
	for(y = 1; y <= s1len; y++)
		matrix[0][y] = matrix[0][y - 1] + 1;
	for (k = 2; k <= s1len + s2len; k++){
		int istart = MAX2(1, k - s2len);
		int iend   = MIN2(k - 1, s1len);
		for (i = istart; i <= iend; i++){
			int j = k - i;
			counter++;
			if(i < 0 || i >s2len || j < 0 || j > s1len)
				printf("Out of bounds (i, j): (%d, %d) \n", i, j);
			matrix[i][j] = MIN3(matrix[i-1][j] + 1, matrix[i][j-1] + 1, matrix[i-1][j-1] + (s1[j-1] == s2[i-1] ? 0 : 1));
			}
	}
	int return_val = matrix[s2len][s1len];
	deallocate2D(matrix, s2len);
	printf("Counter:%d ", counter); 
	return return_val;
}


int parallel_levenshtein_1(char *s1, char *s2) {
    int x, y, k, s1len, s2len;

    s1len = strlen(s1);
    s2len = strlen(s2);

    unsigned int **matrix = malloc((s2len + 1) * sizeof(unsigned int *));
    for(int i = 0; i < s2len + 1; i++)
        matrix[i] = malloc((s1len + 1) * sizeof(unsigned int));

    matrix[0][0] = 0;
    int counter = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for(y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y - 1] + 1;
    for (k = 2; k <= s1len + s2len; k++){
        int istart = MAX2(1, k - s2len);
        int iend   = MIN2(k - 1, s1len);
		#pragma omp parallel for firstprivate(iend, istart) shared(matrix) num_threads(2) 
        for (int i = istart; i <= iend; i++){
            int j = k - i;
            matrix[i][j] = MIN3(matrix[i-1][j] + 1, matrix[i][j-1] + 1, matrix[i-1][j-1] + (s1[j-1] == s2[i-1] ? 0 : 1));
        }
    }
    int return_val = matrix[s2len][s1len];
    deallocate2D(matrix, s2len);
    return return_val;
}

int levenshtein5(char *s1, char *s2) {
    int x, y, k, s1len, s2len, prev_size, prev2_size, current_size, prev_start, prev_end, prev2_start, prev2_end, current_start, current_end;
	unsigned int *prev_p, *prev2_p, *current_p, *temp;
    s1len = strlen(s1);
    s2len = strlen(s2);

    unsigned int prev2[MAX2(s1len, s2len) + 1];
	prev2[0] = 0;
	prev2_size = 1;
	prev2_start = 0;
	prev2_end = 0;

	unsigned int prev[MAX2(s1len, s2len) + 1];
	prev[0] = 1;
	prev[1] = 1;
	prev_size = 2;
	prev_start = 0;
	prev_end = 1;
	unsigned int current[MAX2(s1len, s2len) + 1];
	
	//Assign dummy pointers for fast swapping
	prev_p = prev;
	prev2_p = prev2;
	current_p = current;
	

	
    for (k = 2; k <= s1len + s2len; k++){
        int istart = MAX2(1, k - s2len);
        int iend   = MIN2(k - 1, s1len);
		//Fill in Diagonal
		//printf("\n Diagonal k: %d, \n", k);
		if(k <= MIN2(s1len, s2len)){
			current_start = 0;
			current_end = k;
			current_size = iend - istart + 3;
			current_p[k] = k;
			current_p[0] = k;
		}		
		else{
			current_start = istart;
			current_size = iend - istart + 1;
			current_end = iend;
		}

        #pragma omp parallel for firstprivate(iend, istart, s1, s2) shared(current, prev_p, prev2_p) num_threads(4)
        for (int i = istart; i <= iend; i++){
            int j = k - i;
			current_p[i] = MIN3(prev_p[i] + 1, prev_p[i - 1] + 1, prev2_p[i - 1] + (s1[j - 1] == s2[i - 1] ? 0 : 1));
        }	
	
		temp = prev2_p;

		prev2_p = prev_p;
		prev2_size = prev_size;
		prev2_start = prev_start;
		prev2_end = prev_end;

		prev_p = current_p;
		prev_size = current_size;
		prev_start = current_start;
		prev_end = current_end;

		current_p = temp;		

    }
    int return_val = prev_p[s2len];
    return return_val;
}

int parallel_levenshtein_2(char *s1, char *s2){
    int x, y, k, s1len, s2len;
    unsigned int *prev_p, *prev2_p, *current_p, *temp;
    s1len = strlen(s1);
    s2len = strlen(s2);

	//Initialize previous2 diagonal
    unsigned int prev2[MAX2(s1len, s2len) + 1];
    prev2[0] = 0;

	//Initialize previous diagona
    unsigned int prev[MAX2(s1len, s2len) + 1];
    prev[0] = 1;
    prev[1] = 1;

    unsigned int current[MAX2(s1len, s2len) + 1];
	unsigned int local_curr[MAX2(s1len, s2len) + 1];

    //Assign dummy pointers for fast swapping
    prev_p = prev;
    prev2_p = prev2;
    current_p = current;

    for (k = 2; k <= s1len + s2len; k++){
        int istart = MAX2(1, k - s2len);
        int iend   = MIN2(k - 1, s1len);
        //Fill in Diagonal with base case (if the diagonal passes through a base case cell
        if(k <= MIN2(s1len, s2len)){
        	current_p[k] = k;
        	current_p[0] = k;
        }
						
       	#pragma omp parallel for firstprivate(iend, istart, s1, s2, prev_p, prev2_p) shared(current_p) num_threads(16)
        for (int i = istart; i <= iend; i++){
			int j = k - i;
            current_p[i] = MIN3(prev_p[i] + 1, prev_p[i - 1] + 1, prev2_p[i - 1] + (s1[j - 1] == s2[i - 1] ? 0 : 1));
		}

	   	temp = prev2_p;

		//Update prev, prev2, and current poitners.
        prev2_p = prev_p;
        prev_p = current_p;
        current_p = temp;
    }
    int return_val = prev_p[s2len];
    return return_val;
	
}


void allocate2D(unsigned int** matrix, int rows, int cols){
	unsigned int i;
		
	matrix = (unsigned int**)malloc(rows*sizeof(unsigned int));
	for(i = 0; i < rows; i++){
		matrix[i] = (unsigned int*)malloc(cols*sizeof(unsigned int));
	}
}

void deallocate2D(unsigned int **arr2D, int rows){
	int i;

	for(i = 0; i < rows; i++)
		free(arr2D[i]);
	free(arr2D);
}

