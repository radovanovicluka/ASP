
#include "HashTable.h"
#include <random>
#include "Exceptions.h"

void evaluation( HashTable& table ) {
    int low, high, num;

    cout << "Unesite parametra (donja/gornja granica, broj pretraga): ";
    cin >> low >> high >> num;

    if ( low < 0 || high < 0 || num < 0 || low > high ) {
        throw InvalidValue();
    }

    table.resetStatistics();
    int adr;

    for ( int i = 0; i < num; i++ ) {
        adr = low + ( rand() % ( high - low + 1 ) );
        table.findKey( adr );
    }
    cout << endl << "Prosecan broj pristupa pri uspesnoj pretrazi: " << table.avgAccessSuccess() << endl;
    cout << "Prosecan broj pristupa pri neuspesnoj pretrazi: " << table.avgAccessUnsuccess() << endl << endl;
}

HashTable::HashTable( int size, AddressFunction* func ) : size( size ) {
    fields = new Field[size];
    if ( func == nullptr )
        addressFunction = new QuadraticHashing;
    else
        addressFunction = func;
}

void HashTable::clear( ) {
    delete[] fields;
    fields = new Field[size];
    count = 0;
}

HashTable::~HashTable( ) {
    delete[] fields;
}

ostream& operator<<( ostream& os, const HashTable& table ) {
    for ( int i = 0; i < table.size; i++ ) {
        os << (( table.fields[i].empty ) ? "EMPTY" : table.fields[i].s) << endl;
    }
    return os;
}

string HashTable::findKey( Key k ) {
    Address adr = hashFunction( k );
    Address a = adr;
    int i = 1;

    while ( ( fields[ adr ].deleted || !fields[ adr ].empty ) && i < size ) {

        if ( fields[ adr ].key == k && !fields[ adr ].deleted ) {
            return fields[ adr ].s;
        }

        adr = addressFunction->getAddress( k, a, i ) % size;
        i++;
    }

    brNeuspP += i;
    brNeuspK++;

    return "";
}

bool HashTable::insertKey( Key k, string s ) {
    Address adr = hashFunction( k );
    Address a = adr;
    int i = 1;

    while ( !fields[ adr ].empty ) {

        if ( i == size || fields[ adr ].key == k ) return false;

        adr = addressFunction->getAddress( k, a, i ) % size;
        i++;
    }

    brUsp += i;
    fields[ adr ].key = k;
    fields[ adr ].s = s;
    fields[ adr ].empty = false;
    fields[ adr ].deleted = false;
    count++;

    return true;
}

bool HashTable::deleteKey( Key k ) {
    Address adr = hashFunction( k );
    Address a = adr;
    int i = 1;

    while ( ( fields[ adr ].deleted || !fields[ adr ].empty ) && i < size ) {

        if ( fields[ adr ].key == k && !fields[ adr ].deleted ) {
            fields[ adr ].deleted = true;
            fields[ adr ].empty = true;
            count--;
            return true;
        }

        adr = addressFunction->getAddress( k, a, i ) % size;
        i++;
    }
    return false;
}

double HashTable::avgAccessSuccess() const {
    return static_cast<double>(brUsp) / count;
}

double HashTable::avgAccessUnsuccess() const {
    return static_cast<double>(brNeuspP) / brNeuspK;
}

void HashTable::resetStatistics() {
    brNeuspK = 0;
    brNeuspP = 0;
}


HashTable::HashTable( const HashTable& table ) : size( table.size ) {
    fields = new Field[size];

    for ( int i = 0; i < size; i++ ) {
        fields[i] = table.fields[i];
    }
}

bool AdaptiveHashTable::insertKey(Key k, string s) {
    if ( avgAccessSuccess() > limit || fillRatio() > 3./4 || avgAccessUnsuccess() > limit) {
        resize();
    }
    return HashTable::insertKey(k, s);
}

AdaptiveHashTable::AdaptiveHashTable(int size, int limit, AddressFunction* func) : HashTable( size, func ), limit(limit) {}

void AdaptiveHashTable::resize() {
    AdaptiveHashTable tmp( size * 2, limit * 2, this->addressFunction );

    for ( int i = 0; i < size; i++ ) {
        if ( !fields[i].empty )
            tmp.insertKey( fields[i].key, fields[i].s );
    }

    brUsp = tmp.brUsp;
    delete[] fields;
    fields = tmp.fields;
    tmp.fields = nullptr;
    size *= 2;
}

AdaptiveHashTable::AdaptiveHashTable(const AdaptiveHashTable& table) : HashTable( table ), limit( table.limit ) {}
