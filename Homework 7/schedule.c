#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ucontext.h>

struct thread {
    char *rsp;
    ucontext_t context;
    int tid;
    int valid;
};

#define MAX_THREADS 16
struct thread listThreads[MAX_THREADS] = {0};
struct thread *curThread = NULL;
static int numThreads = 0;
ucontext_t errContext;

void errorfn(void)
{
    printf ("This line shlould not be printed!\n");
}

struct thread *
nextthread (int tid) //returns pointer to the next thread to be run from listThreads stack
{
    int i, nexttid;
    for (i = 1; i <= MAX_THREADS; i++) {
        nexttid = (tid + i) % MAX_THREADS;
        if (listThreads[nexttid].valid) {
            return &listThreads[nexttid];
        }
    }
    return NULL;
}

void schedule (int signal) //schedule called whenever alarm function is called. signal points to SIGALRM signal
{
    struct thread *nextThread = nextthread (curThread->tid);
    struct thread *cur;
    
    if (nextThread == NULL) {
        printf ("no thread to schedule!\n");
        exit (0);
    }
    alarm (1); //alarm() arranges for a SIGALRM signal to be delivered to the calling process in 1 second.
    
    if (nextThread != curThread) {
        
        cur = curThread;
        curThread = nextThread;
        
        swapcontext (&cur->context, &curThread->context); /*The swapcontext() function saves the current context in the structure pointed to by &cur->context, and then activates the context pointed to by &curThread->context. */
        
        
    }
}

void thread1 ()
{
    long long i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 1000000000; j++);
        printf ("thread1 is running\n");
    }
}

void thread2 ()
{
    long long i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 1000000000; j++);
        printf ("thread2 is running\n");
    }
}

void default_handler (void(*fnptr)(), int *valid)
{
    fnptr ();
    valid[0] = 0;
    kill (getpid(), SIGALRM); //The command kill sends the specified signal to the specified process
}


/*
 context - currently available context
 void(*fnptr)()- pointer to thread
 *valid- state of the thread
 */
int create_context (ucontext_t *context, void(*fnptr)(), int *valid)
{
    char *rsp = malloc (4096);
    if (rsp == NULL) {
        return 0;
    }
    getcontext(context); /*The function getcontext() initializes the structure pointed at by 'context' to the       currently active context. Saves current context into 'context'*/
    
    context->uc_stack.ss_sp = (void*)(rsp + 4096); //uc_stack is the stack used by this context, ss_sp - base address of stack
    
    context->uc_stack.ss_size = 4096; //ss_size- number of bytes in stack
    
    context->uc_link = &errContext;      /*uc_link points to the context that will be resumed when the current context terminates (successor context) */
    
    makecontext (context, (void*)default_handler, 2, fnptr, valid); //The makecontext() function modifies the context pointed to by 'context'. The makecontext function sets up an alternate thread of control in ucp, which has previously been initialised using getcontext. When func terminates, control is returned to context.uc_link
    return 1;
}


void create_thread (void(*fnptr)()) //creates a new thread pointed by fnptr
{
    ucontext_t *context;
    
    if (numThreads >= MAX_THREADS) {
        printf ("Unable to create more threads!\n");
        return;
    }
    context = &listThreads[numThreads].context;
    if (!create_context (context, fnptr, &listThreads[numThreads].valid)) //calls create_context
    {
        return;
    }
    listThreads [numThreads].tid = numThreads; //assigns a pid to the new thread
    listThreads [numThreads].valid = 1; //marks new thread state as valid
    numThreads++; //incrementing no. of threads available
}

int main ()
{
    create_context (&errContext, errorfn, NULL); // &errContext points to current context?
    signal (SIGALRM, schedule); //assigns SIGALRM signal to schedule?
    memset (listThreads, 0, sizeof(listThreads)); //converts all elements pointed by listThreads to 0
    
    //2 new threads created
    create_thread (thread1);
    create_thread (thread2);
    
    curThread = &listThreads[0]; //curThread points to the currently running thread (thread1 initially)
    alarm (1); //calls schedule?
    
    setcontext (&curThread->context); //The function setcontext() restores the user context pointed at by 'context'. Execution continues from the point at which the context was stored in 'context'. A successful call does not return.
    return 0;
}
