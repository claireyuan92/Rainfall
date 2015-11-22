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
<<<<<<< HEAD
<<<<<<< HEAD

typedef landscape_t* Landscape;


=======

typedef landscape_t* Landscape;


>>>>>>> parent of 8e22957... working sequential code with drained count
=======

typedef landscape_t* Landscape;


>>>>>>> parent of 8e22957... working sequential code with drained count
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
<<<<<<< HEAD
<<<<<<< HEAD
            printf("%d ",elevation[i][j]);
        }
        printf("\n");
=======
            //printf("%d ",elevation[i][j]);
        }
        //printf("\n");
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
            //printf("%d ",elevation[i][j]);
        }
        //printf("\n");
>>>>>>> parent of 8e22957... working sequential code with drained count
    }
    
    /*
    
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
<<<<<<< HEAD
<<<<<<< HEAD
            //fscanf(fptr, "%d", &elevation[i][j]);
=======

>>>>>>> parent of 8e22957... working sequential code with drained count
=======

>>>>>>> parent of 8e22957... working sequential code with drained count
            printf("%d ",elevation[i][j]);
        }
        printf("\n");
    }
<<<<<<< HEAD
<<<<<<< HEAD

    
    fclose(fptr);
    printf("Finished reading image file !\n");
=======
     */

    
    fclose(fptr);
    //printf("Finished reading image file !\n");
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
     */

    
    fclose(fptr);
    //printf("Finished reading image file !\n");
>>>>>>> parent of 8e22957... working sequential code with drained count
    
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
    }
    
    landscape->absorption=absorption;
    
    // allocate memory for raindrops matrix
    double **raindrops=(double**) malloc (N* sizeof(double*));
    assert(raindrops!=NULL);
    
    for (i=0; i<N; i++) {
        raindrops[i]=(double*) malloc(N*sizeof(double));
        assert(raindrops[i]!=NULL);
    }
    
    landscape->raindrops=raindrops;
    
    double **trickle = (double**) malloc(N*sizeof(double*));
    assert(trickle!=NULL);
    
    for (i=0; i<N; i++) {
        trickle[i]=(double*) malloc(N*sizeof(double));
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
<<<<<<< HEAD
<<<<<<< HEAD



void RainDrop(landscape_t *landscape, double A, int N){
    //Traverse over all landscape points
    int i,j;
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            
            //1) Receive a new raindrop
            landscape->raindrops[i][j]+=1;
            
            //2) If there are raindrops on a point, absorb water into the point
            if(landscape->raindrops[i][j]){
                landscape->absorption[i][j]+=A*landscape->raindrops[i][j];
                landscape->raindrops[i][j]-=A*landscape->raindrops[i][j];
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
    
    
    //Second traversal over all landscape points
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
            
            //trickle out from current point
            landscape->raindrops[i][j]-=landscape->trickle[i][j];
            
            //trickle to lowest neigbhors
            
            int m=min(landscape->elevation[i+1][j],
                      min(landscape->elevation[i][j+1],
                          min(landscape->elevation[i-1][j], landscape->elevation[i][j-1])));
            
            if(m<landscape->elevation[i][j]){
                
                int s=0;
                int x,y;
                
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][j]) {
                            s++;
                        }
                    }
                }
                
                double portion=landscape->trickle[i][j]/s;
                
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][j]) {
                            
                            landscape->raindrops[x][y]+=portion;
                        }
                    }
                }
                
                
            }
            else{
                landscape->raindrops[i][j]+=landscape->trickle[i][j];
            }
            
            
        }
    }
}
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
>>>>>>> parent of 8e22957... working sequential code with drained count

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
<<<<<<< HEAD
<<<<<<< HEAD
            if(fabs(landscape->raindrops[i][j]) > 10e-15){
                landscape->absorption[i][j]+=A*landscape->raindrops[i][j];
                landscape->raindrops[i][j]-=A*landscape->raindrops[i][j];
                flag=true;
            }
=======
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
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
            
<<<<<<< HEAD
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
            
            //3a) Calculate the amount of raindrops that will next trickle to the lowest neighbor
            if(landscape->raindrops[i][j]>1){
                landscape->trickle[i][j]=1;
            }
            else{
                landscape->trickle[i][j]=landscape->raindrops[i][j];
            }
            
            
<<<<<<< HEAD
<<<<<<< HEAD
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
            
            if(m<landscape->elevation[i][j]){
                
                int s=0;
                int x,y;
                
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][j]) {
                            s++;
                        }
                    }
                }
                
                double portion=landscape->trickle[i][j]/(double)s;
                
                for (x=i-1; x<=i+1; x++) {
                    for (y=j-1; y<=j+1; y++) {
                        if ( !(x==i-1 && y==j-1) && !(x==i-1 && y==j+1) && !(x==i+1 && y==j-1) && !(x==i+1 && y==j+1)
                            && m==landscape->elevation[x][j]) {
                            
                            landscape->raindrops[x][y]+=portion;
                        }
                    }
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
    
    int i, j,k=M;
    
    /*
    for (k=0; k<M; k++) {
        
        RainDrop(landscape, A,N);
        
    }
     */
    
    landscape->complete_step=0;
    


    while(Absorb(landscape,A,N,k)) { //continue to absorb when return true;
        landscape->complete_step++;
        k--;
    }
    
    return landscape;
    
    
=======
        }
    }
    
    
=======
        }
    }
    
    
>>>>>>> parent of 8e22957... working sequential code with drained count
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
<<<<<<< HEAD
                }
                */
                if (m==landscape->elevation[i][j-1]) {
                    landscape->raindrops[i][j-1]+=landscape->trickle[i][j]/s;
                }
                if (m==landscape->elevation[i][j+1]) {
                    landscape->raindrops[i][j+1]+=landscape->trickle[i][j]/s;
                }
=======
                }
                */
                if (m==landscape->elevation[i][j-1]) {
                    landscape->raindrops[i][j-1]+=landscape->trickle[i][j]/s;
                }
                if (m==landscape->elevation[i][j+1]) {
                    landscape->raindrops[i][j+1]+=landscape->trickle[i][j]/s;
                }
>>>>>>> parent of 8e22957... working sequential code with drained count
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
    
    
<<<<<<< HEAD
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
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
<<<<<<< HEAD
<<<<<<< HEAD
    
    landscape = Rainfall(argv[4], M,A,N);
=======
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
    
    clock_t start=clock();
    
    landscape = Rainfall(argv[4], M,A,N);
    
    clock_t end = clock();
    
    printf("Time consumed %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);
<<<<<<< HEAD
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
    
    printf("Rainfall simulation took %d time steps to complete.\n", landscape->complete_step);
    
    printf("The following grid shows the number of raindrops absorbed at each point:\n");
    
    int i, j;
    for (i=1; i<=N; i++) {
        for (j=1; j<=N; j++) {
<<<<<<< HEAD
<<<<<<< HEAD
            printf("%.2f ", landscape->absorption[i][j]);
=======
=======
>>>>>>> parent of 8e22957... working sequential code with drained count
            printf("%8g ", landscape->absorption[i][j]);
>>>>>>> parent of 8e22957... working sequential code with drained count
        }
        printf("\n");
    }
    
<<<<<<< HEAD
<<<<<<< HEAD
=======
    /*
>>>>>>> parent of 8e22957... working sequential code with drained count
=======
    /*
>>>>>>> parent of 8e22957... working sequential code with drained count
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

