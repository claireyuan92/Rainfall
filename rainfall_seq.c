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
#include <stdbool.h>


typedef struct{
    int ** elevation;
    double ** absorption;
    double ** raindrops;
    double ** trickle;
    int N;
    int complete_step;
} landscape_t;

typedef landscape_t* Landscape;


int** read_elevation(char *filename, int N){
    
    int ** elevation;
    int i, j;
    
    N=N+2;
    
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
        
        elevation[i][0]=INT_MAX;
        elevation[i][N-1]=INT_MAX;
    }
    for (j=0; j<N; j++) {
        elevation[0][j]=INT_MAX;
        elevation[N-1][j]=INT_MAX;
    }
    
    for (i=1; i<N-1; i++) {
        for (j=1; j<N-1; j++) {
            fscanf(fptr, "%d", &elevation[i][j]);
            //printf("%d ",elevation[i][j]);
        }
        //printf("\n");
    }
    
    /*
    
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {

            printf("%d ",elevation[i][j]);
        }
        printf("\n");
    }
     */

    
    fclose(fptr);
    //printf("Finished reading image file !\n");
    
    return elevation;
    
}

Landscape Landscape_Init(char * filename, int N){
    
    int i,j;
    

    
    landscape_t * landscape;
    landscape=(landscape_t*) malloc(sizeof(landscape_t));
    assert(landscape!=NULL);
    
    landscape->N=N;
    landscape->elevation=read_elevation(filename,N);
    
     N=N+2; // deal with edge problem
    
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
    
    return landscape;
    
}

void Landscape_Destroy(Landscape * landscape){
    
    free((*landscape)->elevation);
    (*landscape)->elevation=NULL;
    free((*landscape)->absorption);
    (*landscape)->absorption=NULL;
    free(*landscape);
    *landscape=NULL;
}

int min(int a, int b){
    return a<b? a:b;
}


bool Absorb(landscape_t * landscape, double A, int N, int k){
    //Traverse over all landscape points
    int i,j;
    bool flag=false;
    
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            
            if (k>0) {
                //1) Receive a new raindrop
                landscape->raindrops[i][j]+=1;
            }

            
            //2) If there are raindrops on a point, absorb water into the point
            if (landscape->raindrops[i][j] > A) {
                landscape->absorption[i][j]+=A;
                landscape->raindrops[i][j]-=A;
                flag=true;
            }
            else if(landscape->raindrops[i][j]<=A && landscape->raindrops[i][j]>10e-6){
                landscape->absorption[i][j]+=landscape->raindrops[i][j];
                landscape->raindrops[i][j]=0;
                flag=true;
            }
            else{}
            
            
            //3a) Calculate the amount of raindrops that will next trickle to the lowest neighbor
            if(landscape->raindrops[i][j]>1){
                landscape->trickle[i][j]=1;
            }
            else{
                landscape->trickle[i][j]=landscape->raindrops[i][j];
            }
            
            
        }
    }
    
    
    //Second traversal over all landscape points
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            
            //trickle out from current point
            landscape->raindrops[i][j]-=landscape->trickle[i][j];
            
            //trickle to lowest neigbhors
            
            int m=min(landscape->elevation[i+1][j],
                      min(landscape->elevation[i][j+1],
                          min(landscape->elevation[i-1][j], landscape->elevation[i][j-1])));
            //printf("%d  ", m);
            
            if(m<landscape->elevation[i][j]){
                
                int s=0;
                int x,y;
                /*
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][y]) {
                            s++;
                        }
                    }
                }
                 */
                
                if (m==landscape->elevation[i][j-1]) {
                    s++;
                }
                if (m==landscape->elevation[i][j+1]) {
                    s++;
                }
                if (m==landscape->elevation[i-1][j]) {
                    s++;
                }
                if (m==landscape->elevation[i+1][j]) {
                    s++;
                }
                
                //printf("%lf ", 1.0/s);
                double portion=landscape->trickle[i][j]/s;
                
                //printf("%lf ", portion);
                /*
                 
                
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][y]) {
                            
                            landscape->raindrops[x][y]+=portion;
                        }
                    }
                }
                */
                if (m==landscape->elevation[i][j-1]) {
                    landscape->raindrops[i][j-1]+=landscape->trickle[i][j]/s;
                }
                if (m==landscape->elevation[i][j+1]) {
                    landscape->raindrops[i][j+1]+=landscape->trickle[i][j]/s;
                }
                if (m==landscape->elevation[i-1][j]) {
                    landscape->raindrops[i-1][j]+=landscape->trickle[i][j]/s;
                }
                if (m==landscape->elevation[i+1][j]) {
                    landscape->raindrops[i+1][j]+=landscape->trickle[i][j]/s;
                }
                
            }
            else{
                landscape->raindrops[i][j]+=landscape->trickle[i][j];
            }
            
            
        }
    }
    
    return flag;
    
}

Landscape Rainfall(char *filename, int M, double A, int N){
    
    landscape_t *landscape = Landscape_Init(filename,N);
    
    int k=M;
    
    landscape->complete_step=0;

    while(Absorb(landscape,A,N,k)) { //continue to absorb until return false;
        landscape->complete_step++;
        k--;
    }
    
    return landscape;
    
    
}





int main(int argc, char** argv)
{
    landscape_t * landscape;
    
    if (argc != 5) {
        printf("Usage: main  <M> <A> <N> <elevation_file>\n");
        assert(argc == 5);
    }
    
    int M=atoi(argv[1]); // # of simulation time steps
    
    double A;
    sscanf(argv[2],"%lf",&A); // absorption rate

    
    int N=atoi(argv[3]); // dimensionof the landscape
    
    clock_t start=clock();
    
    landscape = Rainfall(argv[4], M,A,N);
    
    clock_t end = clock();
    
    printf("Time consumed %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);
    
    printf("Rainfall simulation took %d time steps to complete.\n", landscape->complete_step);
    
    printf("The following grid shows the number of raindrops absorbed at each point:\n");
    
    int i, j;
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            printf("%8g ", landscape->absorption[i][j]);
        }
        printf("\n");
    }
    
    /*
    printf("printing raindrops\n");
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            printf("%.2f ", landscape->raindrops[i][j]);
        }
        printf("\n");
    }
    printf("printing trickle\n");
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            printf("%.2f ", landscape->trickle[i][j]);
        }
        printf("\n");
    }
    
     */
    
    Landscape_Destroy(&landscape);


    return 0;
}

