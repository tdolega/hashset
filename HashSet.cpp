#include "HashSet.h"

size_t HashSet::nothingHash(const int v) {
    return v;
}

uint64_t HashSet::xorshift(const uint64_t &n, int i) {
    return n ^ (n >> i);
}

size_t HashSet::xorshiftHash(int v) {
    uint64_t p = 0x5555555555555555ull;
    uint64_t c = 17316035218449499591ull;
    return c * xorshift(p * xorshift(v, 32), 32);
}

size_t HashSet::robertJenkinsHash(int v) {
    v = (v + 0x7ed55d16) + (v << 12);
    v = (v ^ 0xc761c23c) ^ (v >> 19);
    v = (v + 0x165667b1) + (v << 5);
    v = (v + 0xd3a2646c) ^ (v << 9);
    v = (v + 0xfd7046c5) + (v << 3);
    v = (v ^ 0xb55a4f09) ^ (v >> 16);
    return v;
}

size_t HashSet::shift32multHash(int v) {
    int con = 0x27d4eb2d;
    v = (v ^ 61) ^ (v >> 16);
    v = v + (v << 3);
    v = v ^ (v >> 4);
    v = v * con;
    v = v ^ (v >> 15);
    return v;
}

inline void HashSet::rehashIfNeeded() {
    if (load_factor() > 0.75) rehash(2 * bucketCount);
}

HashSet::HashSet(size_t size, Hashes set_hash) : t(new Bucket[size]), bucketCount(size) {
    switch (set_hash) {
        case set_nothingHash:
            hash = nothingHash;
            break;
        case set_xorshiftHash:
            hash = xorshiftHash;
            break;
        case set_robertJenkinsHash:
            hash = robertJenkinsHash;
            break;
        case set_shift32multHash:
            hash = shift32multHash;
    }
}

HashSet::HashSet(Hashes set_hash) : HashSet(20, set_hash) {}

HashSet::HashSet(const size_t size, size_t (*hash)(const int)) : t(new Bucket[size]), bucketCount(size), hash(hash) {}

HashSet::HashSet(std::initializer_list<int> il) : HashSet(il.size() * 2) {
    for (int v : il)
        insert(v);
}

HashSet::HashSet(const int* list, size_t listSize) : HashSet(listSize * 2) {
    for (size_t i = 0; i < listSize; i++)
        insert(list[i]);
}

HashSet::~HashSet() {
    delete[] t;
}

void HashSet::makeEmpty() {
    delete[] t;
    size_t newBucketCount = 20;
    t = new Bucket[newBucketCount];
    bucketCount = newBucketCount;
    itemsCount = 0;
}

HashSet::HashSet(const HashSet &o) : t(new Bucket[o.bucketCount]), bucketCount(o.bucketCount), hash(o.hash) {
    for (int v : o)
        insert(v);
}

HashSet::HashSet(HashSet &&o) noexcept: t(o.t), bucketCount(o.bucketCount), hash(o.hash), itemsCount(o.itemsCount) {
    o.t = nullptr;
}

HashSet &HashSet::operator=(const HashSet &o) noexcept {
    if (this != &o) {
        bucketCount = o.bucketCount;
        hash = o.hash;
        delete[] t;
        t = new Bucket[bucketCount];
        for (int v : o)
            insert(v);
    }
    return *this;
}

HashSet &HashSet::operator=(HashSet &&o) noexcept {
    if (this != &o) {
        delete[] t;
        t = o.t;
        o.t = nullptr;
        bucketCount = o.bucketCount;
        itemsCount = o.itemsCount;
        hash = o.hash;
    }
    return *this;
}

inline size_t HashSet::getIdx(const int v) {
    return hash(v) % bucketCount;
}

HashSet &HashSet::insert(const int v, bool noRehash) {
    if (!noRehash) rehashIfNeeded();
    size_t idx = getIdx(v);
    bool inserted = t[idx].insert(v);
    if (inserted) itemsCount++;
    return *this;
}

size_t HashSet::benchTime_insert(const int v) {
    auto t0 = high_resolution_clock::now();
    insert(v);
    auto t1 = high_resolution_clock::now();
    auto dur = duration_cast<nanoseconds>(t1 - t0);
    return dur.count();
}

unsigned HashSet::benchOps_insert(const int v) {
    size_t idx = getIdx(v);
    return t[idx].benchOps_insert(v) + 1;
}

HashSet &HashSet::operator+=(const int v) {
    return insert(v);
}

HashSet &HashSet::erase(const int v) {
    size_t idx = getIdx(v);
    bool result = t[idx].remove(v);
    if (result) itemsCount--;
    return *this;
}

HashSet &HashSet::operator-=(const int v) {
    return erase(v);
}

bool HashSet::contains(const int v) {
    size_t idx = getIdx(v);
    return t[idx].exist(v);
}

size_t HashSet::benchTime_contains(const int v) {
    auto t0 = high_resolution_clock::now();
    contains(v);
    auto t1 = high_resolution_clock::now();
    auto dur = duration_cast<nanoseconds>(t1 - t0);
    return dur.count();
}

unsigned HashSet::benchOps_contains(const int v) {
    size_t idx = getIdx(v);
    return t[idx].benchOps_exist(v) + 1;
}

HashSet &HashSet::rehash(size_t newBucketSize) {
    Bucket *oldT = t;
    unsigned oldS = bucketCount;
    t = new Bucket[newBucketSize];
    bucketCount = newBucketSize;
    itemsCount = 0;
    for(auto it = Iterator(oldT, oldS); it != Iterator(nullptr); ++it)
            insert(*it, true);
    delete[] oldT;
    return *this;
}

inline double HashSet::load_factor() const {
    return (double) itemsCount / bucketCount;
}

size_t HashSet::max_load_factor() const {
    size_t max = 0;
    for (unsigned i = 0; i < bucketCount; i++) {
        size_t count = t[i].getSize();
        if (count > max) max = count;
    }
    return max;
}

inline size_t HashSet::size() const {
    return itemsCount;
}

inline size_t HashSet::bucket_count() const {
    return bucketCount;
}

void HashSet::printDebug() const {
    std::cout <<
    "d> bucketCount: " << bucketCount <<
    ", itemsCount: " << itemsCount <<
    ", load factor: " << load_factor() <<
    ", max load factor: " << max_load_factor() <<
    std::endl;
}

HashSet::Iterator::Iterator(void *p) : t(nullptr), bucketCount(0) {}

HashSet::Iterator::Iterator(Bucket *t, size_t bucketCount) : t(t), bucketCount(bucketCount) {
    currB = t[0].begin();
    if (currB == Bucket::end())
        operator++();
};

HashSet::Iterator &HashSet::Iterator::operator++() {
    if (currB != Bucket::end())
        currB = ++currB;
    while (currB == Bucket::end()) {
        if (idx >= bucketCount - 1) {
            t = nullptr;
            return *this;
        }
        currB = t[++idx].begin();
    }
    return *this;
}

bool HashSet::Iterator::operator!=(const HashSet::Iterator &iter) const {
    return t != iter.t;
}

int HashSet::Iterator::operator*() const {
    return *currB;
}

HashSet::Iterator HashSet::begin() const {
    return Iterator(t, bucketCount);
}

HashSet::Iterator HashSet::end() {
    return Iterator(nullptr);
}