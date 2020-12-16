#ifndef PROJEKT_BUCKET_H
#define PROJEKT_BUCKET_H

/** @file */

#include "cstddef"

class Bucket {
    /// node
    struct b {
        /// wartość
        int value;
        /// następny node (b)
        b *next;
    };
    /// pierwszy element listy
    b *head = nullptr;

public:
    ~Bucket();

    /**
     * @brief dodaje wartość do listy jeżeli nie istnieje
     *
     * @param v wartość
     * @return czy dodano nowy?
     */
    bool insert(int v);

    /**
     * @brief usuwa wartość gdy istnieje
     *
     * @param v wartość
     * @return czy usunięto?
     */
    bool remove(int v);

    /**
     * @brief sprawdza czy wartość istnieje w liście
     *
     * @param v wartość
     * @return czy istnieje?
     */
    [[nodiscard]] bool exist(int v) const;

    /**
     * @brief benchmark insert
     *
     * @param v wartość
     * @return ilość wykonanych iteracji
     */
    size_t benchOps_insert(int v);

    /**
     * @brief benchmark exist
     *
     * @param v wartość
     * @return ilość wykonanych iteracji
     */
    [[nodiscard]] size_t benchOps_exist(int v) const;

    /**
     * @brief zwraca wielkość kubełka (długość listy)
     *
     * @return unsigned wielkość
     */
    [[nodiscard]] size_t getSize() const;

    /**
     * @brief metody iteratora
     */
    class Iterator {
        const b *currB;

    public:
        explicit Iterator(const b *node) : currB(node) {}

        Iterator &operator=(b *node);

        Iterator &operator++();

        bool operator!=(const Iterator &iterator) const;

        bool operator==(const Iterator &iterator) const;

        int operator*() const;
    };

    [[nodiscard]] Iterator begin() const;

    static Iterator end();
};

#endif //PROJEKT_BUCKET_H
