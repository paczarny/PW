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

    int rank, size, source, dest, tag, i; 
    MPI_Status status;
    double start_t, end_t;
    
    MPI_Init( &argc, &argv );
  

    int blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_FLOAT, MPI_FLOAT, MPI_INT};
    MPI_Datatype mpi_student_type;
    MPI_Aint     offsets[3];

    offsets[0] = offsetof(student, mark_average);
    offsets[1] = offsetof(student, tuition);
    offsets[1] = offsetof(student, years);

    MPI_Type_create_struct(3, blocklengths, offsets, types, &mpi_student_type);
    MPI_Type_commit(&mpi_student_type);



  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );

    if(size<=1){
      printf("Min ilość procesów to 2.\n", rank);
      exit(0);
    }

    start_t = MPI_Wtime();

    if( rank != 0 ){ 
      char buff[100];
      int position=0;
      student pierwszak;
      MPI_Recv(buff, 100, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status );
      MPI_Unpack(buff, 100, &position, &pierwszak, 1, mpi_student_type, MPI_COMM_WORLD);
    } else {
      char buffer[100];
      int position=0;
      student pierwszak;
      pierwszak.years = 23;
      pierwszak.tuition = 2500.1;
      pierwszak.mark_average = 3.0;
      MPI_Pack(&pierwszak, 1, mpi_student_type, buffer, 100, &position, MPI_COMM_WORLD);
      for( i=1; i<size; i++ ) { 
         MPI_Ssend(buffer, 100, MPI_PACKED, i, 0, MPI_COMM_WORLD );
      }
    }

  end_t = MPI_Wtime();
  MPI_Finalize(); 

  if (rank == 0) {
    printf("time %lf /n", (end_t-start_t)*1000000.);//mikrosekund
  }

  
  return(0);

}

