#include <ctime>
#include <cstdlib>
#include "rsleep.h"

void randsleep() {
  int r = rand();
  double ratio = (double)r / (double) RAND_MAX;
  struct timespec tosleep;
  tosleep.tv_sec =0;
  // 300 millions de ns = 0.3 secondes
  tosleep.tv_nsec = 300000000 + ratio*700000000;
  struct timespec remain;
  while ( nanosleep(&tosleep, &remain) != 0) {
    tosleep = remain;
  }
}

/*
void randsleep() {
  int r = rand(); // 生成一个基本随机数
  double ratio = (double)r / (double)RAND_MAX; // 将随机数转换为0到1之间的浮点数
  struct timespec tosleep; // 定义一个timespec结构，用于指定睡眠时间
  tosleep.tv_sec = 0; // 设置秒数为0，因为睡眠时间小于1秒
  // 300000000纳秒等于0.3秒。通过添加比例*ratio乘以700000000纳秒（最多0.7秒），
  // 生成0.3秒至1秒的随机时间。
  tosleep.tv_nsec = 300000000 + ratio*700000000;
  struct timespec remain; // 用于存储未睡眠完的时间
  // nanosleep尝试睡眠指定的时间。如果睡眠被信号中断，剩余的睡眠时间会被写入remain
  // 并返回-1。这时，while循环会继续，使用remain中的剩余时间再次调用nanosleep。
  while (nanosleep(&tosleep, &remain) != 0) {
    tosleep = remain; // 更新tosleep为剩余的时间，准备再次尝试睡眠
  }
}
*/