#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

#define MAX_THREADS 16
#define STACK_SIZE 4096


enum tstate { T_FREE=0, T_RUNNABLE, T_RUNNING, T_ZOMBIE };

struct thread {
tid_t tid;
enum tstate state;

void (*fn)(void*);
void *arg;

void *stack;
struct context *ctx;
};

extern void  uswtch(struct context **old, struct context *new);

static struct thread threads[MAX_THREADS];
static struct thread *cur = 0;
static int next_tid = 1;

static void thread_trampoline(void);
static struct thread* pick_next(void);

void
thread_init(void)
{
for(int i = 0; i < MAX_THREADS; i++){
threads[i].tid = 0;
threads[i].state = T_FREE;
threads[i].fn = 0;
threads[i].arg = 0;
threads[i].stack = 0;
threads[i].ctx = 0;
}

threads[0].tid = 0;
threads[0].state = T_RUNNING;
cur = &threads[0];
}

static void
thread_trampoline(void)
{
if(cur && cur->fn){
cur->fn(cur->arg);
}

cur->state = T_ZOMBIE;
thread_yield();

for(;;)
;
}

tid_t
thread_create(void (*fn)(void*), void *arg)
{
int i;
for(i = 1; i < MAX_THREADS; i++){
if(threads[i].state == T_FREE)
break;
}
if(i == MAX_THREADS)
return -1;

struct thread *t = &threads[i];
t->tid = next_tid++;
t->state = T_RUNNABLE;
t->fn = fn;
t->arg = arg;

t->stack = malloc(STACK_SIZE);
if(t->stack == 0){
t->state = T_FREE;
return -1;
}

uint sp = (uint)t->stack + STACK_SIZE;

sp -= sizeof(struct context);
t->ctx = (struct context*)sp;

t->ctx->edi = 0;
t->ctx->esi = 0;
t->ctx->ebx = 0;
t->ctx->ebp = 0;
t->ctx->eip = (uint)thread_trampoline;
return t->tid;
}

static struct thread*
pick_next(void)
{

if(cur == 0)
return 0;

int start = (cur - threads + 1) % MAX_THREADS;
for(int off = 0; off < MAX_THREADS; off++){
int idx = (start + off) % MAX_THREADS;
if(threads[idx].state == T_RUNNABLE)
return &threads[idx];
}
return 0;
}
void
thread_yield(void)
{
struct thread *next = pick_next();
if(next == 0)
return;

struct thread *prev = cur;

if(prev->state == T_RUNNING)
prev->state = T_RUNNABLE;

next->state = T_RUNNING;
cur = next;

uswtch(&prev->ctx, next->ctx);
}

int
thread_join(tid_t tid)
{
if(tid <= 0)
return -1;

for(;;){
for(int i =1; i < MAX_THREADS; i++){
if(threads[i].state != T_FREE && threads[i].tid == tid){
if(threads[i].state == T_ZOMBIE){

if(threads[i].stack)
free(threads[i].stack);

threads[i].tid = 0;
threads[i].state = T_FREE;
threads[i].fn = 0;
threads[i].arg = 0;
threads[i].stack = 0;
threads[i].ctx = 0;
return 0;
}

thread_yield();
break;
}
}
int found =0;
for(int j =1; j < MAX_THREADS; j++){
if(threads[j].state != T_FREE && threads[j].tid == tid){
found = 1;
break;
}
}
if(!found)
return -1;
}
}
