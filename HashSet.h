#ifndef PROJEKT_HASHSET_H
#define PROJEKT_HASHSET_H

/** @file */

#include <chrono>
#include <initializer_list>
#include <iostream>

#include "Bucket.h"

using namespace std::chrono;

class HashSet {
    /// główna tablica wiaderek
    Bucket *t;
    /// ilość wiaderek
    size_t bucketCount;
    /// akutalna ilość elementów
    size_t itemsCount = 0;

    /// funkcja haszująca
    size_t (*hash)(const int);

    /**
      * @brief dokonuje rehasz jeżeli zapełnienie przekroczy ustalony poziom
      */
    inline void rehashIfNeeded();

public:
    /// dostępne funkcje haszujące
    enum Hashes {
        set_nothingHash,
        set_xorshiftHash,
        set_robertJenkinsHash,
        set_shift32multHash,
    };

    // lista wbudowanych w klasę funkcji haszujących
    static size_t nothingHash(int v);

    static size_t xorshift(const uint64_t &n, int i);

    static size_t xorshiftHash(int v);

    static size_t robertJenkinsHash(int v);

    static size_t shift32multHash(int v);

    /**
      * @brief konstruktor domyślny - tworzy pusty set
      * Umożliwia wybranie funkcji haszującej z wbudowanych w klasę i wielkość startową
      */
    explicit HashSet(size_t size = 20, Hashes set_hash = set_xorshiftHash);


    /**
      * @brief konstruktor domyślny - tworzy pusty set
      * Umożliwia wybranie funkcji haszującej z wbudowanych w klasę
      */
    explicit HashSet(Hashes set_hash);

    /**
      * @brief konstruktor - tworzy pusty set z własną funkcją haszującą
      */
    explicit HashSet(size_t size, size_t (*hash)(const int));

    /**
     * @brief konstruktor - tworzy i wypełnia set wartościami,
     * wielkość ustawiana jest na 2x ilość danych wejściowych
     */
    HashSet(std::initializer_list<int> il);

    /**
     * @brief konstruktor - tworzy i wypełnia set wartościami z c array,
     * wielkość ustawiana jest na 2x ilość danych wejściowych
     */
    HashSet(const int* list, size_t listSize);

    ~HashSet();

    void makeEmpty();

    HashSet(const HashSet &o);

    HashSet(HashSet &&o) noexcept;

    HashSet &operator=(const HashSet &o) noexcept;

    HashSet &operator=(HashSet &&o) noexcept;

    /**
     * @brief zwraca indeks w tablicy gdzie należy element podany w argumencie
     * metoda wykorzystuje wybraną przez nas funkcję haszującą
     */
    inline size_t getIdx(int v);

    /**
     * @brief dodaje wartość do listy jeżeli nie istnieje, w przeciwnym przypadku nie robi nic
     *
     * @param v wartość
     * @param noRehash zapobiegaj automatycznemu przehaszowaniu przy zwiększaniu
     * @return *this
     */
    HashSet &insert(int v, bool noRehash = false);

    /**
     * @brief insert
     *
     * @param v wartość
     * @return *this
     */
    HashSet &operator+=(int v);

    /**
     * @brief benchmark (czasowy) insert
     *
     * @param v wartość
     * @return czas wykonania operacji
     */
    size_t benchTime_insert(int v);

    /**
     * @brief benchmark (czasowy) contains
     *
     * @param v wartość
     * @return czas wykonania operacji
     */
    size_t benchTime_contains(int v);

    /**
     * @brief usuwa wartość gdy istnieje
     *
     * @param v wartość
     * @return *this
     */
    HashSet &erase(int v);

    /**
     * @brief erase
     *
     * @param v wartość
     * @return *this
     */
    HashSet &operator-=(int v);

    /**
      * @brief sprawdza czy wartość istnieje w secie
      *
      * @param v wartość
      * @return czy istnieje?
      */
    bool contains(int v);

    /**
     * @brief benchmark insert
     *
     * @param v wartość
     * @return ilość wykonanych iteracji
     */
    unsigned benchOps_insert(int v);

    /**
     * @brief benchmark contains
     *
     * @param v wartość
     * @return ilość wykonanych iteracji
     */
    unsigned benchOps_contains(int v);

    /**
     * @brief zmienia wielkość tablicy
     *
     * @param newBucketCount nowa wielkość
     * @return *this
     */
    HashSet &rehash(size_t newBucketCount);

    void printDebug() const;

    /**
     * @brief zwraca średnie zapełnienie kubełków
     *
     * @return load_factor
     */
    [[nodiscard]] inline double load_factor() const;

    [[nodiscard]] size_t max_load_factor() const;

    /**
     * @brief zwraca ilość kubełków
     *
     * @return ilość kubełków
     */
    [[nodiscard]] inline size_t bucket_count() const;

    /**
     * @brief zwraca ilość elementów
     *
     * @return ilość elementów
     */
    [[nodiscard]] inline size_t size() const;

    /**
     * @brief metody iteratora
     */
    class Iterator {
        Bucket *t;
        size_t bucketCount;
        size_t idx = 0;
        Bucket::Iterator currB = Bucket::Iterator(nullptr);

    public:
        explicit Iterator(void *p);

        Iterator(Bucket *t, size_t bucketCount);

        Iterator &operator++();

        bool operator!=(const Iterator &iter) const;

        int operator*() const;
    };

    [[nodiscard]] Iterator begin() const;

    static Iterator end();
};

#endif //PROJEKT_HASHSET_H
