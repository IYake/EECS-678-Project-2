/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */

void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->m_size = 0;
	q->m_front = NULL;
	q->comparer = comparer;
}

void node_init(struct node_t* node){
	node->value = NULL;
	node->next = NULL;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	//ptr is expected to be a node
	node_t* newNode = malloc(sizeof(*newNode));
	node_init(newNode);
	node_t* temp = q->m_front;
	newNode->value = ptr;
	newNode->next = NULL;
	int counter = 0;
	if(q->m_front == NULL) {
	  q->m_front = newNode;
  }
	else if (q->comparer (newNode->value, q->m_front) <= 1) { //should this be 1 or 0?
		q->m_front = newNode;
		newNode->next = temp;
		counter++;
	}
	else {
		while (temp->next != NULL && q->comparer (newNode->value, temp->next->value) > 1) {
			temp = temp->next;
			counter++;
		}
		node_t* tempNode = temp->next;
		temp->next = newNode;
		newNode->next = tempNode;
		counter++;
	}

	// return -1; default return
	//might not return 0
	q->m_size++;
	return counter;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	return q->m_front->value;
	// return NULL;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	node_t* temp;
	temp = q->m_front;
	if (temp == NULL)	{
		return(NULL);
	}
	else if (temp->next == NULL){
		q->m_front = NULL;
		q->m_size -= 1;
	}
	else{
		q->m_front = q->m_front->next;
		q->m_size -= 1;
	}
	return temp->value;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	node_t* temp = q->m_front;
	if (index > q->m_size){
		return NULL;
	}
	else{
		for (int i = 0; i < index; i++){
			temp = temp->next;
		}
		return temp->value;
	}
	// return NULL;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	/*Free nodes when removed*/
	int removed = 0;
	node_t* temp1 = q->m_front;
	node_t* temp2 = q->m_front;
	
	while (temp1 != NULL){
		if (*(int*)temp1->value == *(int*)ptr){
			if (temp1->next == NULL){
				temp1 = NULL;
			}
			else {
				temp1->next = temp1->next->next;
				temp1 = temp1->next->next;
			}
			removed++;
		}
		else{
			temp1 = temp1->next;
			temp2 = temp2->next;
			
		}
	}
	q->m_size -= removed;
	return removed;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	/*Free nodes when removed*/
	if (q->m_front == NULL || index > q->m_size){
		return NULL;
	}
	else if (index == 0){
		return q->m_front;
	}
	else{
		//node trailing temp2
		node_t* temp1 = q->m_front;
		//nod that will be on the specified value
		node_t* temp2 = q->m_front;
		for (int i = 0; i < index-1; i++){
			temp1 = temp1->next;
		}
		temp2 = temp1->next;
		temp1->next  = temp2->next;
		node_t* value = temp2->value;
		temp2 = NULL;
		return value;
	}
	// return 0;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->m_size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	/*Free nodes when removed*/
}

void test_queue(){
	printf("TESTING");
}
