
#ifndef DZ3_HASHTABLE_H
#define DZ3_HASHTABLE_H

#include <string>
#include <iostream>
#include "AddressFunction.h"

using namespace std;

class HashTable {
public:
    explicit HashTable(int size, AddressFunction* func = nullptr);
    HashTable( const HashTable& );
    void operator=( const HashTable& ) = delete;

    string findKey( Key k );
    virtual bool insertKey( Key k, string s );
    bool deleteKey( Key k );
    double avgAccessSuccess() const;
    double avgAccessUnsuccess() const;
    void resetStatistics();
    void clear();
    int keyCount() const { return count; }
    int tableSize() const { return size; }
    friend ostream& operator<<( ostream&, const HashTable& );
    double fillRatio() const {
        return static_cast<double>(count) / size;
    }

    ~HashTable();

private:
    int count = 0, brNeuspP = 0, brNeuspK = 0;

    typedef struct field {
        Key key;
        string s;
        bool deleted = false, empty = true;
    } Field;

    Address hashFunction( Key k ) const {
        return k % size;
    }

protected:
    int size, brUsp = 0;
    Field* fields;
    AddressFunction* addressFunction = nullptr;
};

class AdaptiveHashTable : public HashTable {
private:
    int limit;
    void resize();

public:
    AdaptiveHashTable( int size, int limit, AddressFunction* func = nullptr);
    AdaptiveHashTable( const AdaptiveHashTable& );
    void operator=( const AdaptiveHashTable& ) = delete;

    bool insertKey( Key k, string s ) override;
};

void evaluation( HashTable& );

#endif //DZ3_HASHTABLE_H
