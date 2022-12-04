#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

//turn some of the define values into user input values
#define NUM_THREADS 5
#define SIZE 5
#define NUM_REQUESTS 8
#define MAX_WAIT_TIME 9

//SEMAPHORE
std::mutex mutex; //this locks the queue
std::condition_variable cv_wait_if_empty; //this makes consumer wait until there is something to grab
std::condition_variable cv_wait_if_full; //this makes producer wait until there is room to add to the queue

//QUEUE FUNCTIONS
class Request {
private:
    int id, time;
public:
    Request(int id, int time) : id(id), time(time) {};
    int getId() { return id; }
    int getTime() { return time; }
};

class Queue {
private:
    int head;
    int tail;
    int size;
    Request items[SIZE];
public:
    Queue() : head(-1), tail(-1), size(SIZE) {};

//isEmpty
//if the queue is empty return 1(tr
    bool isEmpty() {
        if(head == -1){
            return true;
        } else{
            return false;
        }
    }

//isFull
//if the queue is full return 1(true)
    bool isFull() {
        if ((head == tail + 1) || (head == 0 && tail == size- 1)) {
            return true;
        }else{return false;}
    }

    void enqueue(Request message) {
        if(isFull()){
            std::cout <<"queue full (enqueue)\n";
        } else{
            if(head == -1){
                head = 0;
            }
            tail = (tail  + 1)% size;
            items[tail] = message;
            std::cout <<"Insert values id " << message.getId() << " and execution time " << message.getTime() << std::endl;
        }
    }

    Request dequeue() {
        Request return_values;
        if(isEmpty()){
            std::cout <<"empty queue\n";
        } else{
            return_values = items[head];
            std::cout <<"we dequeuing these values: " << return_values.getId() << ", " << return_values.getTime() << std::endl;
            if(head == tail){
                head = -1;
                tail = -1;
            } else{
                head = (head + 1) % size;
            }
            std::cout <<"dequeue\n";
            return return_values;
        }
    }

    void display() {
        if(isEmpty()){
            std::cout <<"queue empty\n";
        } else{
            for(int i = head; i != tail; i = (i+1) %size){
                std::cout <<"the id is: " << items[i].getId() << ", the execution time is: " << items[i].getTime() << std::endl;
            }
            std::cout <<"the id is: " << items[tail].getId() << ", the execution time is: " << items[tail].getTime() << std::endl;
        }
    }
};

//delay function
//wait for n seconds;
void delay(int wait_time){
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));

}

//PRODUCER CONSUMER

void producer(Queue *q){
    //pargs *producer_pargs = (pargs*)Pargs;
    int ID = 0;
    int wait;

    int randomTimeProducer;

    std::cout <<"change\n";

     //produce n amount of requests
    for(int i = 0; i < NUM_REQUESTS; i++){
        //wait for random seconds
        wait = 1;
        if(wait == 1){
            randomTimeProducer = rand() %MAX_WAIT_TIME;
            std::cout <<"producer is sleeping for " << randomTimeProducer << " seconds\n";
            delay(randomTimeProducer);
        }
        
        //semaphore for producer
        std::unique_lock<std::mutex> lck(mutex);
        cv_wait_if_full.wait(lck);
        //while the queue is not full -> add a value to the queue
        //lock the queue from being read
        ID++; //update id sequentially
        int randomTimeConsumer = rand() %(MAX_WAIT_TIME *2);
        q->enqueue(Request(ID, randomTimeConsumer));
        //display(q);

        lck.unlock(); //release lock on updating the
    
        cv_wait_if_empty.notify_one(); //let consumer know there is a request available
    }
    //give consumers a chance to finish consuming
    while(1){
        std::this_thread::yield();
    }
}

 void consumer(Queue *q){
    
    //values from queue
    Request values;

    //Timer
    auto con_start = std::chrono::high_resolution_clock::now();
    auto con_finish = std::chrono::high_resolution_clock::now();
    int elapsed;
    long time_wait;
    //each thread will run its own version of consumer, so busy is a local variable to that thread
    //try to consume //this is just to ensure all threads have a chance to continue checking for something in the queue (could have been more elegant)
    for(int i = 0; i < (NUM_REQUESTS); i++){
    bool busy = false;
        //wait until something to consume
        std::unique_lock<std::mutex> lck(mutex);
        cv_wait_if_empty.wait(lck);
        //if(isEmpty(q) == 0){
        //lock and aquire data from the queue
        busy = true;
        values = q->dequeue();
        //std::cout <<"Consumer " << std::this_thread::get_id() << " assigned request " << values.getId() << ", processing request for the next " << values.getTime() << " seconds\n";
        //display(q);

        lck.unlock();
        cv_wait_if_full.notify_one();

        if(busy == true){
            // current time is CURRENT-TIME
            // Consumer i: completed request ID n at time CURRENT-TIME
            std::cout <<"the current time is: " << std::chrono::high_resolution_clock::now() << std::endl;
            delay(values.getTime());
            std::cout <<"Consumer " << std::this_thread::get_id() << ": completed request " << values.getId() << " at " << std::chrono::high_resolution_clock::now() << std::endl;
            busy = false;
        }
    //need to go to top again
    }
}


//driver code

int main(int argc, char* argv[])
  
  std::thread thread[NUM_THREADS];
  
  return EXIT_SUCCESS;
}
