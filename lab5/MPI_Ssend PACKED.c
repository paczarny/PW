#include <stdlib.h>
#include<stdio.h>
#include<math.h>
#include <time.h>
#include "mpi.h"

 float mark_average,tuition;
 int years;
 char buffer[100];
 int position;

int main( int argc, char** argv ){ 
  
  int rank, size, source, dest, tag, i; 
  MPI_Status status;
  clock_t start_t, end_t, total_t;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  if(size<=1){
    printf("Pojedynczy proces o randze: %d (brak komunikatów)\n", rank);
    exit(0);
  }



    if( rank != 0 ){ 
      dest=0; 
      tag=0; 
      years = 23;
      tuition = 2500.1;
      mark_average = (float)rank;
      position=0;
      MPI_Pack(&mark_average, 1, MPI_FLOAT, buffer, 100, &position, MPI_COMM_WORLD);
      MPI_Pack(&tuition, 1, MPI_FLOAT, buffer, 100, &position, MPI_COMM_WORLD);
      MPI_Pack(&years, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
      start_t = clock();
      MPI_Ssend(buffer, 100, MPI_PACKED, dest, tag, MPI_COMM_WORLD );
      end_t = clock();
      printf("Czas wykonania zadania: (%d).\n", (end_t-start_t));
    } else {
      char buffer[100];
      for( i=1; i<size; i++ ) { 
	    MPI_Recv(buffer, 100, MPI_PACKED, MPI_ANY_SOURCE, 
		  MPI_ANY_TAG, MPI_COMM_WORLD, &status );
      position=0;
      MPI_Unpack(buffer, 100, &position, &mark_average, 1, MPI_FLOAT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &tuition, 1, MPI_FLOAT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &years, 1, MPI_INT, MPI_COMM_WORLD);
	    /*printf("Dane od procesu o randze (i=%d): MPI_SOURCE: (%d).\n",i, status.MPI_SOURCE);
      printf("Student lat: %d, srednia ocen: %.2f, czesne do zaplaty: %.2f.\n", 
	    years,mark_average,tuition);*/
      }
      
    }


  MPI_Finalize(); 
  
  return(0);

}

