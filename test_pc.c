#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"
#include "umutex.h"

#define N 8
static int buf[N], head, tail, count;
static umutex_t mu;

static void producer(void *arg)
 {
  int id = (int)arg;
  for (int i = 0; i < 200; i++) {
while(1) {
    mutex_lock(&mu);
    if(count < N) break;
mutex_unlock(&mu);
thread_yield();
}
      buf[tail] = id*1000 + i;
      tail = (tail+1) % N;
      count++;

    mutex_unlock(&mu);
    thread_yield();
  }
}

static void consumer(void *arg){
  (void)arg;
  int got = 0;
int x = -1;
  while(got < 400){
    mutex_lock(&mu);
    if(count > 0){
       x = buf[head];
      head = (head + 1) % N;
      count--;
      got++;
      if(got % 100 == 0) {
 printf(1, "consumer got %d items (last=%d)\n", got, x);
    }
    mutex_unlock(&mu);
    thread_yield();
  }
}
printf(1, "test_pc: done\n");
exit();

}


int main(void){
  thread_init();
  mutex_init(&mu);

  thread_create(producer, (void*)100);
thread_create(producer, (void*)200);
thread_create(consumer, 0);



for(;;)
thread_yield();
  exit();
}
