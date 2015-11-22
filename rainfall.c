//
//  rainfall.c
//  
//
//  Created by Claire Yuan on 11/16/15.
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS 2

pthread_barrier_t  barrier; // barrier synchronization object

typedef struct{
    int ** elevation;
    double ** absorption;
    double ** raindrops;
    double ** trickle;
    int N;
    int complete_step;
    int drained;
} landscape_t;
typedef landscape_t*  Landscape;


int M;
int N;
double A;

landscape_t * landscape;


int** read_elevation(char *filename){
    
    int ** elevation;
    int i, j;
    
    
    FILE *fptr;
    
    fptr=fopen(filename, "r");
    assert(fptr!=NULL);
    
    /* allocate memory for each row pointer*/
    elevation = (int **)malloc(N*sizeof(int*));
    assert(elevation!=NULL);

    /* allocate memory for each row */
    for(i=0; i< N; i++) {
        elevation[i] = (int*)malloc(N*sizeof(int));
        assert(elevation[i]!=NULL);
        
    }
    
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            fscanf(fptr, "%d", &elevation[i][j]);
        }
    }
    
    fclose(fptr);
    
    return elevation;
    
}

void Landscape_Init(char * filename){
    
    int i,j;
    
    landscape=(landscape_t*) malloc(sizeof(landscape_t));
    assert(landscape!=NULL);
    
    landscape->N=N;
    landscape->elevation=read_elevation(filename);
    
    // allocate memory for absorption matrix
    double **absorption=(double**) malloc(N*sizeof(double*));
    assert(absorption!=NULL);
    
    for(i=0;i<N;i++){
        absorption[i]=(double*) malloc(N*sizeof(double));
        assert(absorption[i]!=NULL);
        for (j=0; j<N; j++) {
            absorption[i][j]=0;
        }
    }
    
    landscape->absorption=absorption;
    
    // allocate memory for raindrops matrix
    double **raindrops=(double**) malloc (N* sizeof(double*));
    assert(raindrops!=NULL);
    
    for (i=0; i<N; i++) {
        raindrops[i]=(double*) malloc(N*sizeof(double));
        for (j=0; j<N; j++) {
            raindrops[i][j]=0;
        }
        assert(raindrops[i]!=NULL);
    }
    
    landscape->raindrops=raindrops;
    
    // allocate memory for tricle matrix
    
    double **trickle = (double**) malloc(N*sizeof(double*));
    assert(trickle!=NULL);
    
    for (i=0; i<N; i++) {
        trickle[i]=(double*) malloc(N*sizeof(double));
        for (j=0; j<N; j++) {
            trickle[i][j]=0;
        }
        assert(trickle[i]!=NULL);
    }
    
    landscape->trickle=trickle;
    
    
}

void Landscape_Destroy(){
    
    free(landscape->elevation);
    landscape->elevation=NULL;
    free(landscape->absorption);
    landscape->absorption=NULL;
    free(landscape);
    landscape=NULL;
}

int min(int a, int b){
    return a<b? a:b;
}
int max(int a, int b){
    return a>b? a:b;
}


void Trickle(int i, int j){
    
    //trickle out from current point
    landscape->raindrops[i][j]-=landscape->trickle[i][j];
    
    //trickle to lowest neigbhors
    
    int m=INT_MAX;
    
    if (i>0) {
        m=min(m, landscape->elevation[i-1][j]);
    }
    if (i<N-1) {
        m=min(m, landscape->elevation[i+1][j]);
    }
    if (j>0) {
        m=min(m, landscape->elevation[i][j-1]);
    }
    if (j<N-1) {
        m=min(m,landscape->elevation[i][j+1]);
    }
    
    if(m<landscape->elevation[i][j]){
        
        
        int s=0;
        

        if (i>0 && m==landscape->elevation[i-1][j]) {
            s++;
        }
        if(i<N-1 && m==landscape->elevation[i+1][j]){
            s++;
        }
        if (j>0 && m==landscape->elevation[i][j-1]) {
            s++;
        }
        if (j<N-1 && m==landscape->elevation[i][j+1]) {
            s++;
        }
        
        double portion=landscape->trickle[i][j]/s;
        
        
        if (i>0 && m==landscape->elevation[i-1][j]) {
            landscape->raindrops[i-1][j]+=portion;
        }
        if(i<N-1 && m==landscape->elevation[i+1][j]){
            landscape->raindrops[i+1][j]+=portion;
        }
        if (j>0 && m==landscape->elevation[i][j-1]) {
            landscape->raindrops[i][j-1]+=portion;
        }
        if (j<N-1 && m==landscape->elevation[i][j+1]) {
            landscape->raindrops[i][j+1]+=portion;
        }
    }
    else{
        landscape->raindrops[i][j]+=landscape->trickle[i][j];
    }
}

void *Absorb(void * arg){
    
    int index= *((int *) arg);  
       printf("%d", index);
    //compute Bound for this thread
    int startrow = index * N/NUM_THREADS;  
    int endrow = (index+1) * (N/NUM_THREADS) - 1;  

    int i, j;

    //Traversal over blocks dealed in this thread

    for (i=startrow;i<=endrow; i++) {
        for (j=0; j<N; j++) {
            if (landscape->complete_step<M) {
                //1) Receive a new raindrop
                landscape->raindrops[i][j]+=1;
            }
            if (landscape->raindrops[i][j] > A) {
                landscape->absorption[i][j]+=A;
                landscape->raindrops[i][j]-=A;
                //printf("drainning");
            }
            else {
                landscape->absorption[i][j]+=landscape->raindrops[i][j];
                landscape->raindrops[i][j]=0;
                landscape->drained++;
		
            }

            //3a) Calculate the amount of raindrops that will next trickle to the lowest neighbor
            if(landscape->raindrops[i][j]>1){
                landscape->trickle[i][j]=1;
            }
            else{
               landscape->trickle[i][j]=landscape->raindrops[i][j];
            }
        }
    }
    

    pthread_barrier_wait (&barrier);
    
    for (i=startrow; i<=endrow; i++) {
        for (j=0; j<N; j++) {
            Trickle(i ,j);
        }
    }
    
    pthread_exit(NULL);

    return NULL;
}

void Rainfall(char *filename){
    
    Landscape_Init(filename);
    
    landscape->complete_step=0;
    landscape->drained=0;

    
    pthread_t *threads = (pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
    int *p;
    int i,j;
    
    pthread_barrier_init (&barrier, NULL, NUM_THREADS);
    
    while(landscape->drained!=N*N){
      // create a barrier object with a count of 3
     
      for ( i = 0; i < NUM_THREADS; i++ ){
          p = (int *) malloc(sizeof(int));
          *p = i;
          pthread_create(&threads[i], NULL, Absorb, (void *)(p));
      }
      
      for (i = 0; i < NUM_THREADS; ++i) {
          pthread_join(threads[i], NULL);
      }
     
        
    }    
}





int main(int argc, char** argv)
{
    
    if (argc != 5) {
        printf("Usage: main  <M> <A> <N> <elevation_file>\n");
        assert(argc == 5);
    }
    
    M=atoi(argv[1]); // # of simulation time steps
    
    sscanf(argv[2],"%lf",&A); // absorption rate
    
    N=atoi(argv[3]); // dimension of the landscape
    
    
    clock_t start=clock();
    
    Rainfall(argv[4]);
    
    clock_t end = clock();
    
    
    printf("Time consumed %f\n\n", ((double) (end - start)) / CLOCKS_PER_SEC);
    
    printf("Rainfall simulation took %d time steps to complete.\n", landscape->complete_step);
    
    printf("The following grid shows the number of raindrops absorbed at each point:\n");
    
    
    // Printing Output
    int i, j;
    
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            printf("%8g ", landscape->absorption[i][j]);
        }
        printf("\n");
    }
    
    printf("%d\n",landscape->drained);

    printf("printing raindrops\n");
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            printf("%.2f ", landscape->raindrops[i][j]);
        }
        printf("\n");
    }
    printf("printing trickle\n");
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            printf("%.2f ", landscape->trickle[i][j]);
        }
        printf("\n");
    }
    
    
    Landscape_Destroy();


    return 0;
}

