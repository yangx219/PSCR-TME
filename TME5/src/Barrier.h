
#ifndef PSCR_TME_BARRIER_H
#define PSCR_TME_BARRIER_H


#include <mutex>
#include <condition_variable>
/*// A barrier is a synchronization mechanism that allows a set of threads 
to all wait for each other to reach a common point.
*/
//同步机制，用于协调多个线程的执行。它允许一组线程在达到某个共同点时都等待彼此
//确保所有线程都达到某个执行阶段后才继续执行
class Barrier {
    // 用于跟踪已经到达屏障的线程数量，pour suivre le nombre de threads ayant atteint la barrière
     int counter;
     //// 屏障所需等待的最大线程数量。le nombre maximal de threads à attendre pour la barrière
     const int MAX;
     std::mutex m;
     //用于阻塞线程直到所有线程都到达屏障的条件变量。condition variable pour bloquer les threads jusqu'à ce que tous les threads atteignent la barrière
     std::condition_variable cv;
     public :

     //initialise la barrière avec le nombre maximal de threads à attendre
     Barrier(int max) : counter(0), MAX(max) {}

     void done() {
         std::unique_lock<std::mutex> l(m);
         counter++;
         //唤醒所有等待线程。notify_all() 会唤醒所有等待线程，而 notify_one() 只会唤醒一个等待线程
         if (counter == MAX)
             cv.notify_all();
     }
     // waitFor 方法使线程在屏障上等待，直到所有线程都调用了 done 方法
      void waitFor() {
          std::unique_lock<std::mutex> l(m);
          while (counter != MAX) {// 只要没有达到最大线程数，就等待。
              cv.wait(l);
              }
          }
      };


#endif //PSCR_TME_BARRIER_H