#include <stdlib.h>
#include<stdio.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include <unistd.h>

#define eulerConstant 0.57721566490153286060651209008240243

 int position;
 int iterations;
 int countSteps(int size);
 double sum=0;

int main( int argc, char** argv ){ 

  if(argc!=2){
  	printf("Bledna ilosc argumentow.\n");
    exit(-1); 
  }

  int rank, size, dest, tag=0, i; 
  MPI_Status status;
  iterations=atoi(argv[1]);
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  if(size<=1 || iterations<size){
    printf("Pojedynczy proces o randze: %d (brak komunikatów)\n", rank);
    exit(-1);
  }


    if( rank != 0 ){ 
      char buffer[100];
      int start;
      int steps;
      double sum=0.0;
	    MPI_Recv(buffer, 100, MPI_PACKED, MPI_ANY_SOURCE, 
	    100, MPI_COMM_WORLD, &status);
      position=0;
      MPI_Unpack(buffer, 100, &position, &start, 1, MPI_INT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &steps, 1, MPI_INT, MPI_COMM_WORLD);
      for(int i=0;i<steps;i++){
      	sum+=1.0/(double)start;
        start++;
      }
      MPI_Send(&sum, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);	
    }
    else {
      double partSum=0.0;
      char buffer[100];
      int start=1;
      int steps=countSteps(size);

      for(int i=1;i<size;i++){
      	if((i==size-1) && (iterations%steps!=0))
          steps=iterations%steps;
        position=0;      	
		    MPI_Pack(&start, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&steps, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, 100, MPI_PACKED, i, 100, MPI_COMM_WORLD );
        start+=steps;
      }

       for(int i=1;i<size;i++){
        MPI_Recv(&partSum, 1, MPI_DOUBLE,MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        sum+=partSum;
       }
       double gamma = sum - log(iterations);
       double bezwzgledny = fabs(eulerConstant-gamma);
       printf("Otrzymany wynik:%.16f\n",gamma);
       printf("Blad wzgledny:%.16f\n",bezwzgledny);
	     printf("Blad wzgledny:%.16f\n",bezwzgledny/eulerConstant);
    }


  MPI_Finalize(); 
  
  return(0);

}


  int countSteps(int size){
  	if((iterations%(size-1))==0)
  		return iterations/(size-1);
  	else{
  		return iterations/(size-1)+1;
  	}
  }
