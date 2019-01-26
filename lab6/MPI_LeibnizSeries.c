#include <stdlib.h>
#include<stdio.h>
#include<math.h>
#include <time.h>
#include "mpi.h"
#include <unistd.h>

 int position;
 int steps;
 int countSteps(int size);
 

int main( int argc, char** argv ){ 
  
  if(argc!=2){
  	printf("Bledna ilosc argumentow.\n");
    exit(-1); 
  }

  int rank, size, dest, tag=0, i; 
  MPI_Status status;
  steps=atoi(argv[1]);
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  if(size<3){
    printf("Pojedynczy proces o randze: %d (brak komunikatów)\n", rank);
    exit(-1);
  }

    if(rank==0) {
      
      char buffer[100];
      int start=1;
      int end;
      int endInterval=2*steps-1; 
      int stepForProcess=countSteps(size-2);

      for(int i=2;i<size;i++){
        end=i*stepForProcess*2-1;
        if(end>endInterval)
          end=endInterval;
        position=0;       
        MPI_Pack(&start, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&end, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, 100, MPI_PACKED, i, 100, MPI_COMM_WORLD );
        start=end+2;
      }
      
    }
    else if(rank==1){
      double partSum=0.0;
      double sum=0.0;
      for(int i=2;i<size;i++){
        MPI_Recv(&partSum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        sum+=partSum;
       }
      double bezwzgledny = M_PI_4-sum;
      printf("PIIII:%.16f\n",sum*4.0);
      printf("Blad bezwzgledny:%.16f\n",bezwzgledny);
      printf("Blad wzgledny:%.16f\n",bezwzgledny/M_PI_4);
    }
    else{ 
      char buffer[100];
      int start;
      int end;
      double sum=0.0;
	    MPI_Recv(buffer, 100, MPI_PACKED, MPI_ANY_SOURCE, 
	    100, MPI_COMM_WORLD, &status);
      position=0;
      MPI_Unpack(buffer, 100, &position, &start, 1, MPI_INT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &end, 1, MPI_INT, MPI_COMM_WORLD);
      for(int i=start;i<=end;i+=2){
      	if((i%4)==3)
      	sum-=1.0/(double)i;
      	else
      	sum+=1.0/(double)i;
      }
      MPI_Send(&sum, 1, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);	
    }



  MPI_Finalize(); 
  
  return(0);

}


  int countSteps(int size){
  	if((steps%size)==0)
  		return steps/size;
  	else{
  		return steps/size+1;
  	}
  }
