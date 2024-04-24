#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 2000000000
#define RED 1
#define BLACK 0
#define BackSpace 10

//struttura della riga della tabella dove effettuare i calcoli per trovare il percorso minimo

typedef struct p_table{
    //distanza che il nodo(rappresentato dall'indice dell' array che ha come elementi "p_table")
    //ha dal nodo 0
    int dist;
    //nodo precedente al nodo corrente nel percorso minimo trovato fino a quel momento
    int prev;
    //posizione che un determinato nodo ha nella heap
    int heap_pos;
}p_table;

//coda di priorità minima (min-heap) per eseguire algoritmo di Dijkstra più efficientemente

typedef struct heap
{
    //lunghezza min-heap
    int heap_size;
    //puntatore a array di interi(che rappresentano i nodi). Nodi ordinati in base alla loro distanza
    //dal nodo 0
    int* in_heap;
}heap;

//nodo dell'albero rosso nero che contiene la distanza di un grafo e il suo indice
typedef struct rb_node {
    int gr_ind;
    int dist;
    int color;
    struct rb_node* left;
    struct rb_node* right;
    struct rb_node* p;
}rb_node;

//funzioni per creare le strutture dati del programma

int** createGraph(int size);

heap* createHeap(int size);

p_table* createTable(int size);


//getMinPath implementa l'algoritmo di Dijkstra per trovare i percorsi minimi
void getMinPath(heap *hp, int** graph, p_table* tb, int* ptr_sum);


//funzioni di supporto a getMinPath
void minHeapify(heap *hp, int n, p_table* tb);

void reducePriority(heap* hp, int n, p_table* tb);

int deleteMin(heap *hp, int* ptr_sum, p_table* tb);

int Parent(int n);


//funzione che trova la dimensione dei grafi e la dimensione della classifica
void find_sizeGraph_topK(char *command, int *info);


//funzioni per liberare la memoria
void freeGraph(int** graph, int dim);

void freeHeap(heap* hp);

void freeTree(rb_node* root);

void freeTable(p_table* tb);


//funzioni per la gestione dell'albero rosso nero
rb_node* rbInsert(rb_node* root, rb_node* z);

rb_node* createNil();

rb_node* treeMinimum(rb_node* node);

rb_node* treeMaximum(rb_node* node);

void rbInsertFix(rb_node** root, rb_node* z);

void leftRotate(rb_node** root, rb_node* x);

void rightRotate(rb_node** root, rb_node* y);

rb_node* rbDelete(rb_node* root, rb_node* z);

void rbTransplant(rb_node** root, rb_node* u, rb_node* v);

void rbDeleteFix(rb_node** root, rb_node* x);


//funzione che stampa i migliori K nodi
void printTopK(rb_node* root);

//nodo NULL dell'albero rosso nero
rb_node* NIL;

//numero di nodi da stampare
int to_print = 0;

int main(){
    char input[30], *buffer = NULL;
    int** graph = NULL, info[2] = {0,0}, buff_len = 0, arc_weight = 0, charRead = 0, gr_row = 0, gr_col = 0, *ptr_sum, gr_ind = 0;
    int paths_sum = 0;
    heap* hp = NULL;
    p_table* table = NULL;
    rb_node *root = NULL, *to_add = NULL, *max = NULL;
    FILE* fp;
    //in questa variabile sommo tutti i percorsi minimi trovati
    //quando elimino il minimo dalla heap sommo la sua distanza in questa variabile
    ptr_sum = &(paths_sum);
    NIL = createNil();
    root = NIL;
    if((fp = fopen("input_1","r")) == NULL) return -1 ;
    while (fgets(input, 30, fp) != NULL)
    {
        if (isdigit(*input) && input[0] != '0')
        {
            find_sizeGraph_topK(input, info);
            graph = createGraph(info[0]);
            hp = createHeap(info[0]);
            table = createTable(info[0]);
            buff_len = (info[0] * 10) + info[0] + 1;
            buffer = malloc(buff_len * sizeof(char));
        }

        if (strcmp(input, "AggiungiGrafo\n") == 0)
        {
            for (int i = 0; i < info[0]; i++)
            {
                //inizializza table, heap, graph
                gr_col = 0;
                table[gr_row].prev = -1;
                table[gr_row].heap_pos = gr_row;
                if(gr_row == 0){
                    table[gr_row].dist = 0;
                }else{
                    table[gr_row].dist = INFINITY;
                }
                hp->in_heap[gr_row] = gr_row;

                arc_weight = 0;
                charRead = getc_unlocked(fp);

                while (charRead != BackSpace)
                {
                    arc_weight = 0;

                    while ((charRead != BackSpace) && (charRead != ',')){
                        arc_weight = 10*arc_weight + charRead - '0';
                        charRead = getc_unlocked(fp);
                    }

                    if (charRead != BackSpace){
                        charRead = getc_unlocked(fp);
                    }

                    graph[gr_row][gr_col] = arc_weight;
                    gr_col++;

                }
                gr_row++;
            }
            if(strcmp(buffer,"AggiungiGrafo\n") == 0 || strcmp(buffer,"TopK\n") == 0){
                fseek(fp,-strlen(buffer),SEEK_CUR);
            }
            gr_row = 0;
            hp->heap_size = info[0];
            //calcola percorso minimo
            getMinPath(hp,graph,table,ptr_sum);
            //inserisce nel rb tree il risultato solo se la classifica non è piena
            //oppure se la distanza appena calcolata è minore della massima distanza
            //nell'albero
            if(gr_ind < info[1]){
                to_add = malloc(sizeof(rb_node));
                to_add->gr_ind = gr_ind;
                to_add->dist = *ptr_sum;
                root = rbInsert(root,to_add);
            }else{
                max = treeMaximum(root);
                if(*ptr_sum < max->dist){
                    root = rbDelete(root,max);
                    free(max);
                    to_add = malloc(sizeof(rb_node));
                    to_add->gr_ind = gr_ind;
                    to_add->dist = *ptr_sum;
                    root = rbInsert(root,to_add);
                }
            }
            *ptr_sum = 0;
            gr_ind++;
        }
        if (strcmp(input, "TopK\n") == 0){
            if(gr_ind < info[1]){
                to_print = gr_ind;
            }else{
                to_print = info[1];
            }
            printTopK(root);
            printf("\n");
            to_print = 0;
        }
    }
    fclose(fp);
    return 0;
}

//trova dimensione del grafo e della classifica
void find_sizeGraph_topK(char *command, int *info)
{
    int space = 1;
    while (*command)
    {
        if (*command == ' ')
        {
            space++;
            command++;
        }
        else if (isdigit(*command) && (space != 2))
        {
            info[0] = (int)(strtol(command, &command, 10));
        }
        else if ((isdigit(*command)) && (space == 2))
        {
            info[1] = (int)(strtol(command, &command, 10));
        }
        else
        {
            command++;
        }
    }
}

//grafo rappresentato come matrice di interi

int** createGraph(int size){
    int** graph = malloc(size * sizeof(int*));
    for(int i = 0; i < size; i++){
        graph[i] = malloc(size * sizeof(int));
    }
    return graph;
}

//Heap contiene al suo interno la lunghezza della heap stessa e i nodi del grafo
//ordinati in base alla loro distanza dal più piccolo al più grande(con la tipica strutura della heap)

heap* createHeap(int size){
    heap* hp = malloc(sizeof(heap));
    hp->heap_size = size;
    hp->in_heap = malloc(size* sizeof(int));
    hp->heap_size = size;
    return hp;
}

//tabella su cui fare i calcoli per percorso minimo
p_table* createTable(int size){
    return malloc(size* sizeof(struct p_table));
}


//funzione che implementa algoritmo di Dijkstra
void getMinPath(heap *hp, int** graph, p_table* tb, int* ptr_sum)
{
    int new_dist, g_node, dim = hp->heap_size;

    while (hp->heap_size > 0)
    {
        g_node = deleteMin(hp, ptr_sum, tb);
        for(int i = 0; i < dim; i++){
            if(graph[g_node][i] != 0){
                new_dist = tb[g_node].dist + graph[g_node][i];
                if(tb[i].dist > new_dist){
                    tb[i].dist = new_dist;
                    tb[i].prev = g_node;
                    reducePriority(hp,tb[i].heap_pos,tb);
                }
            }
        }
    }
}

void minHeapify(heap *hp, int n, p_table* tb)
{
    int l, r, pos_min, save;
    l = (2*n) + 1;
    r = (2*n) + 2;
    if(l < hp->heap_size && (tb[hp->in_heap[l]].dist) < (tb[hp->in_heap[n]].dist)){
        pos_min = l;
    }else{
        pos_min = n;
    }
    if(r < hp->heap_size && (tb[hp->in_heap[r]].dist) < (tb[hp->in_heap[pos_min]].dist)){
        pos_min = r;
    }
    if(pos_min != n){
        save = hp->in_heap[n];
        hp->in_heap[n] = hp->in_heap[pos_min];
        //aggiorno posizione nella heap del nodo contenuto in posizione n nella heap
        tb[hp->in_heap[n]].heap_pos = n;
        hp->in_heap[pos_min] = save;
        //aggiorno posizione nella heap del nodo contenuto in posizione pos_min nella heap
        tb[hp->in_heap[pos_min]].heap_pos = pos_min;
        minHeapify(hp,pos_min,tb);
    }
}

void reducePriority(heap* hp, int n, p_table* tb){
    int save;
    int par;
    while( n > 0 && tb[hp->in_heap[Parent(n)]].dist > tb[hp->in_heap[n]].dist){
        par = Parent(n);
        save = hp->in_heap[par];
        hp->in_heap[par] = hp->in_heap[n];
        tb[hp->in_heap[par]].heap_pos = par;
        hp->in_heap[n] = save;
        tb[hp->in_heap[n]].heap_pos = n;
        n = par;
    }
}

int deleteMin(heap *hp, int* ptr_sum, p_table* tb)
{
    int min;
    if (hp->heap_size < 1)
    {
        return -1;
    }
    min = hp->in_heap[0];
    //somma la distanza solo se diversa da INFINITY. Se la distanza == INFINITY allora
    //il nodo da eliminare non è collegato al nodo 0. Pertanto la sua distanza da 0 è da considerarsi nulla
    if(tb[min].dist != INFINITY){
        *ptr_sum += tb[min].dist;
    }
    hp->in_heap[0] = hp->in_heap[(hp->heap_size)-1];
    tb[hp->in_heap[0]].heap_pos = 0;
    hp->heap_size = hp->heap_size - 1;
    minHeapify(hp,0,tb);
    return min;
}

//calcola l'indice del parente del nodo contenuto nell'indice n
int Parent(int n){
    float i = (float) n;
    if(n % 2 == 0){
        return (int) ((i/2) - 1);
    }else{
        return (int) ((i/2) - 0.5);
    }
}

rb_node* rbInsert(rb_node* root, rb_node* z){
    rb_node* x = root, *y = NIL;
    while (x != NIL){
        y = x;
        if(z->dist < x->dist){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    z->p = y;
    if(y == NIL){
        root = z;
    }else if(z->dist < y->dist){
        y->left = z;
    }else{
        y->right = z;
    }
    z->left = NIL;
    z->right = NIL;
    z->color = RED;
    rbInsertFix(&root, z);
    return root;
}


void rbInsertFix(rb_node** root, rb_node* z){
    rb_node* y;
    while(z->p->color == RED){
        if(z->p == z->p->p->left){
            y = z->p->p->right;
            if(y->color == RED){
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            }else{
                if(z == z->p->right){
                    z = z->p;
                    leftRotate(root,z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                rightRotate(root, z->p->p);
            }
        }else{
            y = z->p->p->left;
            if(y->color == RED){
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            }else{
                if(z == z->p->left){
                    z = z->p;
                    rightRotate(root,z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                leftRotate(root, z->p->p);
            }

        }
    }
    (*root)->color = BLACK;
}

void leftRotate(rb_node** root, rb_node* x){
    rb_node* y = x->right;
    x->right = y->left;
    if(y->left != NIL){
        y->left->p = x;
    }
    y->p = x->p;
    if(x->p == NIL){
        *root = y;
    }else if(x == x->p->left){
        x->p->left = y;
    }else{
        x->p->right = y;
    }
    y->left = x;
    x->p = y;
}

void rightRotate(rb_node** root, rb_node* y){
    rb_node* x = y->left;
    y->left = x->right;
    if(x->right != NIL){
        x->right->p = y;
    }
    x->p = y->p;
    if(y->p == NIL){
        *root = x;
    }else if(y == y->p->right){
        y->p->right = x;
    }else{
        y->p->left = x;
    }
    x->right = y;
    y->p = x;
}

rb_node* rbDelete(rb_node* root, rb_node* z){
    rb_node* y = z, *x;
    int y_orig_col = y->color;
    if(z->left == NIL){
        x = z->right;
        rbTransplant(&root,z,z->right);
    }else if(z->right == NIL){
        x = z->left;
        rbTransplant(&root,z,z->left);
    }else{
        y = treeMinimum(z->right);
        y_orig_col = y->color;
        x = y->right;
        if(y->p == z){
            x->p = y;
        }else{
            rbTransplant(&root,y,y->right);
            y->right = z->right;
            y->right->p = y;
        }
        rbTransplant(&root,z,y);
        y->left = z->left;
        y->left->p = y;
        y->color = z->color;
    }
    if(y_orig_col == BLACK){
        rbDeleteFix(&root,x);
    }
    return root;
}

void rbTransplant(rb_node** root, rb_node* u, rb_node* v){
    if(u->p == NIL){
        *root = v;
    }else if(u == u->p->left){
        u->p->left = v;
    }else{
        u->p->right = v;
    }
    v->p = u->p;
}

void rbDeleteFix(rb_node** root, rb_node* x){
    rb_node * w;
    while(x != *root && x->color == BLACK){
        if(x == x->p->left){
            w = x->p->right;
            if(w->color == RED){
                w->color = BLACK;
                x->p->color = RED;
                leftRotate(root,x->p);
                w = x->p->right;
            }
            if(w->left->color == BLACK && w->right->color == BLACK){
                w->color = RED;
                x = x->p;
            }else{
                if(w->right->color == BLACK){
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(root,w);
                    w = x->p->right;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->right->color = BLACK;
                leftRotate(root,x->p);
                x = *root;
            }
        }else{
            w = x->p->left;
            if(w->color == RED){
                w->color = BLACK;
                x->p->color = RED;
                rightRotate(root,x->p);
                w = x->p->left;
            }
            if(w->right->color == BLACK && w->left->color == BLACK){
                w->color = RED;
                x = x->p;
            }else{
                if(w->left->color == BLACK){
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(root,w);
                    w = x->p->left;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->left->color = BLACK;
                rightRotate(root,x->p);
                x = *root;
            }
        }
    }
    x->color = BLACK;
}

rb_node* treeMinimum(rb_node* x){
    while(x->left != NIL){
        x = x->left;
    }
    return x;
}

rb_node* treeMaximum(rb_node* x){
    while(x->right != NIL){
        x = x->right;
    }
    return x;
}

rb_node* createNil(){
    rb_node* nil = malloc(sizeof(rb_node));
    nil->color = BLACK;
    return nil;
}

void printTopK(rb_node* root){
    if(root != NIL){
        printTopK(root->left);
        if(to_print != 0){
            if(to_print == 1){
                fprintf(stdout,"%d",root->gr_ind);
                to_print--;
            }else{
                fprintf(stdout,"%d ",root->gr_ind);
                to_print--;
            }
        }
        printTopK(root->right);
    }
}

void freeTree(rb_node* root){
    if(root != NIL){
        freeTree(root->right);
        freeTree(root->left);
        free(root);
    }
}
