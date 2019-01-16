#include <stdlib.h>
#include<stdio.h>
#include<math.h>
#include <time.h>
#include "mpi.h"

typedef struct {
        float mark_average;
        float tuition;
        int years;

}student;

int main( int argc, char** argv ){ 

  FILE *fp;
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

  MPI_Datatype   *message_type_ptr; 
  int            block_lengths[3];   /* Długości bloków (tu równe 1) */
  MPI_Aint       displacements[3];   /* Długości pól */
  MPI_Aint       addresses[4];       
  MPI_Datatype   typelist[3];        

  student student1;

  typelist[0]=MPI_FLOAT;       
  typelist[1]=MPI_FLOAT;
  typelist[2]=MPI_INT;
  block_lengths[0]=block_lengths[1]=block_lengths[2]=1;
  /* Adresy pól */
  MPI_Get_address(&student1, &addresses[0]);
  MPI_Get_address(&(student1.mark_average), &addresses[1]);
  MPI_Get_address(&(student1.tuition), &addresses[2]);
  MPI_Get_address(&(student1.years), &addresses[3]);
  /* Przesunięcia */ 
  displacements[0]=addresses[1]-addresses[0];
  displacements[1]=addresses[2]-addresses[1];
  displacements[2]=addresses[3]-addresses[2];
  
  MPI_Type_create_struct(3, block_lengths, displacements,
                  typelist, message_type_ptr);
  MPI_Type_commit(message_type_ptr);






    if( rank != 0 ){ 
      int count;
      dest=0; 
      tag=0; 
      student pierwszak;
      pierwszak.years = 23;
      pierwszak.tuition = 2500.1;
      pierwszak.mark_average = (float)rank;
      start_t = clock();
      MPI_Send( &pierwszak,  sizeof(pierwszak), *message_type_ptr, dest, tag, MPI_COMM_WORLD );
      end_t = clock();
      printf("Czas wykonania zadania: (%d).\n", (end_t-start_t));
    } else {
      student student2;

      for( i=1; i<size; i++ ) { 
	    MPI_Recv( &student2, sizeof(student2), *message_type_ptr, MPI_ANY_SOURCE, 
		  MPI_ANY_TAG, MPI_COMM_WORLD, &status );
	    /*printf("Dane od procesu o randze (i=%d): MPI_SOURCE: (%d).\n",i, status.MPI_SOURCE);
      printf("Student lat: %d, srednia ocen: %.2f, czesne do zaplaty: %.2f.\n", 
	    student2.years,student2.mark_average,student2.tuition);*/
      }
      
    }


  MPI_Type_free(message_type_ptr);
  MPI_Finalize(); 
  
  return(0);

}

