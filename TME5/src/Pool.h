#pragma once

#include "Queue.h"
#include "Job.h"
#include <vector>
#include <thread>

using namespace std;
namespace pr {
// Un travailleur de pool qui exécute des travaux à partir d'une file d'attente.
    void poolWorker(Queue<Job> * queue) {
        while (true) {
			// On récupère un travail dans la file d'attente
            Job * j = queue->pop();
			// Si la file d'attente est vide et qu'on est en mode non bloquant, on s'arrête
            if (j == nullptr) {
                return;
            }
            j -> run();
            delete j;
        }

    }

class Pool {
	Queue<Job> queue;
	vector<std::thread> threads;
public:
// On crée un pool de travailleurs avec une file d'attente de taille qsize
    Pool(int qsize) : queue(qsize) {}
// On démarre le pool avec nbthread travailleurs
	void start (int nbthread){
        threads.reserve(nbthread);
        for (int i=0 ; i < nbthread ; i++) {
			// On crée un thread qui exécute la fonction poolWorker
             threads.emplace_back(poolWorker, &queue);
             }
    }
// On soumet un travail à la file d'attente
	void submit (Job * job){queue.push(job);} ;
	void stop() {
		// on met le flag isBlocking à false pour que les threads se terminent
        queue.setBlocking(false);
		// On réveille tous les threads qui sont bloqués sur la file d'attente
         for (auto & t : threads) {
             t.join();
             }
		
         threads.clear();
    };
	~Pool(){stop();} ;
};

}