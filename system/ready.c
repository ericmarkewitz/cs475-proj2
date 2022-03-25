/* ready.c - ready */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

struct queue *readyqueue;	/** ready queue */


/**
 * Put process on ready queue
 * @param pid	ID of process to place on ready queue
 * @param resch	reschedule afterward?
 */
status	ready(pid32 pid, bool8 resch)
{
	register struct procent *prptr;

	if (isbadpid(pid))
		return(SYSERR);

	// set process state to indicate ready and add to ready list
	prptr = &proctab[pid];

	// TODO - set the process' state pointed by prptr to "ready"
	prptr->prstate = PR_READY;	//DC REMOVE

	pri16 priority = prptr->prprio;

	//printqueue(readyqueue);

	// TODO - enqueue the process
	enqueue(pid, readyqueue, priority); //DC REMOVE

	//printqueue(readyqueue);

	//kprintf("\n\n\n");
	printqueue(readyqueue);

	if(AGING && readyqueue->size > 3){
		aging(readyqueue);
		kprintf("\nAGED!\n");
	}
	printqueue(readyqueue);
	
	kprintf("\n\n\n");


	if (resch == RESCHED_YES){
		
		resched();
	}
		
	return OK;
}

/**
 * @brief 1/3 times we want to take the last element in the queue
 * pop it off and give it a priority of one more than the current head
 * 
 */
void aging(struct queue *q){
	int randVal = rand() % 3;
	kprintf("Random Value: %d\n", randVal);

	if(randVal == 0){
		kprintf("Inside loop\n");
		struct qentry *oldHead = q->head; //Pointer to the head
		struct qentry *beforeTail = q->tail->prev;

		pid32 pidBeforeTail = remove(beforeTail->pid, q); //Pop off entry before tail

		int currHeadPrio = oldHead->key;

		enqueue(pidBeforeTail, q, currHeadPrio+1);
	}
	return;
}