#include "Queue.h"

namespace fifoQueue {

    fifoQueue::Queue::Queue() 
    {
        mQueueHead = 0;
        mQueueTail = 0;
        mNumItems = 0;
        /// Initialize the queue
        for ( int i = 0; i < kQueueSize; i++ )
        {
            mQueue[i]= 0x00;
        }        
    }

    boolean fifoQueue::Queue::push(char* data )
    {
        // Copy the data to the queue
        for (size_t i = 0; i < QUEUE_ITEM_SIZE; i++)
        {
            mQueue[mQueueTail] = data[i];
            if(data[i] == '\0')
            {
                break;
            }
        }
        mQueueTail = (mQueueTail + QUEUE_ITEM_SIZE) % kQueueSize;

        if ( mNumItems > kQueueSize )
        {
            overFlowValue = true;
        }
        else
        {
            mNumItems++;
        }
        return true;
    }

    boolean fifoQueue::Queue::pop(char* data)
    {
        if ( isEmpty() )
        {
            return false;
        }
        // Copy the data from the queue
        for (size_t i = 0; i < QUEUE_ITEM_SIZE; i++)
        {
            data[i] = mQueue[mQueueHead];
            if(data[i] == '\0')
            {
                break;
            }
        }
        mQueueHead = (mQueueHead + QUEUE_ITEM_SIZE) % kQueueSize;
        overFlowValue = false;
        mNumItems--;
        return true;
    }

    boolean fifoQueue::Queue::isEmpty(){
    return mNumItems == 0;
    }

    boolean fifoQueue::Queue::overFlow(){
        return overFlowValue;
    }

    int fifoQueue::Queue::count(){
        return mNumItems;
    }
}