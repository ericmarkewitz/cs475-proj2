/* queue.c - enqueue, dequeue, isempty, nonempty, et al. */

#include <xinu.h>
#include <stdlib.h>

/**
 * Prints out contents of a queue
 * @param q	pointer to a queue
 */
void	printqueue(struct queue *q)
{
	//TODO - print all contents from head to tail
	kprintf("[");
	if (nonempty(q))
	{
		struct qentry *curr = q->head;
		while (curr != NULL)
		{
			kprintf("(pid=%u, key=%u)", curr->pid, curr->key);
			curr = curr->next;
		}
	}
	kprintf("] size: %d\n", q->size);
}

/**
 * Checks whether queue is empty
 * @param q	Pointer to a queue
 * @return 1 if true, 0 otherwise
 */
bool8	isempty(struct queue *q)
{
	//TODO - trivial
	return (q->size == 0);
}

/**
 * Checks whether queue is nonempty
 * @param q	Pointer to a queue
 * @return 1 if true, 0 otherwise
 */
bool8	nonempty(struct queue *q)
{
	//TODO - trivial (but don't just check q's size because it could be NULL)
	return (q == NULL || q->size > 0);
}


/**
 * Checks whether queue is full
 * @param q	Pointer to a queue
 * @return 1 if true, 0 otherwise
 */
bool8	isfull(struct queue *q)
{
	//TODO - check if there are at least NPROC processes in the queue
	return (q->size >= NPROC);
}


/**
 * Insert a qentry at its correct place in the queue based on the key
 * @param pid	ID process to insert
 * @param q	Pointer to the queue to use
 * @param key key to determine its palce in the queue
 *
 * @return pid on success, SYSERR otherwise
 */
pid32 enqueue(pid32 pid, struct queue *q, int32 key)
{
	if (isfull(q) || isbadpid(pid)) {
		return SYSERR;
	}

	//TODO - allocate space on heap for a new qentry
	struct qentry *newEntry = (struct qentry*) malloc(sizeof(struct qentry));

	//TODO - initialize the new QEntry
	newEntry->pid = pid;
	newEntry->key = key;
	//newEntry->prev = q->tail;
	//newEntry->next = NULL;

	//If there is only one element in the queue it's both the head and the tail
	if(q->size == 0){
		newEntry->prev = NULL;
		newEntry->next = NULL;

		q->head = newEntry;
		q->tail = newEntry;
		q->size = 1;
		
		return pid;
	}
	else{
		//If theres more than one element in the tail loop through and see where it fits
		struct qentry *currEntry = q->head;
		while(currEntry != NULL){
			if(newEntry->key > currEntry->key){
				newEntry->prev = currEntry->prev;
				newEntry->next = currEntry;

				currEntry->prev = newEntry;

				if(currEntry == q->head){
					q->head = newEntry;
				}
				else{
					struct qentry *beforeCurr = currEntry->prev->prev;
					//kprintf("Before curr pid, key: %u, %d", beforeCurr->pid, beforeCurr->key);
					beforeCurr->next = newEntry;
				}
				q->size++;
				return pid;
			}
			else{
				currEntry = currEntry->next;
			}
		}
		//If the new entry is smaller than the tail, make it the new tail
		currEntry = q->tail;
		newEntry->prev = currEntry;
		newEntry->next = NULL;

		currEntry->next = newEntry;

		q->tail = newEntry;
		q->size++;
		return pid;
		
	}
}


/**
 * Remove and return the first process on a list
 * @param q	Pointer to the queue to use
 * @return pid of the process removed, or EMPTY if queue is empty
 */
pid32 dequeue(struct queue *q)
{
	pid32	pid;	//ID of process removed

	//TODO - return EMPTY if queue is empty
	if (isempty(q)) {
		return EMPTY;
	}

	//TODO - get the head entry of the queue

	//TODO - unlink the head entry from the rest

	//TODO - free up the space on the heap

	//save pointer to head entry
	struct qentry *head = q->head;
	struct qentry *newHead = head->next;

	//save pid of head entry
	pid = head->pid;

	//unlink head from queue
	if (newHead != NULL)
		newHead->prev = NULL;
	else
		q->tail = NULL;

	//update queue to point head pointer at newhead
	q->head = newHead;

	//decrement size of queue
	q->size--;

	//deallocate head entry
	free(head, sizeof(struct qentry));

	return pid;
}


/**
 * Finds a queue entry given pid
 * @param pid	a process ID
 * @param q	a pointer to a queue
 * @return pointer to the entry if found, NULL otherwise
 */
struct qentry *getbypid(pid32 pid, struct queue *q)
{
	//TODO - return EMPTY if queue is empty
	if (isempty(q))
		return NULL;

	//TODO - find the qentry with the given pid
	struct qentry *currEntry = q->head;
	while(currEntry != NULL && currEntry->pid != pid)
		currEntry = currEntry->next;

	//TODO - return a pointer to it
	return currEntry;
}

/**
 * Remove a process from the front of a queue (pid assumed valid with no check)
 * @param q	pointer to a queue
 * @return pid on success, EMPTY if queue is empty
 */
pid32	getfirst(struct queue *q)
{
	//TODO - return EMPTY if queue is empty

	//TODO - remove process from head of queue and return its pid

	if (isempty(q)) {
		return EMPTY;
	}

	return dequeue(q);
}

/**
 * Remove a process from the end of a queue (pid assumed valid with no check)
 * @param q	Pointer to the queue
 * @return pid on success, EMPTY otherwise
 */
pid32	getlast(struct queue *q)
{
	//TODO - return EMPTY if queue is empty
	if (isempty(q)) {
		return EMPTY;
	}

	//TODO - remove process from tail of queue and return its pid
	return remove(q->tail->pid, q);
}



/**
 * Remove a process from an arbitrary point in a queue
 * @param pid	ID of process to remove
 * @param q	Pointer to the queue
 * @return pid on success, SYSERR if pid is not found
 */
pid32	remove(pid32 pid, struct queue *q)
{
	//TODO - return EMPTY if queue is empty
	if (isempty(q))
	{
		return EMPTY;
	}

	//TODO - remove process identified by pid parameter from the queue and return its pid


	//find the entry with pid
	struct qentry *currEntry = q->head;
	while (currEntry != NULL)
	{
		//found it!
		if (currEntry->pid == pid)
		{
			//unlink: find next and prev entries
			struct qentry *next = currEntry->next;
			struct qentry *prev = currEntry->prev;
			if (next != NULL)
				next->prev = prev;
			if (prev != NULL)
				prev->next = next;

			//update queue structure
			if (currEntry == q->head)
				q->head = prev;
			if (currEntry == q->tail)
				q->tail = next;
			q->size--;

			//deallocate current entry
			free(currEntry, sizeof(struct qentry));
			return pid;
		}
		currEntry = currEntry->next;
	}

	//TODO - if pid does not exist in the queue, return SYSERR
	return SYSERR;
}
