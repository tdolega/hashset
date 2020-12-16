#include "Bucket.h"

Bucket::~Bucket() {
    while (head) {
        b *next = head->next;
        delete head;
        head = next;
    }
}

bool Bucket::insert(const int v) {
    if (exist(v)) return false;
    head = new b{ v, head };
    return true;
}

size_t Bucket::benchOps_insert(const int v) {
    size_t ops = benchOps_exist(v);
    insert(v);
    return ops;
}

bool Bucket::remove(const int v) {
    if (!head) return false;
    b *_head = head;
    b *prev = head;
    if (_head->value == v) {
        head = _head->next;
        delete _head;
        return true;
    }
    _head = _head->next;
    while (_head) {
        if (_head->value == v) {
            prev->next = _head->next;
            delete _head;
            return true;
        }
        _head = _head->next;
    }
    return false;
}

bool Bucket::exist(const int v) const {
    b *_head = head;
    while (_head) {
        if (_head->value == v) return true;
        _head = _head->next;
    }
    return false;
}

size_t Bucket::benchOps_exist(const int v) const {
    size_t ops = 0;
    b *_head = head;
    while (_head) {
        ops++;
        if (_head->value == v) return ops;
        _head = _head->next;
    }
    return ops;
}

size_t Bucket::getSize() const {
    size_t size = 0;
    b *_head = head;
    while (_head) {
        size++;
        _head = _head->next;
    }
    return size;
}

Bucket::Iterator &Bucket::Iterator::operator=(b *node) {
    currB = node;
    return *this;
}

Bucket::Iterator &Bucket::Iterator::operator++() {
    if (currB) currB = currB->next;
    return *this;
}

bool Bucket::Iterator::operator!=(const Iterator &iterator) const {
    return currB != iterator.currB;
}

bool Bucket::Iterator::operator==(const Iterator &iterator) const {
    return currB == iterator.currB;
}

int Bucket::Iterator::operator*() const {
    return currB->value;
}

Bucket::Iterator Bucket::begin() const {
    return Iterator(head);
}

Bucket::Iterator Bucket::end() {
    return Iterator(nullptr);
}