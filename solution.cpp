
// Process Block Structure
struct Process{
    int process_pid;
    Process* next;
}

// FIFO Queue implementation of Process nodes
struct Queue{
    Process* qFront, qRear;
    
    int push(int pid){  
        Process* pro = new Process();
        pro->process_pid = pid;
        if(qRear == NULL){
            qFront = qRear = pro;
        }
        else{
            qRear->next = pro;
            qRear = pro;
        }
    }

    int pop(){
        if(qFront == NULL){
            return -1;          // Underflow condition
        }
        int pid = qFront->process_pid;
        qFront = qFront->next;

        if(qFront==NULL) qRear = NULL;

        return pid;
    }
}

// This is a FIFO semaphore
struct Semaphore{
    int val = 1;                // value
    Queue* q = new Queue();     // this is a FIFO Queue and is defined above..

    void wait(int pid){
        // pid denotes process id

        val--;
        if(val < 0){
            q->push(pid);
            block(pid);         // this will block that particular process, using the system call and will be sent to waiting queue, unless the wake() is called.  
        }
    }

    void signal(){
        val++;
        if(val<=0){
            int processId = q->pop();
            wake(processId);    // to wakeup the process with the given process id
        }
    }
}

// INITIALIZATON

int reader_count = 0;                   // to indicate how many readers are executing critical section
Semaphore next_turn = new Semaphore();  // semaphore maintaining the order in which the reader and writer are requesting access to critical section
Semaphore token = new Semaphore();        // required semaphore to access the critical section
Semaphore r_change = new Semaphore();   // semaphore to change the reader_count

// Readers Process
do{
/* ENTRY SECTION */
        next_turn->wait(process_id);            // process waiting for its turn to get executed
        r_change->wait(process_id);             // process requesting access to change reader_count
        reader_count++;                         //updating the number of readers trying to access critical section 
        if(reader_count==1)                     // if this is the first reader then request access to critical section
            token->wait();                      //requesting  access to the critical section for readers
        
        turn->signal();                           //releasing turn so that the next reader or writer can take the token and can be serviced
        r_change->signal();                       //release access to the reader_count
/* CRITICAL SECTION */
       
/* EXIT SECTION */
        r_change->wait(process_id)              //requesting access to change reader_count         
        reader_count--;                         //a reader has finished executing critical section so reader_count will decrease by 1
        if(reader_count==0)                     //if all the reader have finished executing their critical section
            token->signal();                    //releasing access to critical section for next reader or writer
        r_change->signal();                     //release access to the reader_count  
       
/* REMAINDER SECTION */
       
}while(true);


// Writer's Process

do{
/* ENTRY SECTION */
      next_turn->wait(process_id);              //waiting for its turn to get executed
      token->wait(process_id);                  //requesting  access to the critical section
      turn->signal(process_id);                 //releasing turn so that the next reader or writer can take the token and can be serviced

/* CRITICAL SECTION */

/* EXIT SECTION */
      token->signal()                       //releasing access to critical section for next reader or writer

/* REMAINDER SECTION */

}while(true);