#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

// Maximum number of items in the queue
const int MAX_QUEUE_SIZE = 10;

class ProducerConsumer
{
public:
    ProducerConsumer() : count(0) {}

    // Function called by producer thread
    void produce(int val)
    {
        // Acquire lock
        std::unique_lock<std::mutex> lock(m);

        // Wait while queue is full
        while (count == MAX_QUEUE_SIZE)
        {
            std::cout << "Queue is full, producer is waiting" << std::endl;
            // Sleep until notified by consumer
            cv.wait(lock);
        }

        // Push item to the queue
        q.push(val);
        count++;

        // Notify consumer
        cv.notify_one();

        // Release lock
        lock.unlock();
    }

    // Function called by consumer thread
    int consume()
    {
        // Acquire lock
        std::unique_lock<std::mutex> lock(m);

        // Wait while queue is empty
        while (count == 0)
        {
            std::cout << "Queue is empty, consumer is waiting" << std::endl;
            // Sleep until notified by producer
            cv.wait(lock);
        }

        // Pop item from the queue
        int val = q.front();
        q.pop();
        count--;

        // Notify producer
        cv.notify_one();

        // Release lock
        lock.unlock();

        return val;
    }

private:
    std::queue<int> q;
    std::mutex m;
    std::condition_variable cv;
    int count;
};

void producer(ProducerConsumer& pc)
{
    for (int i = 0; i < 20; i++)
    {
        std::cout << "Producing: " << i << std::endl;
        pc.produce(i);
    }
}

void consumer(ProducerConsumer& pc)
{
    for (int i = 0; i < 20; i++)
    {
        int val = pc.consume();
        std::cout << "Consuming: " << val << std::endl;
    }
}

int main()
{
    ProducerConsumer pc;

    std::thread producerThread(producer, std::ref(pc));
    std::thread consumerThread(consumer, std::ref(pc));

    producerThread.join();
    consumerThread.join();

    return 0;
}
