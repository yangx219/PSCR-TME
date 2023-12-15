#include <thread>
#include <iostream>

#include "Banque.h"
#include <vector>
#include <thread>
#include "Compte.h"
//g++ -std=c++11 Banque.cpp Compte.cpp main.cpp -o SG
//./SG
using namespace std;
using namespace pr;

const int NB_THREAD = 20;
const int K = 20;//账户数
const int SOLDEINITIAL = 1000;

void transferts_aleatoires(pr::Banque *banque) {
	

	int compteur = 10;

	for (; compteur > 0; --compteur) {
		// On choisit deux comptes au hasard
		auto i = ::rand() % banque->size();
		auto j = ::rand() % banque->size();

		// On part du principe qu'on peut se faire des transferts à soi-même
		auto m = ::rand() % 100;

		// cout << m << endl;
		// On transfert une somme aléatoire entre 0 et 100
		cout<<"\ntransfert de "<<i<<" vers "<<j<<" de "<<m<<endl;
		banque->transfert(i,j,m);

		// On s'endort pendant une durée aléatoire entre 0 et 200ms
		auto r = ::rand() % 20;

		std::this_thread::sleep_for(std::chrono::milliseconds(r));
	}
}
//计算银行的总资产
//calcule le bilan de la banque
int bilan_naif(pr::Banque *banque)
{
	int bilan = 0;

	for (int i = 0; i < K ; ++i) {
		 bilan += banque->get_solde(i);
	 }

	 return bilan;
 }

int main()
{
	// On fixe la graine
	::srand(::time(nullptr));

	// On créé une instance de banque
	pr::Banque SG(K, SOLDEINITIAL);
	// On vérifie que le comptable est content
	// cout << SG.comptabiliser(K * SOLDEINITIAL) << endl;
	//calculer le bilan de la banque
	cout << "le bilan de la banque est : " << endl;
	cout << bilan_naif(&SG) << endl;
	// On vérifie qu'un compte random est bien à la bonne valeur
	cout << "Solde du compte 12 :"  << endl;
	cout << SG.get_solde(12) << endl;


	vector<thread> threads;

	// TODO : creer des threads qui font ce qui est demandé
	threads.reserve(NB_THREAD);
	for (int i = 0; i < NB_THREAD; ++i) {
		threads.push_back(thread(transferts_aleatoires, &SG));
	}
	// Threads créés, on attend qu'ils se terminent
	for (auto & t : threads) {
		t.join();
	}

	// Est-ce que le comptable est content ?
	// cout << SG.comptabiliser(K * SOLDEINITIAL) << endl;
	cout << "le bilan de la banque est : " << endl;
	cout << bilan_naif(&SG) << endl;
	// Est-ce que notre fonction a vraiment fait qqch ?
	cout << "Solde du compte 12 :"  << endl;
	cout << SG.get_solde(12) << endl;
	

	// TODO : tester solde = NB_THREAD * JP
	return 0;
}