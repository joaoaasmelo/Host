#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "p3_helper.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(...) printf("DEBUG: "__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) do {} while (0)
#endif

//The number of jobs to process
#define JOBS_NO 8 

//The number of concurrent workers
#define SIMULTANEOUS_WORKERS_MAX 2

// typedef struct {
// 	unsigned long input_value; //The value we want to compute
// 	unsigned char processed_flag; // boolean - Flag the result
// 	unsigned long result; //Stores the result after processing
// 	unsigned int worker_pid; //Worker process id - to track the process pid.
// } JOB_Handler;

void dispatch_jobs_v0(JOB_Handler jobs[], int number_of_jobs)
{
	//Process all jobs - monolithic approach
	for (int i=0; i<number_of_jobs; i++){
		DEBUG_PRINTF("Working job: %d (%lu)\n", i+1, jobs[i].input_value);
		JOB_update_result_and_processing_status_clear(jobs + i, prime_count(jobs[i].input_value));
	}

	DEBUG_PRINTF("All jobs processed!\n");
}

/*void dispatch_jobs_v1(JOB_Handler jobs[], int number_of_jobs)
{
    // Process all jobs - One process (worker) per job
    for (int i = 0; i < number_of_jobs; i++) {
        // Create a new process
        int pid = fork();

        if (pid == 0) {
            // Child process
            DEBUG_PRINTF("Working job: %d (%lu)\n", i + 1, jobs[i].input_value);
            JOB_update_result_and_processing_status_clear(jobs + i, prime_count(jobs[i].input_value));
            exit(0);
        }
        // No need to wait in parent process in monolithic approach
    }

    // Parent process continues without waiting
    DEBUG_PRINTF("All jobs processed!\n");
}*/

void dispatch_jobs_v1(JOB_Handler jobs[], int number_of_jobs)
{
    int pipe_fd[2]; // Pipe file descriptors
    
    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Process all jobs - One process (worker) per job
    for (int i = 0; i < number_of_jobs; i++) {
        // Create a new process
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            close(pipe_fd[0]); // Close unused read end
            DEBUG_PRINTF("Working job: %d (%lu)\n", i + 1, jobs[i].input_value);
            
            // Write the input value to the pipe
            write(pipe_fd[1], &jobs[i].input_value, sizeof(unsigned long));
            
            // Close the write end of the pipe
            close(pipe_fd[1]);
            
            // Process the job and update result
            JOB_Handler job = jobs[i];
            job.result = prime_count(job.input_value); // Assuming prime_count updates result in-place
            
            // Write the updated job back to the parent process
            write(pipe_fd[1], &job, sizeof(JOB_Handler));
            
            // Close the write end of the pipe
            close(pipe_fd[1]);
            
            exit(0);
        }
    }
    
    close(pipe_fd[1]); // Close write end in parent process
    
    // Parent process reads data from each child
    for (int i = 0; i < number_of_jobs; i++) {
        unsigned long input_value;
        JOB_Handler job;
        
        // Read input value from the pipe
        read(pipe_fd[0], &input_value, sizeof(unsigned long));
        
        // Read updated job from the pipe
        read(pipe_fd[0], &job, sizeof(JOB_Handler));
        
        // Close the read end of the pipe
        close(pipe_fd[0]);
        
        // Update the original job array with the result
        jobs[i] = job;
        
        DEBUG_PRINTF("Job %d processed!\n", i + 1);
    }
    
    // Close the read end of the pipe
    close(pipe_fd[0]);
    
    // Parent process continues after all jobs are processed
    DEBUG_PRINTF("All jobs processed!\n");
}

void dispatch_jobs_v2(JOB_Handler jobs[], int number_of_jobs)
{
	//Process all jobs - One process (worker) per job, with
	//a limit on the number of simultaneous workers (SIMULTANEOUS_WORKERS_MAX)

	int workers_count = 0;

	for (int i=0; i<number_of_jobs; i++){
		//Create a new process
		int pid = fork();

		if (pid == 0){
			//Child process
			DEBUG_PRINTF("Working job: %d (%lu)\n", i+1, jobs[i].input_value);
			JOB_update_result_and_processing_status_clear(jobs + i, prime_count(jobs[i].input_value));
			exit(0);
		}

		workers_count++;

		if (workers_count >= SIMULTANEOUS_WORKERS_MAX){
			//Parent process
			//Wait for one of the workers to finish
			int status;
			wait(&status);
			workers_count--;
		}
	}

}

int main(int argc, char const *argv[])
{
	/* 
	 * In  this main function we'll 
	 * 1 - Parametrize a set of job tasks 
	 * 2 - Call the job processing function 
	 * 3 - Print the processing results 
	 */

	JOB_Handler local_var_job_array[JOBS_NO];
	
	//Seeding...
	srand(time(NULL));

	//Fill in some random values
	for (int i=0; i<JOBS_NO; i++)
		JOB_init(local_var_job_array + i, (1+(rand() % 10))*100000 );  //[1..10]*100000
	
	//Process all jobs - blocking function
	//dispatch_jobs_v0(local_var_job_array, JOBS_NO);
	//dispatch_jobs_v1(local_var_job_array, JOBS_NO);
	dispatch_jobs_v2(local_var_job_array, JOBS_NO);

	//Print the results
	for (int i=0; i<JOBS_NO; i++){
		printf("Job %d, ", i+1);

		if (local_var_job_array[i].processed_flag) 
			printf(" Processed. Result=%lu", local_var_job_array[i].result);
		else 
			printf(" Not Processed.");

		printf("\n");
	}

	return 0;
}
