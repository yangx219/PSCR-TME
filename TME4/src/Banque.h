#pragma once

#include "Compte.h"
#include <vector>

namespace pr {
class Banque {
	typedef std::vector<Compte> comptes_t;
	// pour la version avec mutex
	mutable std::mutex  m;
	comptes_t comptes;
public :
	Banque (size_t ncomptes, size_t solde) : comptes (ncomptes, Compte(solde)){
	}
	void transfert(size_t deb, size_t cred, unsigned int val) ;
	size_t size() const ;
	bool comptabiliser (int attendu) const ;
	int get_solde(size_t compte) ;

	/*****************************/
	void transfert_deadlock(size_t deb, size_t cred, unsigned int val);
    void transfert_multilock(size_t deb, size_t cred, unsigned int val);
    void transfert_manuallock(size_t deb, size_t cred, unsigned int val);
};

}
