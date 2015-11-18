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


typedef struct{
    int ** elevation;
    double ** absorbed;
    double ** raindrops;
    double ** trickle;
    int N;
    int complete_step;
} landscape_t;

typedef landscape_t* Landscape;

typedef struct{
    int x;
    int y;
}coord_t;

int** read_elevation(char *filename, int N){
    
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
	    printf("%d ",elevation[i][j]);
        }
	printf("\n");
    }
        
    fclose(fptr);
    printf("Finished reading image file !\n");
    
    return elevation;
    
}

Landscape Landscape_Init(char * filename, int N){
    
    int i,j;
    
    landscape_t * landscape;
    landscape=(landscape_t*) malloc(sizeof(landscape_t));
    assert(landscape!=NULL);
    
    landscape->N=N;
    landscape->elevation=read_elevation(filename,N);
    
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
    
}

void Landscape_Destroy(Landscape * landscape){
    
    free((*landscape)->elevation);
    (*landscape)->elevation=NULL;
    free(*landscape)->absorption;
    (*landscape)->absorption=NULL;
    free(*landscape);
    *landscape=NULL;
}

void RainDrop(landscape_t *landscape, int M, double A){
    
}

int min(int a, int b){
    return a<b? a:b;
}

coord_t* Trickle(landscape_t* landscape, int i, int j){
    
    //coord_t * res=malloc(sizeof(*res));
    int s=1;
    
    if (i==0 && j==0) {
        int m=min(landscape->elevation[i+1][j], landscape->elevation[i][j+1]);
        if (m<landscape->elevation[i][j]) {
            if (m==landscape->elevation[i+1][j]) {
                coord_t ind;
                ind.x=i+1;ind.y=j;
                
            }
        }
    }
    
    if(i>0 && i< landscape->N-1) {
        //inner lands
        if (j>0 && j<landscape->N-1) {
            
            int m=min(landscape->elevation[i+1][j],
                      min(landscape->elevation[i][j+1],
                          min(landscape->elevation[i-1][j], landscape->elevation[i][j-1])));
            
            if(m<landscape->elevation[i][j]){
                if(m==landscape->elevation[i+1][j]){
                    res=realloc(res,s*sizeof(*res));
                    res[s-1].x=i+1; res[s-1].y=j;
                }
            }
        }
    }
    return res;
}



Landscape Rainfall(char *filename, int M, double A, int N){
    
    landscape_t *landscape = Landscape_Init(filename,N);
    
    int i, j,k;
    
    for (k=0; k<M; k++) {
        
        //RainDrop(landscape, M, A);
        
        //Traverse over all landscape points
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                
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
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++) {
                
                //trickle out from current point
                landscape->raindrops[i][j]-=landscape->trickle[i][j];
                
                //trickle to lowest neigbhors
                coord_t * lowestnbrs = Trickle(landscape,i,j);
                
                if(lowestnbrs==NULL){
                    landscape->raindrops[i][j]+=landscape->trickle[i][j];
                }
                else{
                    int s=sizeof(lowestnbrs)/sizeof(lowestnbrs[0]);
                    double portion=landscape->trickle[i][j]/s;
                    int m;
                    for (m=0; m<s; m++) {
                        int x=lowestnbrs[m].x;
                        int y=lowestnbrs[m].y;
                        
                        landscape->raindrops[x][y]+=portion;
                        
                        
                    }
                }
                
                free(lowestnbrs);
                lowestnbrs=NULL;
                
            }
        }
        
    }
    
    
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
    sscanf(argv[2],"%.2f",&A); // absorption rate
    
    int N=atoi(argv[3]); // dimensionof the landscape
    
    
    


    return 0;
}

