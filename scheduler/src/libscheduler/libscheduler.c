/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/

typedef struct _job_t
{
	int num;
	int arrival_time;
	int start_time;
	int remaining_time;
	int run_time;
	int priority;
} job_t;

priqueue_t queue;
int preemptive;
int numCores;
int curr_time;//maybe this should be float
int total_jobs;
float waiting_time;
float response_time;
float turnaround_time;

job_t** activeCores;

int fcfs(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	return jobA->arrival_time - jobB->arrival_time;
}

int sjf(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	if (jobA->run_time != jobB->run_time){
		return jobA->run_time - jobB->run_time;
	}
	else{
		return jobA->arrival_time - jobB->arrival_time;
	}
}
int psjf(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	if (jobA->remaining_time != jobB->remaining_time){
		return jobA->remaining_time - jobB->remaining_time;
	}
	else{
		return jobA->arrival_time - jobB->arrival_time;
	}
}
int pri(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	if (jobA->priority != jobB->priority){
		return jobA->priority - jobB->priority;
	}
	else{
		return jobA->arrival_time - jobB->arrival_time;
	}
	return 0;
}
int ppri(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	if (jobA->priority != jobB->priority){
		return jobA->priority - jobB->priority;
	}
	else{
		return jobA->arrival_time - jobB->arrival_time;
	}
	return 0;
}
int rr(const void *a, const void *b){
	job_t* jobA = (job_t*)a;
	job_t* jobB = (job_t*)b;
	if (jobA->num == jobB->num){
		return 0;
	}
	return 1;
}
/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/

void scheduler_start_up(int cores, scheme_t scheme)
{
	numCores = cores;
	total_jobs = 0;
	waiting_time = 0.0;
	turnaround_time = 0.0;
	response_time = 0.0;
	curr_time = 0.0;

	activeCores = malloc(sizeof(job_t) * cores);
	for (int i = 0; i < numCores; i++){
		activeCores[i] = 0;
	}

	switch(scheme){
		case FCFS:
			priqueue_init(&queue,fcfs);
			preemptive = 0;
			break;
		case SJF:
			priqueue_init(&queue,sjf);
			preemptive = 0;
			break;
		case PSJF:
			priqueue_init(&queue,psjf);
			preemptive = 1;
			break;
		case PRI:
			priqueue_init(&queue,pri);
			preemptive = 0;
			break;
		case PPRI:
			priqueue_init(&queue,ppri);
			preemptive = 1;
			break;
		case RR:
			priqueue_init(&queue,rr);
			preemptive = 0;
			break;
	}
}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	update_remaining_time(time);
	total_jobs++;
	job_t* job = malloc(sizeof(job_t));
	job->num = job_number;
	job->arrival_time = time;
	job->start_time = -1;
	job->run_time = running_time;
	job->remaining_time = running_time;
	job->priority = priority;

	int core = -1;
	for (int i = 0; i < numCores; i++){

		if (activeCores[i] == 0){
			core = i;
			break;
		}
	}

	if (core != -1){
		job->start_time = time;
		activeCores[core] = job;
		return core;
	}

	if (preemptive){
		job_t* least_priority_job = job;

		for (int i = 0; i < numCores; i++){
			if (queue.comparer(least_priority_job,activeCores[i]) < 0){
				core = i;
				least_priority_job = activeCores[i];
			}
		}

		if (core != -1){
			job->start_time = time;
			if (time == least_priority_job->start_time){
				least_priority_job->start_time = -1;
			}
			activeCores[core] = job;
			priqueue_offer(&queue,least_priority_job);
			return core;
		}
	}

	job->start_time = -1;
	priqueue_offer(&queue,(void*)job);
	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	update_remaining_time(time);

	job_t* finished_job = activeCores[core_id];
	waiting_time += time - finished_job->run_time - finished_job->arrival_time;
	response_time += finished_job->start_time - finished_job->arrival_time;
	turnaround_time += time - finished_job->arrival_time;

	free(finished_job);
	activeCores[core_id] = 0;

	if (queue.m_front != NULL){
		job_t* job = priqueue_poll(&queue);

		if (job->start_time == -1){
			job->start_time = time;
		}
		activeCores[core_id] = job;
		return job->num;
	}

	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	update_remaining_time(time);
	job_t* job = activeCores[core_id];

	if (queue.m_front != NULL){
		
		priqueue_offer(&queue,job);
		job = priqueue_poll(&queue);

		if (job->start_time == -1){
			job->start_time = time;
		}

		activeCores[core_id] = job;
	
	}
	return job->num;
	
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return (total_jobs > 0 ? waiting_time/total_jobs : 0.0);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return (total_jobs > 0 ? turnaround_time/total_jobs : 0.0);
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
		return (total_jobs > 0 ? response_time/total_jobs : 0.0);
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	priqueue_destroy(&queue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	node_t* temp = queue.m_front;
	if (queue.m_front != NULL){
		while (temp != NULL){
			job_t* job = temp->value;
			printf(
									"Num: %d, Arrival_time: %d, Start_time: %d, Remaining_time: %d, Run_time: %d, Priority: %d\n"
									,job->num,job->arrival_time,job->start_time,job->remaining_time,job->run_time,job->priority
								);
			temp = temp->next;
		}
		printf("\n");
	}
		
}

void update_remaining_time(int time){
	for (int i = 0; i < numCores; i++){
		if (activeCores[i] != 0){
			activeCores[i]->remaining_time -= time - curr_time;
		}
	}
	curr_time = time;
}
