#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <vector>
#include <algorithm>
#include "HashMap.h"

using namespace std;
using namespace pr;

void printFrequency (const string & word, HashMap<string,int> & map) { 
	auto it = map.find(word); 
	//如果找到满足条件的元素，it 将指向该元素；如果未找到满足条件的元素，it 将指向范围的末尾
	if (it != map.end()) {  
		std::cout << "Frequency : " << it->key << ":" << it->value << endl; 
		} else {
			 std::cout << "Word :"<< word << " not found." << endl; 
		}
}
int main() {
    //HashMap<std::string, int> map(2 << 18); // 256 k word capacity
    HashMap<std::string, int> map(256 * 1024); // 256 K word capacity
    ifstream input = ifstream("./WarAndPeace.txt");
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::cout << "Parsing War and Peace" << endl;
    std::string s;
    regex re(R"([^a-zA-Z])");
      printf("test1\n");
      
    /***/while (input >> s) {
          printf("test2\n");
        s = regex_replace(s, re, "");
          printf("test3\n");
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        //auto ret = map.insert({s, 1});
        auto ret = map.put({s, 1});

        if (!ret.second) {
            (*ret.first).value++;
        }
    }
    input.close();

    printf("test1\n");

    
    std::cout << "Finished Parsing War and Peace" << endl;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Parsing with hash took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms.\n";
    start = std::chrono::steady_clock::now();
    std::cout << "Parsing War and Peace" << endl;
    vector<pair<string, int> > counts;
    counts.reserve(5000);
    input = ifstream("./WarAndPeace.txt");
    while (input >> s) {
        s = regex_replace(s, re, "");
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        auto it = counts.begin(); 
		while (it != counts.end()) { 
			if (it->first == s) { 
				break; 
			} 
			++it;
        } 
        if (it != counts.end()) { 
            it->second++; 
        } else { 
                counts.push_back(make_pair(s,1));
        } 
    }	 
    input.close();
    end = std::chrono::steady_clock::now();
    std::cout << "Parsing with pair<string,int> took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms.\n";

    printFrequency("war", map);
    printFrequency("peace", map);
    printFrequency("toto", map);

    start = std::chrono::steady_clock::now();
    std::cout << "Most frequent" << endl;
    std::vector<pr::HashMap<std::string, int>::Entry * > entries;
    entries.reserve(map.size());
    for (auto it = map.begin(); it != map.end(); ++it) {
        entries.push_back(&(*it));
    }
    std::sort(entries.begin(), entries.end(), [](auto a, auto b) { return a->value > b->value; });
    int line = 0;
    for (const auto & val : entries) {
        std::cout << val->key << "->" << val->value << " ,";
        if (++line % 10 == 0) {
            std::cout << endl;
        }
        if (line == 100) {
            break;
        }
    }
    std::cout << std::endl;

    end = std::chrono::steady_clock::now();
    std::cout << "Sorting by frequency took "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << "us.\n";
    return 0;
}
