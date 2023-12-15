#include "Banque.h"

#include <iostream>

using namespace std;

namespace pr {
//使用 unique_lock<mutex> 来确保在转账操作期间，其他线程不能访问或修改这些账户
// Verrouille l'opération de transfert pour assurer la sécurité des transactions entre comptes
	void Banque::transfert(size_t deb, size_t cred, unsigned int val) {
		unique_lock<mutex> lock(m);
		Compte & debiteur = comptes[deb];
		Compte & crediteur = comptes[cred];
		if (debiteur.debiter(val)) {
			crediteur.crediter(val);
		}
	}

/**********************************************************************************/
// Risque de deadlock en verrouillant successivement deux comptes
	void Banque::transfert_deadlock(size_t deb, size_t cred, unsigned int val) {
        Compte & debiteur = comptes[deb];
        Compte & crediteur = comptes[cred];
		// On verrouille les deux comptes
        debiteur.lock();
        crediteur.lock();
        if (debiteur.debiter(val)) {
            crediteur.crediter(val);
        }
		// On déverrouille les deux comptes
        debiteur.unlock();
        crediteur.unlock();
    }

// Utilise std::lock pour verrouiller simultanément deux comptes et éviter les deadlocks
//同时锁定两个账户，避免死锁
    void Banque::transfert_multilock(size_t deb, size_t cred, unsigned int val) {
        Compte & debiteur = comptes[deb];
        Compte & crediteur = comptes[cred];
        lock(debiteur,crediteur);
        if (debiteur.debiter(val)) {
            crediteur.crediter(val);
        }
        debiteur.unlock();
        crediteur.unlock();
    }

//根据账户编号的顺序来锁定，以避免死锁。
// Verrouille les comptes dans un ordre spécifique pour prévenir les deadlocks
    void Banque::transfert_manuallock(size_t deb, size_t cred, unsigned int val) {
        Compte & debiteur = comptes[deb];
        Compte & crediteur = comptes[cred];

        if(deb < cred){
            debiteur.lock();
            crediteur.lock();
        }else{
            crediteur.lock();
            debiteur.lock();
        }
        if (debiteur.debiter(val)) {
            crediteur.crediter(val);
        }
        debiteur.unlock();
        crediteur.unlock();
    }

/**********************************************************************************/

//// Verrouille l'opération pour garantir la cohérence des données lors de la récupération de la taille
	size_t Banque::size() const {
		//mutex
		unique_lock<mutex> lock(m);
		return comptes.size();
	}

	//在遍历账户时，对每个账户进行锁定和解锁，保护整个操作的一致性。
	// Verrouille chaque compte lors du calcul du bilan pour assurer la cohérence des données
	bool Banque::comptabiliser(int attendu) const {
		//mutex
		unique_lock<mutex> lock(m);
		int bilan = 0;
		int id = 0;
		
		for (const auto & compte : comptes) {
			compte.lock();
			if (compte.getSolde() < 0) {
				cout << "Compte " << id << " en négatif : " << compte.getSolde() << endl;
			}
			bilan += compte.getSolde();
			id++;
		}
		for(const auto & c : comptes){
        c.unlock();
    	}
		if (bilan != attendu) {
			cout << "Bilan comptable faux : attendu " << attendu << " obtenu : " << bilan << endl;
		}
		return bilan == attendu;
	}

//
	int Banque::get_solde(size_t compte) {
		unique_lock<mutex> lock(m);
		return comptes[compte].getSolde();
	}
}