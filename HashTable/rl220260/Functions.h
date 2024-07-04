
#ifndef DZ3_FUNCTIONS_H
#define DZ3_FUNCTIONS_H

#include "HashTable.h"

void readFile( string , HashTable& );
int menu();
HashTable* createTable();
void search( HashTable* );
void insertValue( HashTable* );
void deleteValue( HashTable* );

#endif //DZ3_FUNCTIONS_H
