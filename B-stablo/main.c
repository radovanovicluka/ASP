#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct AccountDetails {
    long long int CA_ID, CA_B_ID, CA_C_ID, CA_TAX_ST;
    char CA_NAME[256];
    double CA_BAL;
} AccountDetails;


typedef struct Key {
    long long int customerID, accountID;
} Key;


typedef struct CustomerAccount {
    Key key;
    AccountDetails *details;
} CustomerAccount;


typedef struct Customer {
    long long int C_ID, address;
    char firstName[32], lastName[32], email[64];
} Customer;


typedef struct listNode {
    Customer *customer;
    struct listNode *next;
} listNode;


typedef struct List {
    listNode *head, *tail;
} List;


typedef struct node {
    CustomerAccount **accounts;
    struct node *father;
    struct node **sons;
    int capacity, size;
} node;


typedef struct BTree {
    node *root;
    int capacity;
} BTree;


typedef struct node_q {
    node* node;
    struct node_q *next;
} node_q;


typedef struct Queue {
    node_q *head;
    node_q *tail;
} Queue;


char *readLine( FILE * );
CustomerAccount* createAccount( FILE * );
int compareAcc( CustomerAccount * , CustomerAccount * );
void freeAccount( CustomerAccount * );
Customer* createCustomer( FILE * );
List* customerList( FILE * );
void freeList( List * );
Customer* findCustomer( List * , long long int );
node* emptyNode( int capacity, node *father );
int isFull( node *curr );
void initQueue ( Queue * );
int queueEmpty ( Queue* );
void freeBTree ( BTree* );
void insert ( Queue *, node* );
node* delete ( Queue * );;
void freeQueue( Queue* );
int insertInNode( BTree * , node * , CustomerAccount * );
BTree* createTree( FILE * , int );
node* findDestination( BTree * , CustomerAccount * );
node* searchCustomer( BTree * , long long int , int * , int * );
node* findNext( node * , int * , int * );
node* findPrev( node * , int * , int * );
void findAccounts( BTree* tree, long long int ID, FILE *f, List * );
void printBTree ( BTree* tree );
void addAccount( BTree * , CustomerAccount * );
void addRecord( BTree * tree );
node* searchAccount( BTree * , CustomerAccount * , int * );
void freeNode( node* curr );
void deleteAccount( BTree * , CustomerAccount * );
void deleteRecord( BTree * );
void deleteAll( BTree * , long long int );


int main() {
    BTree *tree = NULL;
    int choise, m;
    long long int ID;
    char fileName[256];
    FILE *f = NULL, *g = NULL, *w = NULL;
    List *customers;

    g = fopen( "Customer.txt", "r" );

    customers = customerList( g );

    while ( 1 ) {
        printf("\n1. Kreiraj stablo\n2. Unisti stablo\n3. Ispisi stablo\n4. Pretrazi korisnika\n5. Dodavanje novog racuna\n");
        printf("6. Brisanje racuna\n7. Brisanje svih racuna korisnika\n\n0. Zavrsi program\n");

        while ( 1 ) {
            printf("\nUnesite izbor: ");
            scanf("%d", &choise);
            if ( choise >= 0 && choise < 8 ) break;
        }

        switch( choise ) {
            case 1: {
                if ( !tree ) {
                    printf("\nNaziv fajla: ");
                    scanf("%s", fileName);
                    printf("\nRed stabla: ");
                    scanf( "%d", &m );
                    f = fopen( fileName, "r" );

                    tree = createTree( f, m );
                }
                else {
                    printf("\nStablo vec postoji\n");
                }
                break;
            }
            case 2: {
                if ( tree ) {
                    freeBTree( tree );
                    tree = NULL;
                }
                else {
                    printf("\nStablo ne postoji\n");
                }
                break;
            }
            case 3: {
                printf("\n");
                printBTree( tree );
                break;
            }
            case 4: {
                printf("\nUnesite ID korisnika: ");
                scanf( "%lld", &ID );
                printf("\nNaziv odredista: ");
                scanf("%s", fileName);

                w = fopen( fileName, "w" );

                findAccounts( tree, ID, w, customers );
                break;
            }
            case 5: {
                addRecord( tree );
                break;
            }
            case 6: {
                deleteRecord( tree );
                break;
            }
            case 7: {
                printf("\nUnesite ID korisnika: ");
                scanf( "%lld", &ID );

                deleteAll( tree, ID );
                break;
            }
            case 0: {
                freeBTree( tree );
                freeList( customers );
                if ( f )
                    fclose( f );
                fclose( g );
                if ( w )
                    fclose( w );
                return 0;
            }
        }
    }
}


void deleteAll( BTree *tree, long long int ID ) {
    int index, null = 0;
    CustomerAccount *acc;
    node *tmp;

    while ( ( tmp = searchCustomer( tree, ID, &index, &null ) ) ) {
        acc = tmp->accounts[ index ];
        deleteAccount( tree, acc );
    }
}


void deleteRecord( BTree *tree ) {
    CustomerAccount *acc = ( CustomerAccount* ) calloc(1, sizeof ( CustomerAccount ) );
    if ( !acc ) exit( 1 );
    acc->details = ( AccountDetails* ) calloc(1, sizeof ( AccountDetails ) );
    if ( !acc->details ) exit( 1 );
    printf("\nUnesite podatke racuna: ");
    scanf( "%lld|%lld|%lld|%[^|]|%lld|%lf", &(acc->details->CA_ID), &(acc->details->CA_B_ID), &(acc->details->CA_C_ID),
           acc->details->CA_NAME, &(acc->details->CA_TAX_ST), &(acc->details->CA_BAL));

    acc->key.customerID = acc->details->CA_C_ID;
    acc->key.accountID = acc->details->CA_ID;

    deleteAccount( tree, acc );
}


int merge( node* curr, node* brother ) {
    CustomerAccount **arrayK = calloc( curr->capacity - 1, sizeof ( CustomerAccount* ) );
    node **arrayP = calloc( curr->capacity, sizeof( node* ) );

//    CustomerAccount *arrayK[10] = { 0 };
//    node *arrayP[10] = { 0 };
    node *father = curr->father;
    int brSize;

    int index = 0;
    for ( ; father->sons[index] != curr; index++ );

    int i = 0;
    for ( ; i < curr->size; i++ ) {
        arrayK[i] = curr->accounts[i];
        arrayP[i] = curr->sons[i];
    }
    arrayK[i] = father->accounts[ index ];
    arrayP[i] = curr->sons[i];
    i++;

    int j = 0;
    for ( ; j < brother->size; j++, i++ ) {
        arrayK[i] = brother->accounts[j];
        arrayP[i] = brother->sons[j];
    }
    arrayP[ i ] = brother->sons[ j ];

    i = 0;
    brSize = brother->size;
    brother->size = 0;
    for ( ; i < curr->size + brSize + 1; i++ ) {
        brother->accounts[ i ] = arrayK[ i ];
        brother->sons[ i ] = arrayP[ i ];
        brother->size++;
    }
    brother->sons[ i ] = arrayP[ i ];

    for ( int i = 0; i < brother->size + 1; i++ ) {
        if ( brother->sons[i] ) {
            brother->sons[i]->father = brother;
        }
    }

//    freeNode( curr );
    free( arrayP );
    free( arrayK );

    return index;
}


node* hasRight( node* curr ) {
    node *father = curr->father, *brother;

    int i = 0;
    for ( ; father->sons[i]; i++ ) {
        if ( father->sons[i] == curr ) {
            i++;
            break;
        }
    }
    brother = father->sons[i];
    if ( brother ) return brother;
    return NULL;
}


node* hasLeft( node* curr ) {
    node *father = curr->father, *brother;

    int i = 0;
    for ( ; father->sons[i]; i++ ) {
        if ( father->sons[i] == curr ) {
            i--;
            break;
        }
    }
    if ( i < 0 ) return NULL;
    brother = father->sons[i];
    if ( brother ) return brother;
    return NULL;
}


void borrowR( node* curr, node* brother ) {
    node* father = curr->father;

    int i = 0;
    for ( ; father->sons[i]; i++ ) {
        if ( father->sons[i] == curr ) {
            break;
        }
    }

    curr->sons[ curr->size + 1 ] = brother->sons[0];
    curr->accounts[curr->size] = father->accounts[i];
    father->accounts[i] = brother->accounts[0];

    for ( int j = 0; j < brother->size - 1; j++ ) {
        brother->accounts[j] = brother->accounts[ j + 1 ];
        brother->sons[j] = brother->sons[ j + 1 ];
    }
    brother->sons[ brother->size - 1 ] = brother->sons[ brother->size ];
    brother->accounts[ brother->size - 1 ] = NULL;
    brother->sons[ brother->size ] = NULL;

    brother->size--;
    curr->size++;

    if ( curr->sons[ curr->size ] )
        curr->sons[ curr->size ]->father = curr;
}


void borrowL( node* curr, node* brother ) {
    node* father = curr->father;

    int i = 0;
    for ( ; father->sons[i]; i++ ) {
        if ( father->sons[i] == curr ) {
            break;
        }
    }

    int j;
    for ( j = curr->size; j > 0; j-- ) {
        curr->accounts[ j + 1 ] = curr->accounts[j];
        curr->sons[ j + 1 ] = curr->sons[j];
    }
    curr->sons[j + 1] = curr->sons[ j ];

    curr->sons[0] = brother->sons[ brother->size ];
    curr->accounts[0] = father->accounts[ i ];
    father->accounts[i] = brother->accounts[ brother->size - 1 ];

    brother->accounts[ brother->size - 1 ] = NULL;
    brother->sons[ brother->size ] = NULL;

    brother->size--;
    curr->size++;

    if ( curr->sons[0] )
        curr->sons[0]->father = curr;
}


node* canBorrowR( node *curr ) {
    node* brother = hasRight( curr );
    if ( brother && brother->size > ceil(brother->capacity / 2.) - 1 ) return brother;
    return NULL;
}


node* canBorrowL( node *curr ) {
    node *brother = hasLeft( curr );
    if ( brother && brother->size > ceil(brother->capacity / 2.) - 1 ) return brother;
    return NULL;
}


node* findReplacement( node* curr, int *index ) {
    node *tmp = curr;
    tmp = tmp->sons[ *index + 1 ];
    while ( tmp->sons[0] ) {
        tmp = tmp->sons[0];
    }
    *index = 0;

    return tmp;
}


void deleteAccount( BTree * tree, CustomerAccount *acc ) {
    int index, oldIndex, steps = 0;
    node *curr = searchAccount( tree, acc, &index ), *new, *brother;
    CustomerAccount *tmp = ( CustomerAccount* ) calloc(1, sizeof( CustomerAccount ) );
    if ( !tmp ) exit( 1 );

    oldIndex = index;

    if ( curr->sons[0] ) {
        new = findReplacement( curr, &index );
        *tmp = *new->accounts[ index ];
        new->accounts[ index ] = curr->accounts[ oldIndex ];
        curr->accounts[ oldIndex ] = tmp;
        curr = new;
    }

    while ( 1 ) {

        if ( !curr->sons[0] )
            freeAccount(curr->accounts[index]);
        for (int i = index; i < curr->size - 1; i++) {
            curr->accounts[i] = curr->accounts[i + 1];
            curr->sons[i] = curr->sons[i + 1];
        }
        curr->sons[curr->size - 1] = curr->sons[curr->size];
        curr->sons[curr->size] = NULL;
        curr->accounts[curr->size - 1] = NULL;
        curr->size--;

        if ( curr->father && curr->size < ceil(curr->capacity / 2.) - 1 ) {
            if ( ( brother = canBorrowR( curr ) ) ) {
                borrowR( curr, brother );
                break;
            }
            else if ( ( brother = canBorrowL( curr ) ) ) {
                borrowL( curr, brother );
                break;
            }
//            else merge nodes
//            curr postaje otac od spojenih
//            index pozicija preuzetog kljuca
//            UVEK SE STAVLJA U DESNI!!!
            else {
                if ( ( brother = hasRight( curr ) ) ) {
                    index = merge( curr, brother );
                    new = brother;
                    curr = brother->father;
                }
                else if ( ( brother = hasLeft( curr ) ) ) {
                    index = merge( brother, curr );
                    new = curr;
                    curr = curr->father;
                }
            }
        }
        else {
            if ( !curr->father && curr->size == 0 ) {
                freeNode( curr );
                new->father = NULL;
                tree->root = new;
            }
            break;
        }
    }
}


node* searchAccount( BTree *tree, CustomerAccount *acc, int *index ) {
    node* tmp = tree->root;

    while ( tmp != NULL ) {
        int i = 0;
        for ( ; i < tmp->size; i++ ) {
            if ( compareAcc( acc, tmp->accounts[i] ) == -1 ) {
                break;
            }
            if ( compareAcc( acc, tmp->accounts[i] ) == 0 ) {
                *index = i;
                return tmp;
            }
        }
        tmp = tmp->sons[i];
    }
    return NULL;
}


void addRecord( BTree * tree ) {
    CustomerAccount *acc = ( CustomerAccount* ) calloc(1, sizeof ( CustomerAccount ) );
    if ( !acc ) exit( 1 );
    acc->details = ( AccountDetails* ) calloc(1, sizeof ( AccountDetails ) );
    if ( !acc->details ) exit( 1 );
    printf("\nUnesite podatke racuna: ");
    scanf( "%lld|%lld|%lld|%[^|]|%lld|%lf", &(acc->details->CA_ID), &(acc->details->CA_B_ID), &(acc->details->CA_C_ID),
            acc->details->CA_NAME, &(acc->details->CA_TAX_ST), &(acc->details->CA_BAL));

    acc->key.customerID = acc->details->CA_C_ID;
    acc->key.accountID = acc->details->CA_ID;

    addAccount( tree, acc );
}


node* findPrev( node *curr, int *index, int *steps ) {
    long long int ID = curr->accounts[ *index ]->key.customerID;
    node *tmp = curr;
    if ( curr->sons[0] ) {
        tmp = tmp->sons[ *index ];
        while ( tmp->sons[0] ) {
            (*steps)++;
            tmp = tmp->sons[tmp->size];
        }
        *index = tmp->size - 1;
        if ( tmp->accounts[ *index ]->key.customerID == ID )
            return tmp;
        else
            return NULL;
    }
    else {
        if ( *index != 0 ) {
            (*steps)++;
            *index = *index - 1;
            if ( tmp->accounts[ *index ]->key.customerID == ID )
                return tmp;
            else
                return NULL;
        }

        while ( tmp ) {
            (*steps)++;

            if ( tmp->father && tmp->father->sons[0] != tmp ) {
                int i = tmp->father->size;
                for ( ; tmp->father->sons[i] && tmp->father->sons[i] != tmp; i--) {
                    (*steps)++;
                }
                *index = i;

                if ( *index < tmp->size && tmp->accounts[ *index ]->key.customerID == ID )
                    return tmp->father;
                else
                    return NULL;
            }
            tmp = tmp->father;
        }
    }
    return NULL;
}


node* findNext( node *curr, int *index, int *steps ) {
    long long int ID = curr->accounts[ *index ]->key.customerID;
    node *tmp = curr;
    if ( curr->sons[0] ) {
        tmp = tmp->sons[ *index + 1 ];
        while ( tmp->sons[0] ) {
            (*steps)++;
            tmp = tmp->sons[0];
        }
        *index = 0;
        if ( tmp->accounts[ *index ]->key.customerID == ID )
            return tmp;
        else
            return NULL;
    }
    else {
        if ( *index < curr->size - 1 ) {
            (*steps)++;
            *index = *index + 1;
            if ( tmp->accounts[ *index ]->key.customerID == ID )
                return tmp;
            else
                return NULL;
        }

        while ( tmp ) {
            (*steps)++;

            if ( tmp->father && tmp->father->sons[tmp->father->size] != tmp ) {
                int i = 0;
                for ( ; tmp->father->sons[i] && tmp->father->sons[i] != tmp; i++) {
                    (*steps)++;
                }
                *index = i;

                if ( *index < tmp->father->size && tmp->father->accounts[ *index ]->key.customerID == ID )
                    return tmp->father;
                else
                    return NULL;
            }
            tmp = tmp->father;
        }
    }
    return NULL;
}


void findAccounts( BTree* tree, long long int ID, FILE *f, List *list ) {
    int index, oldIndex, steps = 0;
    node *tmp = searchCustomer(tree, ID, &index, &steps);
    oldIndex = index;
    node *other;
    Customer *cus = findCustomer( list, ID );
    node *tmp2 = tmp;

    if ( !cus ) {
        return;
    }

    if (index == -1) {
        fprintf( f, "%lld|%s|%s|%s|null|null|null|null\n", cus->C_ID, cus->firstName, cus->lastName, cus->email );
        return;
    }

    fprintf( f, "%lld|%s|%s|%s|%lld|%lld|%lld|%lf\n", cus->C_ID, cus->firstName, cus->lastName, cus->email,
             tmp->accounts[ index ]->details->CA_ID, tmp->accounts[ index ]->details->CA_C_ID,
             tmp->accounts[ index ]->details->CA_TAX_ST, tmp->accounts[ index ]->details->CA_BAL );

    while ( ( other = findPrev( tmp, &index, &steps ) ) ) {
//         ispis za other->accounts[ index ]; u f
        fprintf( f, "%lld|%s|%s|%s|%lld|%lld|%lld|%lf\n", cus->C_ID, cus->firstName, cus->lastName, cus->email,
                 other->accounts[ index ]->details->CA_ID, other->accounts[ index ]->details->CA_C_ID,
                 other->accounts[ index ]->details->CA_TAX_ST, other->accounts[ index ]->details->CA_BAL );
        tmp = other;
    }

    index = oldIndex;
    tmp = tmp2;

    while ( ( other = findNext( tmp, &index, &steps ) ) ) {
//     ispis za other->accounts[ index ]; u f
        fprintf( f, "%lld|%s|%s|%s|%lld|%lld|%lld|%lf\n", cus->C_ID, cus->firstName, cus->lastName, cus->email,
                 other->accounts[ index ]->details->CA_ID, other->accounts[ index ]->details->CA_C_ID,
                 other->accounts[ index ]->details->CA_TAX_ST, other->accounts[ index ]->details->CA_BAL );
        tmp = other;
    }

    printf("Za pretragu svih racuna bilo je potrebno %d koraka\n", steps);

}


// Vraca pokazivac na nod u kom se nalazi acc i index CustAcc
node* searchCustomer( BTree* tree, long long int ID, int *index, int *steps ) {
    node* tmp = tree->root;

    while ( tmp ) {
        int i = 0;
        for ( ; i < tmp->size; i++ ) {
            (*steps)++;
            if ( ID == tmp->accounts[i]->key.customerID ) {
                *index = i;
                return tmp;
            }
        }
        i = 0;
        for ( ; i < tmp->size; i++ ) {
            (*steps)++;
            if ( ID < tmp->accounts[i]->key.customerID ) {
                break;
            }
        }
        tmp = tmp->sons[i];
    }
    *index = -1;
    return NULL;
}


node* findDestination( BTree *tree, CustomerAccount *acc ) {
    node* tmp = tree->root;

    while ( tmp->sons[0] != NULL ) {
        int i = 0;
        for ( ; i < tmp->size; i++ ) {
            if ( compareAcc( acc, tmp->accounts[i] ) == -1 ) {
                break;
            }
        }
        tmp = tmp->sons[i];
    }
    return tmp;
}


void addAccount( BTree* tree, CustomerAccount* acc ) {
    node* dest = findDestination( tree, acc );
    int i = insertInNode( tree, dest, acc );
    if ( !i ) {
        printf("GRESKA: Primarni kljuc vec postoji\n");
    }
}


BTree* createTree( FILE* f, int m ) {
    BTree* tree = ( BTree* ) calloc(1, sizeof ( BTree ) );
    if ( !tree ) exit( 1 );

    tree->capacity = m;
    tree->root = emptyNode( tree->capacity, NULL );
    CustomerAccount* tmp;
    node* dest;

    while ( ( tmp = createAccount( f ) ) != NULL ) {
        addAccount( tree, tmp );
    }
    return tree;
}


void splitNode( BTree *tree, node *dest, CustomerAccount *account ) {
    node *new, *left = NULL, *right = NULL, *root;
    int mid = dest->capacity / 2;

    while ( 1 ) {
        CustomerAccount **arrayK = ( CustomerAccount** ) calloc( dest->capacity, sizeof ( CustomerAccount * ) );
        if ( !arrayK ) exit( 1 );

        node **arrayP = ( node** ) calloc( dest->capacity + 1, sizeof ( CustomerAccount * ) );
        if ( !arrayP ) exit( 1 );

        new = emptyNode( dest->capacity, dest->father );
        for ( int i = 0; i < dest->size; i++ ) {
            arrayK[i] = dest->accounts[i];
            arrayP[i] = dest->sons[i];
        }
        arrayP[dest->capacity - 1] = dest->sons[dest->capacity - 1];

        int i = 0;
        for ( ; i < dest->size; i++ ) {
            if ( compareAcc( account, dest->accounts[i] ) == -1 ) break;
        }
        for ( int k = dest->size; k > i; k-- ) {
            arrayK[ k ] = arrayK[ k - 1 ];
            arrayP[ k + 1 ] = arrayP[ k ];
        }
        arrayP[i + 1] = right;
        arrayK[i] = account;

        for ( int i = mid + 1; i < dest->capacity ; i++ ) {
            new->size++;
            new->accounts[i - mid - 1] = arrayK[i];
            new->sons[i - mid - 1] = arrayP[i];
        }
        new->sons[new->size] = arrayP[ dest->capacity ];

        for ( int i = 0; i < new->size + 1 ; i++ ) {
            if ( new->sons[i] ) new->sons[i]->father = new;
        }

        dest->size = 0;
        for ( int i = 0; i < mid; i++ ) {
            dest->size++;
            dest->accounts[i] = arrayK[i];
            dest->sons[i] = arrayP[i];
        }
        for( int i = mid; i<dest->capacity - 1; i++ ) {
            dest->accounts[i] = NULL;
            dest->sons[ i + 1 ] = NULL;
        }
        dest->sons[mid] = arrayP[ mid ];

        left = dest;
        right = new;

        if ( !dest->father ) {
            root = emptyNode( dest->capacity, NULL );
            root->accounts[0] = arrayK[mid];
            root->sons[0] = left;
            left->father = root;
            root->sons[1] = right;
            right->father = root;
            tree->root = root;
            root->size = 1;
            break;
        }
        else if ( !isFull( dest->father ) ) {
            int i = 0;
            for ( ; i < dest->father->size; i++ ) {
                if ( compareAcc( arrayK[mid], dest->father->accounts[i] ) == -1 ) break;
            }
            for ( int k = dest->father->size; k > i; k-- ) {
                dest->father->accounts[ k ] = dest->father->accounts[ k - 1 ];
                dest->father->sons[ k + 1 ] = dest->father->sons[ k ];
            }
            dest->father->accounts[i] = arrayK[mid];
            dest->father->sons[ i + 1 ] = new;
            dest->father->size++;
            break;
        }
        dest = dest->father;
        account = arrayK[ mid ];
        free(arrayK);
        free(arrayP);
    }
}


int insertInNode( BTree *tree, node *dest, CustomerAccount* account ) {
    if ( !isFull( dest ) ) {
        int i = 0;
        for ( ; i < dest->size; i++ ) {
            if ( compareAcc( account, dest->accounts[i] ) == 0 ) return 0;
            if ( compareAcc( account, dest->accounts[i] ) == -1 ) break;
        }
        for ( int k = dest->size; k > i; k-- ) {
            dest->accounts[ k ] = dest->accounts[ k - 1 ];
        }
        dest->accounts[i] = account;
        dest->size++;
    }
    else {
        splitNode( tree, dest, account );
    }
    return 1;
}


void freeNode( node* curr ) {
    for ( int i = 0; i < curr->size; i++ ) {
        freeAccount( curr->accounts[i] );
    }
    free( curr->accounts );
    free( curr->sons );
    free( curr );
}


int isFull( node *curr ) {
    if ( curr->capacity - 1 == curr->size ) return 1;
    else return 0;
}


node* emptyNode( int capacity, node *father ) {
    node *new = ( node* ) calloc(1, sizeof ( node ) );
    if ( !new ) exit( 1 );

    new->capacity = capacity;
    new->size = 0;
    new->father = father;
    new->accounts = ( CustomerAccount** ) calloc(capacity - 1, sizeof ( CustomerAccount* ) );
    if ( !new->accounts ) exit( 1 );

    new->sons = ( node** )calloc(capacity, sizeof ( node* ) );
    if ( !new->sons ) exit( 1 );

    return new;
}


Customer* findCustomer( List *list, long long int id ) {
    listNode *curr = list->head;

    while ( curr ) {
        if ( curr->customer->C_ID == id ) return curr->customer;
        curr = curr->next;
    }
    return NULL;
}


void freeList( List *list ) {
    listNode *curr = list->head;
    listNode *old;

    while ( curr ) {
        old = curr;
        curr = curr->next;

        free(old->customer);
        free(old);
    }
    free( list );
}


List* customerList( FILE *f ) {
    List *list = ( List* ) calloc(1, sizeof (List ) );
    if ( !list ) exit( 1 );

    Customer *curr;
    listNode *node;

    while ( ( curr = createCustomer( f ) ) != NULL ) {
        node = ( listNode* ) calloc(1, sizeof( listNode ) );
        if ( !node ) exit( 1 );

        node->customer = curr;

        if ( !list->head ) {
            list->head = list->tail = node;
        }
        else {
            list->tail->next = node;
            list->tail = list->tail->next;
        }
    }
    return list;
}


Customer* createCustomer( FILE *f ) {
    Customer *customer = ( Customer* ) calloc(1, sizeof ( Customer ) );
    if ( !customer ) exit( 1 );

    char *line = readLine( f );

    if ( !line ) return NULL;

    sscanf( line, "%lld|%[^|]|%[^|]|%[^|]|%lld", &(customer->C_ID), customer->firstName, customer->lastName,
                                                customer->email, &( customer->address ) );

    free( line );
    return customer;
}


void freeAccount( CustomerAccount *acc ) {
    if ( acc ) {
        free(acc->details);
        free(acc);
    }
}


int compareAcc( CustomerAccount *acc1, CustomerAccount *acc2 ) {
    if ( acc1->key.customerID > acc2->key.customerID ) return 1;
    if ( acc1->key.customerID < acc2->key.customerID ) return -1;
    if ( acc1->key.accountID > acc2->key.accountID ) return 1;
    if ( acc1->key.accountID < acc2->key.accountID ) return -1;
    return 0;
}


CustomerAccount* createAccount( FILE *f ) {
    CustomerAccount* acc = ( CustomerAccount* )calloc(1, sizeof ( CustomerAccount ) );
    if ( !acc ) exit( 1 );

    acc->details = ( AccountDetails* ) calloc(1, sizeof ( AccountDetails ) );
    if ( !acc->details ) exit( 1 );

    char *line = readLine( f );

    if ( !line ) return NULL;

    sscanf( line, "%lld|%lld|%lld|%[^|]|%lld|%lf", &(acc->details->CA_ID), &(acc->details->CA_B_ID), &(acc->details->CA_C_ID),
                                            acc->details->CA_NAME, &(acc->details->CA_TAX_ST), &(acc->details->CA_BAL));

    acc->key.accountID = acc->details->CA_ID;
    acc->key.customerID = acc->details->CA_C_ID;

    free( line );
    return acc;
}


char *readLine( FILE* f ) {
    char *string = NULL, c, *tmp;
    int size = 0;

    while ( ( c = getc( f ) ) != '\n' && c != EOF ) {
        tmp = realloc( string, sizeof( char ) * ( size + 1 ) );
        if ( tmp == NULL ) {
            printf("MEM_GRESKA");
            free( string );
            exit( 0 );
        }

        string = tmp;
        string[size++] = c;
    }

    if ( size ) {
        tmp = realloc( string, sizeof( char ) * ( size + 1 ) );
        if ( tmp == NULL ) {
            printf("MEM_GRESKA");
            free( string );
            exit( 0 );
        }

        string = tmp;

        string[size] = '\0';
    }
    return string;
}


void freeQueue( Queue* q) {
    node_q *tmp = q->head;
    node_q *next = tmp;

    while( next ) {
        next = next->next;
        free( tmp );
    }
    free( q );
}


void freeBTree ( BTree* tree ) {
    Queue* q = ( Queue* ) calloc( 1, sizeof ( Queue ));
    if ( !q ) exit( 1 );

    initQueue( q );

    if ( !tree )
        return;

    node* tmp = tree->root;
    insert( q, tmp );

    while ( !queueEmpty( q ) ) {
        tmp = delete( q );
        if ( tmp ) {
            for ( int i = 0; i < tmp->capacity; i++ ) {
                if (tmp->sons[i] ) insert( q, tmp->sons[i]);
            }
            freeNode(tmp);
        }
    }
    free( tree );
    freeQueue( q );
}


void printNode ( node* curr ) {
    printf("( ");
    for ( int i = 0; i < curr->size; i++ ) {
        printf("%lld,%lld", curr->accounts[i]->key.customerID % 1000, curr->accounts[i]->key.accountID % 1000);
        if ( i != curr->size - 1 ) printf(" | ");
    }
    printf(" ) ");
}


void printBTree ( BTree* tree ) {
    Queue* q = ( Queue* )calloc( 1, sizeof ( Queue ));
    if ( !q ) exit( 1 );

    initQueue( q );

    if ( !tree )
        return;

    node* tmp = tree->root;
    insert( q, tmp );
    insert( q, NULL );

    while ( !queueEmpty( q ) ) {
        tmp = delete( q );
        if ( tmp ) {
            for ( int i = 0; i < tmp->size + 1; i++ ) {
                if (tmp->sons[i] ) insert( q, tmp->sons[i]);
            }
            printNode( tmp );
        }

        if ( !q->head->node ) {
            delete( q );
            printf("\n");
            if ( !queueEmpty( q ) ) {
                insert( q, NULL );
            }
        }
    }
    freeQueue( q );
}


void initQueue ( Queue *Q ) {
    Q->head = NULL;
    Q->tail = NULL;
}


void insert ( Queue *Q, node* node1 ) {
    node_q *new = ( node_q* )calloc(1, sizeof ( node_q ) );
    if ( !new ) exit( 1 );

    if ( new == NULL )
        exit( 1 );

    new->node = node1;
    new->next = NULL;

    if ( Q->tail != NULL ) {
        Q->tail->next = new;
    }

    if ( Q->head == NULL ) {
        Q->head = new;
    }
    Q->tail = new;
}


node* delete ( Queue *Q ) {
    if ( Q->head == NULL )
        return NULL;

    node *tmp = Q->head->node;
    node_q* tmp1 = Q->head;
    Q->head = Q->head->next;

    free(tmp1 );

    if ( Q->head == NULL )
        Q->tail = NULL;

    return tmp;
}


int queueEmpty ( Queue* q ) {
    if ( q->head == NULL )
        return 1;
    else
        return 0;
}
