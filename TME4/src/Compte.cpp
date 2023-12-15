#include "Compte.h"

using namespace std;

namespace pr {
void Compte::crediter (unsigned int val) {
	// unique_lock<mutex> g(m);
	lock_guard<recursive_mutex> g(m);
	solde+=val ;
}
bool Compte::debiter (unsigned int val) {
	// unique_lock<mutex> g(m);
	lock_guard<recursive_mutex> g(m);
	bool doit = solde >= val;
	if (doit) {
		solde-=val ;
	}
	return doit;
}
int Compte::getSolde() const  {
	// unique_lock<mutex> g(m);
	lock_guard<recursive_mutex> g(m);
	return solde;
}
// NB : vector exige Copyable, mais mutex ne l'est pas...
Compte::Compte(const Compte & other) {
	other.m.lock();
	solde = other.solde;
	other.m.unlock();
}


// Acquiert获得 le verrou锁. Bloque si le verrou est déjà pris par un autre thread.
void Compte::lock() const {
	m.lock();
}

// Libère le verrou. Doit être appelé après lock() pour éviter le blocage d'autres threads.
void Compte::unlock() const {
	m.unlock();
}

// Tente d'acquérir le verrou sans bloquer. Renvoie true si le verrou est acquis.
bool Compte::try_lock() const {
	return m.try_lock();
}

}