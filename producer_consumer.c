#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>


//turn some of the define values into user input values
#define NUM_THREADS 5
#define SIZE 5
#define NUM_REQUESTS 8
#define MAX_WAIT_TIME 9

//SEMAPHORE
sem_t mutex; //this locks the queue
sem_t wait_if_empty; //this makes consumer wait until there is something to grab
sem_t wait_if_full; //this makes producer wait until there is room to add to the queue


//QUEUE FUNCTIONS
typedef struct {
    int id, time;
    int message[];
}request;

void initRequest(request *r, int id, int time){
    r-> message[2];
}

typedef struct{
    int head;
    int tail;
    int size;
   request items[];
}queue;

//initialize Queue class
void initQueue(queue *q, int elements){
q-> head = -1;
q-> tail = -1;
q-> size = elements;
q -> items[elements];
}
//isEmpty
//if the queue is empty return 1(tr
int isEmpty(queue *q) {
    if(q->head == -1){
        return 1;
    } else{
        return 0;
    }
}

//isFull
//if the queue is full return 1(true)
int isFull(queue *q){
    if ((q->head == q->tail + 1) || (q->head == 0 && q->tail == (q->size)- 1)) {
      // if((q->head == 0 && q->tail == (q->size) -1) || q->tail == (q->head)%((q->size)-1)){
        return 1;
    }else{return 0;}

}

void enqueue(queue *q, int id, int time){
     //int message[2] = {id, time};
     request message = {id, time};
    if(isFull(q)){
        printf("queue full (enqueue)\n");
    } else{
        if(q->head == -1){
            q->head = 0;
        }
        q->tail = (q->tail  + 1)% q->size;
        q->items[q->tail] = message;
        printf("Insert values id %d and execution time %d\n", message.id, message.time);
    }
}

request dequeue(queue *q){
    request return_values;
    if(isEmpty(q)){
        //printf("empty queue\n");
    } else{
        return_values = q->items[q->head];
        //printf("we dequeuing these values: %d, %d\n", return_values.id, return_values.time);
        if(q->head == q->tail){
            q->head = -1;
            q->tail = -1;
        } else{
            q->head = (q->head + 1) % q->size;
        }
        //printf("dequeue\n");
        return return_values;
    }
}

void display(queue *q){
    if(isEmpty(q)){
        printf("queue empty\n");
    } else{
        for(int i = q->head; i != q->tail; i = (i+1) %q->size){
            printf("the id is: %d, the execution time is: %d \n", q->items[i].id, q->items[i].time);
        }
        printf("the id is: %d, the execution time is: %d \n", q->items[q->tail].id, q->items[q->tail].time);
    }
}


//delay function
//wait for n seconds;
void delay(int wait_time){

 time_t start = time(NULL);
 time_t current;

 do{
     time(&current);
 } while(difftime(current, start)< wait_time);

}
//Pthread args
typedef struct{
    queue queue;
    int number;
}pargs;

//PRODUCER CONSUMER

void producer(queue *q){
    //pargs *producer_pargs = (pargs*)Pargs;
    int ID = 0;
    int wait;

    int randomTimeProducer;

    printf("change\n");

     //produce n amount of requests
    for(int i = 0; i < NUM_REQUESTS; i++){
        //wait for random seconds
        wait = 1;
        if(wait == 1){
            randomTimeProducer = rand() %MAX_WAIT_TIME;
            printf("producer is sleeping for %d seconds\n", randomTimeProducer);
            delay(randomTimeProducer);
        }
        
        //semaphore for producer
        sem_wait(&wait_if_full);
        //while the queue is not full -> add a value to the queue
            sem_wait(&mutex); //lock the queue from being read
            ID++; //update id sequentially
            int randomTimeConsumer = rand() %(MAX_WAIT_TIME *2);
            enqueue(q, ID,randomTimeConsumer);
            //display(q);
            sem_post(&mutex); //release lock on updating the
    
        sem_post(&wait_if_empty); //let consumer know there is a request available
    }
    //give consumers a chance to finish consuming
    while(1){

    }
}

 void consumer(queue *q){
    
    //values from queue
    request values;

    //Timer
    time_t con_start, con_finish;
    time(&con_start);
    int elapsed;
    long time_wait;
    //each thread will run its own version of consumer, so busy is a local variable to that thread
    //try to consume //this is just to ensure all threads have a chance to continue checking for something in the queue (could have been more elegant)
    for(int i = 0; i < (NUM_REQUESTS); i++){
    int busy = 0;
        //wait until something to consume
        sem_wait(&wait_if_empty);
        //if(isEmpty(q) == 0){
        //lock and aquire data from the queue
        busy = 1;
        sem_wait(&mutex);
        values = dequeue(q);
        //printf("Consumer %d assigned request %d, processing request for the next %d seconds\n", pid_t, values.id, values.time);
        //display(q);
        sem_post(&mutex);
        sem_post(&wait_if_full);

        if(busy == 1){
            // current time is CURRENT-TIME
            // Consumer i: completed request ID n at time CURRENT-TIME
            printf("the current time is: %ld\n", time(&con_start));
            delay(values.time);
            printf("Consumer %ld: completed request %d at %ld\n", pthread_self(), values.id, time(&con_finish));
            busy = 0;
        }
    //need to go to top again
    }
}



//MAIN

pid_t pid_fork;

int main(int argc,  char* argv[]){
   // int num_threads = 5;
    pthread_t thread[NUM_THREADS];
    //pthread_t parent_thread;

    //intialize the queue
    queue q;
    initQueue(&q, SIZE);

     //initialize pthread arguments
    pargs thread_arg;
    thread_arg.queue = q;
     
    

    //initialize semaphore
     sem_init(&mutex, 0, 1); //mutex will always only have 1
     sem_init(&wait_if_full, 0, (SIZE));//the producer will be able to continue add to queue initally for the size of the queue
     sem_init(&wait_if_empty, 0, 0); //the queue is initally 0, so consumer cannot try to take something
     
    
    //these are all the threads
    for(int i = 0; i <NUM_THREADS; i++){
        
        //parent thread is first, rest are childern
        if(i == 0){
            fflush(stdout);
             thread_arg.number = i;
             pthread_create(&thread[i], NULL, (void *)producer, &thread_arg);
             
        } else{ 
            fflush(stdout);
            thread_arg.number = i;
            pthread_create(&thread[i], NULL, (void *)consumer, &thread_arg);
        }

    }
    fflush(stdout);
    //join the threads to terminate //program won't get here//program must be terminated manually 
    for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(thread[i], NULL);  
}

//destroy the semaphores
sem_destroy(&mutex);
sem_destroy(&wait_if_empty);
sem_destroy(&wait_if_full);
    

}
