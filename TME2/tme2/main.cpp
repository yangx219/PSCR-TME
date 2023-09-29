#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>


int main () {
	using namespace std;
	using namespace std::chrono;

	ifstream input = ifstream("./WarAndPeace.txt");

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

	size_t nombre_lu = 0;
	// prochain mot lu
	string word;

	// Vecteur de paires (mot, nombre d'occurrences)
	vector<pair<string, int>> wordOccurrences;  
	
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	regex re( R"([^a-zA-Z])");

	while (input >> word) {
		// élimine la ponctuation et les caractères spéciaux
		word = regex_replace ( word, re, "");
		// passe en lowercase
		transform(word.begin(),word.end(),word.begin(),::tolower);
		
		// Recherche si le mot existe déjà dans le vecteur wordOccurrences
        auto it = find_if(wordOccurrences.begin(), wordOccurrences.end(),
                          [&](const pair<string, int>& entry) { return entry.first == word; });

        if (it != wordOccurrences.end()) {
            // Si le mot existe déjà, incrémente son nombre d'occurrences
            it->second++;
        } else {
            // Si le mot est nouveau, l'ajoute au vecteur avec une occurrence de 1
            wordOccurrences.push_back(make_pair(word, 1));
        }
		nombre_lu++;
	}
	input.close();

	cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
    cout << "Parsing took "
              << duration_cast<milliseconds>(end - start).count()
              << "ms.\n";

    // Afficher le nombre d'occurrences des mots spécifiques
    for (const pair<string, int>& entry : wordOccurrences) {
        if (entry.first == "war" || entry.first == "peace" || entry.first == "toto") {
            cout << "Occurrences of '" << entry.first << "': " << entry.second << " times." << endl;
        }
    }

    return 0;
}