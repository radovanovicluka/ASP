#include "Functions.h"
#include "Exceptions.h"
#include <fstream>

void deleteValue( HashTable* table ) {
    int key;

    cout << endl << "Unesite kljuc: ";
    cin >> key;
    if ( key < 0 ) { throw InvalidValue(); }

    if ( table->deleteKey( key ) ) {
        cout << "Obrisana vrednost" << endl << endl;
    }
    else {
        cout << "Kljuc nepostoji" << endl << endl;
    }
}


void insertValue( HashTable* table ) {
    int key;
    string s;

    cout << "Unesite kljuc: ";
    cin >> key;
    cout << endl;
    if ( key < 0 ) { throw InvalidValue(); }
    cout << "Unesite string: ";
    cin >> s;

    if ( table->insertKey( key, s ) ) {
        cout << "Uneta vrednost" << endl << endl;
    }
    else {
        cout << "Kljuc vec postoji" << endl << endl;
    }
}


void search( HashTable* table ) {
    int key;
    string s;

    cout << "Unesite kljuc za pretragu: ";
    cin >> key;
    if ( key < 0 ) { throw InvalidValue(); }

    if ( (s = table->findKey(key)) != "" ) {
        cout << endl << "Kljuc je pronadjen" << endl << "string: " << s << endl << endl;
    }
    else {
        cout << endl << "Kljuc nije pronadjen" << endl << endl;
    }
}


HashTable* createTable() {
    int size, limit, choice;

    cout << "1. Obicna tabela" << endl << "2. Adaptivna tabela" << endl << endl << "Izaberite opciju: ";
    cin >> choice;
    if ( !(choice == 1 || choice == 2) ) { throw InvalidValue(); }

    cout << endl << "Unesite velicinu tabele: ";
    cin >> size;
    if ( size <= 0 ) { throw InvalidValue(); }

    if ( choice == 1 ) {
        return new HashTable( size );
    }
    else {
        cout << endl << "Unesite granicu za broj pristupa: ";
        cin >> limit;

        if ( limit <= 1 ) { throw InvalidValue(); }
        return new AdaptiveHashTable( size, limit );
    }
}


int menu() {
    int choice;

    cout << "1. Kreiraj tabelu" << endl << "2. Unisti tabelu" << endl << "3. Pretrazi" << endl << "4. Dodaj" << endl <<
         "5. Obrisi" << endl << "6. Ispisi statistiku" << endl << "7. Resetuj statistiku" << endl << "8. Broj vrednosti" <<
         endl << "9. Velicina tabele" << endl << "10. Ispisi tabelu" << endl << "11. Popunjenost" << endl << "12. Evaluacija"
         << endl << "13. Unos iz datoteke" << endl << "14. Isprazni tabelu" << endl << endl << "0. Zavrsi rad programa"
         << endl << endl << "Izaberite opciju: ";

    cin >> choice;
    cout << endl;

    if ( choice < 0 || choice > 14 ) { throw InvalidValue(); }
    return choice;
}


void readFile( string fileName, HashTable& table ) {
    ifstream input;
    input.open(fileName);
    if ( !input.is_open() ) {
        throw FileError();
    }

    string s;
    int key;

    while ( input >> s ) {
        input >> key;

        table.insertKey( key, s );
    }
    input.close();
}

