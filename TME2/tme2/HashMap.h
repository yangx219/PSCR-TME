#ifndef SRC_HASHMAP_H_
#define SRC_HASHMAP_H_

#include <cstddef>
#include <ostream>
#include <forward_list>
#include <vector>
#include <algorithm> // 添加这个头文件

namespace pr {

template <typename K, typename V>
class HashMap {
public:
    class Entry {
    public:
        K key;
        V value;
        Entry(const K &k, const V& v) : key(k), value(v) {}
    };

private:
    typedef std::vector<std::forward_list<Entry> > buckets_t;
    buckets_t buckets;
    size_t sz;

public:
    HashMap(size_t size) : buckets(size), sz(0) {}

    V* get(const K & key) {
        size_t h = std::hash<K>()(key);
        size_t target = h % buckets.size();
        for (Entry & ent : buckets[target]) {
            if (ent.key == key) {
                return & ent.value;
            }
        }
        return nullptr;
    }

    std::pair<typename std::forward_list<Entry>::iterator, bool> put(const std::pair<K, V>& kvPair) {
    const K& key = kvPair.first;
    const V& value = kvPair.second;

    size_t h = std::hash<K>()(key);
    size_t target = h % buckets.size();
    for (Entry& ent : buckets[target]) {
        if (ent.key == key) {
            ent.value = value; // 更新现有值
            return std::make_pair(buckets[target].before_begin(), false);
        }
    }
    sz++;
    buckets[target].emplace_front(Entry(key, value)); // 插入新的键值对
    return std::make_pair(buckets[target].before_begin(), true);
}




    size_t size() const { return sz; }

    void grow() {
        HashMap other = HashMap(buckets.size() * 2);
        for (auto & list : buckets) {
            for (auto & entry : list) {
                other.put(entry.key, entry.value);
            }
        }
        buckets = std::move(other.buckets);
    }

    typename std::forward_list<Entry>::iterator begin() {
        for (size_t i = 0; i < buckets.size(); ++i) {
            if (!buckets[i].empty()) {
                return buckets[i].begin();
            }
        }
        return buckets.back().end(); // Return the end of the last bucket if all buckets are empty
    }

    typename std::forward_list<Entry>::iterator end() {
        return buckets.back().end();
    }


    typename std::forward_list<Entry>::iterator find(const K & key) {
        size_t h = std::hash<K>()(key);
        size_t target = h % buckets.size();
        for (auto it = buckets[target].begin(); it != buckets[target].end(); ++it) {
            if (it->key == key) {
                return it;
            }
        }
        return buckets[target].end();
    }
    };
    } /* namespace pr */
#endif /* SRC_HASH_H_ */
