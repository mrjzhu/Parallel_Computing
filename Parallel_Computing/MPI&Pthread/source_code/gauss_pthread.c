/* Gaussian elimination without pivoting.
 * Compile with "gcc gauss.c" 
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

/* Program Parameters */
#define MAXN 5000  /* Max value of N */
int N;  /* Matrix size */
int norm=0;
char* file_name; /*output File_name*/
int p_number; 

/* Matrices and vectors */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN];
/* A * X = B, solve for X */

/* junk */
#define randm() 4|2[uid]&3

/* Prototype */
void gauss();  
void gauss_pthread();

void print_to_file();
/* returns a seed for srand based on the time */
unsigned int time_seed() {
  struct timeval t;
  struct timezone tzdummy;

  gettimeofday(&t, &tzdummy);
  return (unsigned int)(t.tv_usec);
}

/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
  int seed = 0;  /* Random seed */
  char uid[32]; /*User name */

  /* Read command-line arguments */
  srand(time_seed());  /* Randomize */

  if(argc!=5){
    printf("Usage: %s <matrix_dimension> [random seed] [pthread_number] [file_name]\n",
           argv[0]);    
    exit(0);
  }
  else{
    seed = atoi(argv[2]);
    srand(seed);
    printf("Random seed = %i\n", seed);
  
    N = atoi(argv[1]);
    if (N < 1 || N > MAXN) {
      printf("N = %i is out of range.\n", N);
      exit(0);
    }
  }

  
  file_name=argv[4];
  p_number=atoi(argv[3]);
  /* Print parameters */
  printf("\nMatrix dimension N = %i.\n", N);
}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
  int row, col;

  printf("\nInitializing...\n");
  for (col = 0; col < N; col++) {
    for (row = 0; row < N; row++) {
      A[row][col] = (float)rand() / 32768.0;
    }
    B[col] = (float)rand() / 32768.0;
    X[col] = 0.0;
  }

}

/* Print input matrices */
void print_inputs() {
  int row, col;

  if (N < 10) {
    printf("\nA =\n\t");
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
	printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
      }
    }
    printf("\nB = [");
    for (col = 0; col < N; col++) {
      printf("%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
    }
  }
}

void print_X() {
  int row;

  if (N < 100) {
    printf("\nX = [");
    for (row = 0; row < N; row++) {
      printf("%5.2f%s", X[row], (row < N-1) ? "; " : "]\n");
    }
  }
}



int main(int argc, char **argv) {
  /* Timing variables */
  struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
  struct timezone tzdummy;
  clock_t etstart2, etstop2;  /* Elapsed times using times() */
  unsigned long long usecstart, usecstop;
  struct tms cputstart, cputstop;  /* CPU times for my processes */

  /* Process program parameters */
  parameters(argc, argv);

  /* Initialize A and B */
  initialize_inputs();

  /* Print input matrices */
  print_inputs();

  /* Start Clock */
  printf("\nStarting clock.\n");
  gettimeofday(&etstart, &tzdummy);
  etstart2 = times(&cputstart);

  /* Gaussian Elimination */
  //gauss();

  //pthread
  gauss_pthread();

  /* Stop Clock */
  gettimeofday(&etstop, &tzdummy);
  etstop2 = times(&cputstop);
  printf("Stopped clock.\n");
  usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
  usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

  /* Display output */
  print_X();
  print_to_file();
  /* Display timing results */
  printf("\nElapsed time = %g ms.\n",
	 (float)(usecstop - usecstart)/(float)1000);

  printf("(CPU times are accurate to the nearest %g ms)\n",
	 1.0/(float)CLOCKS_PER_SEC * 1000.0);
  printf("My total CPU time for parent = %g ms.\n",
	 (float)( (cputstop.tms_utime + cputstop.tms_stime) -
		  (cputstart.tms_utime + cputstart.tms_stime) ) /
	 (float)CLOCKS_PER_SEC * 1000);
  printf("My system CPU time for parent = %g ms.\n",
	 (float)(cputstop.tms_stime - cputstart.tms_stime) /
	 (float)CLOCKS_PER_SEC * 1000);
  printf("My total CPU time for child processes = %g ms.\n",
	 (float)( (cputstop.tms_cutime + cputstop.tms_cstime) -
		  (cputstart.tms_cutime + cputstart.tms_cstime) ) /
	 (float)CLOCKS_PER_SEC * 1000);
      /* Contrary to the man pages, this appears not to include the parent */
  printf("--------------------------------------------\n");
  

  exit(0);
}

/* Provided global variables are MAXN, N, A[][], B[], and X[],
 * defined in the beginning of this code.  X[] is initialized to zeros.
 */
void gauss() {
  int row, col;  /* Normalization row, and zeroing
			* element row and col */
  float multiplier;

  printf("Computing Serially.\n");
  
  /* Gaussian elimination */
  for (norm = 0; norm < N - 1; norm++) {

    for (row = norm + 1; row < N; row++) {
      multiplier = A[row][norm] / A[norm][norm];
      for (col = norm; col < N; col++) {
	A[row][col] -= A[norm][col] * multiplier;
      }
      B[row] -= B[norm] * multiplier;
    }
  }

  /* (Diagonal elements are not normalized to 1.  This is treated in back
   * substitution.)
   */
  /* Back substitution */
  for (row = N - 1; row >= 0; row--) {
    X[row] = B[row];
    for (col = N-1; col > row; col--) {
      X[row] -= A[row][col] * X[col];
    }
    X[row] /= A[row][row];
  }

}

/*------------------------------------------------------------------------------*/

void print_to_file(){
    FILE *file;
    if((file=fopen(file_name,"wt"))==NULL)
    {
        perror("fopen");
        exit(1);
    }
    
    
    int row;
    fprintf(file,"\nX = [");
    for (row = 0; row < N; row++) {
      fprintf(file,"%5.2f%s", X[row], (row < N-1) ? "; " : "]\n");
    }
        fclose(file);

  
}
void *exc(void* arg){
      float multiplier;
      int *p=(int *)arg;
      int k=*p;                                   /* The tag of threads*/
      int T=(N-1-norm)/p_number;                  /* The number of rows each thread allocated,  for each norm*/  
      int row,col;
      for(row=norm+k*T+1;row<=norm+(k+1)*T;row++){ /*each thread compute their allocated rows*/
          multiplier = A[row][norm] / A[norm][norm]; 
          for(col = norm; col < N; col++) {
            A[row][col] -= A[norm][col] * multiplier;
        }
        B[row] -= B[norm] * multiplier;
      }
      free(p);
      pthread_exit(NULL);            
        
    
}

void gauss_pthread() {


  int row, col;  /* Normalization row, and zeroing
                  * element row and col */
  int i,l;
  /* Gaussian elimination */
    for (norm = 0; norm < N - 1; norm++)
    {
      int less=N-1-norm;
      if(less<p_number){    /* If the left rows less than the number of processors, I need machine to computing serially*/
        for(l=0;l<less;l++){
          int row=N-1-l;
          float multiplier;
          multiplier = A[row][norm] / A[norm][norm]; 
          for (col = norm; col < N; col++){
            A[row][col] -= A[norm][col] * multiplier;
          }
          B[row] -= B[norm] * multiplier;
        }
      }
      else
      {
         /* If not, do Gaussian Elimination for each thread*/
        
        pthread_t tids[p_number]; /*definition of thread*/

        for(i=0;i<p_number;i++){
            int *arg = (int *)malloc(sizeof(int));
            *arg = i;
            pthread_create(&tids[i],NULL,exc,arg); /*Create threads to to execute the function: exc*/
        }
        for(i=0;i<p_number;i++){
            pthread_join(tids[i],NULL);  /* Join each threads after computing*/
        }

      /* compute some left rows after allocated to ranks in one loop */      
        int left= (N-1-norm)%p_number;
        if(left!=0){
          for(l=0;l<left;l++)
          {
            int row=N-1-l;
            float multiplier;
            multiplier = A[row][norm] / A[norm][norm]; 
            for (col = norm; col < N; col++){
              A[row][col] -= A[norm][col] * multiplier;
            }
            B[row] -= B[norm] * multiplier;
          }
        }
      }  
    }

    printf("after gaussian elimination::\n");
    print_inputs();

  /* Back substitution */
  for (row = N - 1; row >= 0; row--) {
    X[row] = B[row];
    for (col = N-1; col > row; col--) {
      X[row] -= A[row][col] * X[col];
    }
    X[row] /= A[row][row];
  }

}