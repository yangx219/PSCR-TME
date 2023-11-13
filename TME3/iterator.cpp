#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <vector>
#include <utility>
#include <forward_list>

using namespace std;

template <typename K, typename V>
class HashMap {
public:
    struct Entry {
        const K key;
        V value;
        Entry(const K& key, const V& value) : key(key), value(value) {}
    };

private:
    typedef vector<forward_list<Entry>> buckets_t;
    buckets_t buckets;
    size_t nb_stored_values;

public:
    HashMap(size_t size) {
        buckets.resize(size);
        nb_stored_values = 0;
    }

    HashMap() {
        buckets.resize(256);
        nb_stored_values = 0;
    }

    size_t nb_buckets() const {
        return buckets.size();
    }

    size_t size() const {
        return nb_stored_values;
    }

    void grow() {
        size_t former_size = buckets.size();
        HashMap nouvelle_map(2 * former_size);

        for (auto &bucket : buckets) {
            for (auto &entry : bucket) {
                nouvelle_map.put(entry.key, entry.value);
            }
        }

        *this = move(nouvelle_map);
    }

    V* get(const K& key) {
        size_t h = hash<K>()(key) % buckets.size();
        for (Entry& entry : buckets[h]) {
            if (entry.key == key) {
                return &entry.value;
            }
        }
        return nullptr;
    }

    bool put(const K& key, const V& value) {
        size_t h = hash<K>()(key) % buckets.size();
        for (Entry& entry : buckets[h]) {
            if (entry.key == key) {
                entry.value = value;
                return true;
            }
        }

        if (static_cast<double>(size() + 1) / nb_buckets() > 0.8) {
            grow();
        }
        buckets[h].emplace_front(key, value);
        nb_stored_values++;
        return false;
    }

    bool increment(const K& key) {
        size_t h = hash<K>()(key) % buckets.size();
        for (Entry& entry : buckets[h]) {
            if (entry.key == key) {
                entry.value++;
                return true;
            }
        }

        if (static_cast<double>(size() + 1) / nb_buckets() > 0.8) {
            grow();
        }
        buckets[h].emplace_front(key, 1);
        nb_stored_values++;
        return false;
    }

    bool del(const K& key) {
        size_t h = hash<K>()(key) % buckets.size();
        auto &bucket = buckets[h];
        auto it = bucket.before_begin();
        for (auto next_it = bucket.begin(); next_it != bucket.end(); next_it++) {
            if (next_it->key == key) {
                bucket.erase_after(it);
                nb_stored_values--;
                return true;
            }
            it = next_it;
        }
        return false;
    }

    // Iterators
    class iterator {
        typename buckets_t::iterator vit;
        typename forward_list<Entry>::iterator lit;
        buckets_t *buckets;

    public:
        iterator(buckets_t &buckets, bool end = false) : buckets(&buckets) {
            if (end) {
                vit = this->buckets->end();
            } else {
                vit = this->buckets->begin();
                while (vit != this->buckets->end() && vit->empty()) {
                    ++vit;
                }
                if (vit != this->buckets->end()) {
                    lit = vit->begin();
                }
            }
        }

        iterator &operator++() {
            ++lit;
            if (lit == vit->end()) {
                do {
                    ++vit;
                } while (vit != buckets->end() && vit->empty());

                if (vit != buckets->end()) {
                    lit = vit->begin();
                }
            }
            return *this;
        }

        bool operator!=(const iterator &other) const {
            return vit != other.vit || (vit != buckets->end() && lit != other.lit);
        }

        Entry &operator*() const {
            return *lit;
        }
    };

    iterator begin() {
        return iterator(buckets);
    }

    iterator end() {
        return iterator(buckets, true);
    }

    size_t count(iterator begin, iterator end) {
        size_t count = 0;
        for (auto it = begin; it != end; ++it) {
            count++;
        }
        return count;
    }
};

int main() {
    // ... Rest of your main function code goes here
}

