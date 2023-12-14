
#ifndef PSCR_TME_BARRIER_H
#define PSCR_TME_BARRIER_H


#include <mutex>
#include <condition_variable>
/*// A barrier is a synchronization mechanism that allows a set of threads 
to all wait for each other to reach a common point.
*/
class Barrier {
     int counter;
     const int MAX;
     std::mutex m;
     std::condition_variable cv;
     public :
     Barrier(int max) : counter(0), MAX(max) {}

     void done() {
         std::unique_lock<std::mutex> l(m);
         counter++;
         if (counter == MAX)
             cv.notify_all();
     }
      void waitFor() {
          std::unique_lock<std::mutex> l(m);
          while (counter != MAX) {
              cv.wait(l);
              }
          }
      };


#endif //PSCR_TME_BARRIER_H