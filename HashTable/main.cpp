#include <iostream>
#include "HashTable.h"
#include "Exceptions.h"
#include "Functions.h"

int main() {
    HashTable *table = nullptr;
    int choice;
    string fileName;

    while ( true ) {
        try {
            choice = menu();
            if ( choice != 1 && choice != 0 && table == nullptr ) {
                cout << "Tabela ne postoji" << endl << endl;
                continue;
            }
            switch( choice ) {
                case 1: {
                    if (table == nullptr) {
                        table = createTable();
                    } else {
                        cout << "Tabela vec postoji" << endl;
                    }
                    break;
                }
                case 2: {
                    delete table;
                    table = nullptr;
                    break;
                }
                case 3: {
                    search( table );
                    break;
                }
                case 4: {
                    insertValue( table );
                    break;
                }
                case 5: {
                    deleteValue( table );
                    break;
                }
                case 6: {
                    cout << "Prosecan broj pristupa pri uspesnoj pretrazi: " << table->avgAccessSuccess() << endl << endl;
                    cout << "Prosecan broj pristupa pri neuspesnoj pretrazi: " << table->avgAccessUnsuccess() << endl << endl;
                    break;
                }
                case 7: {
                    table->resetStatistics();
                    cout << "Statistika resetovana" << endl << endl;
                    break;
                }
                case 8: {
                    cout << "Broj kljuceva u tabeli je " << table->keyCount() << endl << endl;
                    break;
                }
                case 9: {
                    cout << "Velicina tabele je " << table->tableSize() << endl << endl;
                    break;
                }
                case 10: {
                    cout << *table << endl;
                    break;
                }
                case 11: {
                    cout << "Popunjenost tabele je " << table->fillRatio() << endl << endl;
                    break;
                }
                case 12: {
                    evaluation(*table);
                    break;
                }
                case 13: {
                    cout << "Unesite naziv datoteke: ";
                    cin >> fileName;
                    cout << endl;
                    readFile( fileName, *table );
                    break;
                }
                case 14: {
                    table->clear();
                    break;
                }
                case 0: {
                    delete table;
                    return 0;
                }
            }
        }
        catch ( InvalidValue err ) {
            cout << err.what() << endl << endl;
        }
        catch ( FileError err ) {
            cout << err.what() << endl << endl;
        }
    }
}
