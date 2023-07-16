//
// Created by motti on 10/07/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Node of the BST
struct station {
    int distance;
    int cars[512];
    struct station *left, *right, *parent, *prec;
};

//Node of the path
struct node{
    int distance;
    struct node *next;
};

//All functions
struct station* find(struct station *x, int distance);
void add_station(struct station **root, int distance, int car_number, int *autonomy);
void remove_station(struct station **root, int distance);
struct station* treeSuccessor(struct station *x);
struct station* treePredecessor(struct station *x);
struct station* treeMinimum(struct station *x);
struct station* treeMaximum(struct station *x);
void inorder(struct station *root);
void add_car(struct station *root, int distance, int autonomy);
void remove_car(struct station *root, int distance, int autonomy);
int get_max_car(struct station *x);
void plan_linear(struct station *root, int start, int end);
void plan_path(struct station *root, int start, int end);
void plan_path_reverse(struct station *root, int start, int end);
void insert_in_path(struct node **path, int distance);
void remove_from_path(struct node **path, int distance);
void print_path(struct node *path);
void print_path_reverse(struct node *path);
void simplify_path(struct node *head, int MAX);
void plan_path_bfs(struct station *root, int start, int end);
void clear_prec(struct station *root);
void plan_path_bfs_reverse(struct station *root, int start, int end);

int main(){
    struct station *BST = NULL;
    FILE *file = stdin;
    char line[20];

    if(file == NULL){
        perror("Errore nell'apertura del file");
        return 1;
    }

    while(fscanf(file, "%s", line) != EOF){
        if(strcmp(line, "aggiungi-stazione") == 0){
            int distance, car_number;
            int aut[512];
            for(int i=0; i<512; i++){
                aut[i] = 0;
            }
            if (fscanf(file, "%d", &distance) != EOF && fscanf(file, "%d", &car_number) != EOF){
                for(int i = 0; i<car_number; i++){
                    if (fscanf(file, "%d", &aut[i]) == EOF) break;
                }
                add_station(&BST, distance, car_number, aut);
            }
        } else if(strcmp(line, "demolisci-stazione") == 0){
            int distance;
            if (fscanf(file, "%d", &distance) != EOF){
                remove_station(&BST, distance);
            }
        } else if (strcmp(line, "aggiungi-auto") == 0){
            int distance, autonomy;
            if ( fscanf(file, "%d", &distance) != EOF && fscanf(file, "%d", &autonomy) != EOF){
                add_car(BST, distance, autonomy);
            }
        } else if (strcmp(line, "rottama-auto") == 0){
            int distance, autonomy;
            if (fscanf(file, "%d", &distance) != EOF && fscanf(file, "%d", &autonomy) != EOF){
                remove_car(BST, distance, autonomy);
            }
        } else if (strcmp(line, "pianifica-percorso") == 0){
            int start, end;
            if (fscanf(file, "%d", &start) != EOF && fscanf(file, "%d", &end) != EOF){
                if(start < end) {
                    plan_path_bfs(BST, start, end);
                    printf("\n");
                } else {
                    plan_path_bfs_reverse(BST, start, end);
                    printf("\n");
                }
            }
        }
    }

    fclose(file);
    free(BST);
    return 0;
}

void inorder(struct station *root){
    if(root!=NULL) {
        inorder(root->left);
        printf(" %d (%d) ", root->distance, get_max_car(root));
        inorder(root->right);
    }
}

void add_station(struct station **root, int distance, int car_number, int *autonomy){
    //If the station is already present
    if(find(*root, distance) != NULL){
       printf("non aggiunta\n");
       return;
    }

    //Create the node
    struct station *temp;
    temp = malloc(sizeof(struct station));
    temp->distance = distance;
    for(int i = 0; i < 512; i++){
        temp->cars[i] = autonomy[i];
    }
    temp->left = NULL;
    temp->right = NULL;
    temp->prec = NULL;
    temp->parent = NULL;

    //Insert the new station in the BST
    struct station *y = NULL;
    struct station *x = *root;
    while(x != NULL){
        y = x;
        if(temp->distance < x->distance){
            x = x->left;
        } else {
            x = x->right;
        }
    }

    temp->parent = y;
    if(y == NULL){
        *root = temp;
    } else if(temp->distance < y->distance) {
        y->left = temp;
    } else {
        y->right = temp;
    }
    printf("aggiunta\n");
}

void remove_station(struct station **root, int distance){
    struct station *z = find(*root, distance);
    if(z == NULL){
        printf("non demolita\n");
    } else {
        struct station *y, *x;
        if (z->left == NULL || z->right == NULL) {
            y = z;
        } else {
            y = treeSuccessor(z);
        }

        if (y->left != NULL) {
            x = y->left;
        } else {
            x = y->right;
        }

        if (x != NULL) {
            x->parent = y->parent;
        }
        if (y->parent == NULL) {
            *root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }

        if (y != z) {
            z->distance = y->distance;
            for(int i=0; i<512; i++){
                z->cars[i] = y->cars[i];
            }
        }

        free(y);
        printf("demolita\n");
    }
}

struct station* treeMinimum(struct station *x){
    while(x->left != NULL){
        x = x->left;
    }
    return x;
}


struct station* treeMaximum(struct station *x){
    while(x->right != NULL){
        x = x->right;
    }
    return x;
}

struct station* treeSuccessor(struct station *x){
    if(x->right != NULL){
        return treeMinimum(x->right);
    }
    struct station *y;
    y = x->parent;
    while(y != NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

struct station* treePredecessor(struct station *x){
    if(x->left != NULL){
        return treeMaximum(x->left);
    }
    struct station *y;
    y = x->parent;
    while(y != NULL && x == y->left){
        x = y;
        y = y->parent;
    }
    return y;
}

struct station* find(struct station *x, int distance){
    if (x == NULL || x->distance == distance){
        return x;
    }
    if (x->distance > distance){
        return find(x->left, distance);
    } else {
        return find(x->right, distance);
    }
}

void add_car(struct station *root, int distance, int autonomy){
    struct station *x = find(root, distance);
    if(x == NULL){
        printf("non aggiunta\n");
        return;
    } else {
        for(int i = 0; i<512; i++){
            if(x->cars[i] == 0){
                x->cars[i] = autonomy;
                printf("aggiunta\n");
                return;
            }
        }
    }
    printf("non aggiunta\n");
}

void remove_car(struct station *root, int distance, int autonomy){
    struct station *x = find(root, distance);
    if(x == NULL){
        printf("non rottamata\n");
        return;
    } else {
        for(int i = 0; i<512; i++){
            if(x->cars[i] == autonomy){
                x->cars[i] = 0;
                printf("rottamata\n");
                return;
            }
        }
    }
    printf("non rottamata\n");
}

/*
void plan_linear(struct station* root, int start, int end){
        struct node *path = NULL;
        struct station *x = find(root, start);
        //printf(" %d ", x->distance);
        insert_in_path(&path, x->distance, get_max_car(x));
        int car_autonomy = get_max_car(x);
        struct station *y = treeSuccessor(x);

        if(y->distance == end){
            if(y->distance >= x->distance + car_autonomy){
                printf("nessun percorso");
                return;
            }
        }

        while(y->distance < end){
            if(y->distance <= x->distance + car_autonomy){
                insert_in_path(&path, y->distance, get_max_car(y));
            }
            x = y;
            car_autonomy = get_max_car(x);
            y = treeSuccessor(x);
        }

        insert_in_path(&path, y->distance, get_max_car(y));
        print_path(path);
        printf("\n");
        //simplify_path(path, end-start+1);
        free(path);
}

void build_path(struct node *head, struct node **path){
    struct node *current = head;
    struct node *prev = NULL;

    while (current != NULL) {
        struct node *next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *path = prev;
}

void simplify_path(struct node *head, int MAX) {
        struct node *curr, *near, *path;
        int dist, length;

        // Calcola la lunghezza della lista
        length = 0;
        curr = head;
        while (curr != NULL) {
            length++;
            curr -> min_distance = MAX;
            curr = curr->next;
        }

        // Inizializza la distanza minima per il nodo di partenza
        head -> min_distance = 0;

        // Aggiorna le distanze dei nodi adiacenti
        for (int i = 0; i < length; i++) {
            curr = head;
            while (curr != NULL) {
                near = curr->next;
                while (near != NULL) {
                    dist = curr->min_distance + abs(curr->distance - near->distance);
                    if (dist < near->min_distance && curr->distance + curr->max_autonomy < near->distance) {
                        near->min_distance = dist;
                    }
                    near = near->next;
                }
                curr = curr->next;
            }
        }

        // Costruisci il percorso più corto
        build_path(head, &path);
        print_path(path);
}

void plan_path_reverse(struct station* root, int start, int end){
    inorder(root);
    struct node *path = NULL;
    struct station *curr = find(root, start);
    struct station *prec = treePredecessor(curr);
    struct station *pprec = treePredecessor(prec);
    int first = 0;

    while(prec != NULL && prec->distance > end) {
        if(first == 0){
            insert_in_path(&path, start, get_max_car(curr));
            first = 1;
        }
        if(curr->distance - get_max_car(curr) > prec->distance){
            printf("nessun percorso");
            return;
        }
        while(prec != NULL && curr->distance - get_max_car(curr) <= prec->distance){
            pprec = treePredecessor(prec);
            if(pprec == NULL || prec == NULL || prec->distance < end){
                printf("nessun percorso");
                return;
            }
            if (prec -> distance >= end && pprec->distance < end){
                break;
            }
            if(curr->distance - get_max_car(curr) > pprec->distance){
                insert_in_path(&path, prec->distance, get_max_car(prec));
                curr = prec;
                prec = pprec;
                break;
            }
            prec = treePredecessor(prec);
        }
    }

    if(prec == NULL || curr->distance - get_max_car(curr) > prec->distance){
        printf("nessun percorso");
        return;
    }

    insert_in_path(&path, prec->distance, get_max_car(prec));
    print_path_reverse(path);
    printf("\n");
    free(path);
}

void plan_path(struct station* root, int start, int end){
    //inorder(root);
    struct node *path = NULL;
    struct station *curr = find(root, end);
    struct station *prec = treePredecessor(curr);
    struct station *pprec = treePredecessor(prec);
    int first = 0;

    while(prec != NULL && prec->distance > start){
        if(first == 0){
            insert_in_path(&path, end, get_max_car(curr));
            first = 1;
        }
        if(prec->distance + get_max_car(prec) < curr->distance){
            prec = treePredecessor(prec);
            pprec = treePredecessor(pprec);
        }
        while(prec->distance + get_max_car(prec) >= curr->distance && prec!=NULL){
            pprec = treePredecessor(prec);
            if(prec->distance < start || prec == NULL || pprec == NULL){
                printf("nessun percorso");
                return;
            }
            if (pprec->distance + get_max_car(pprec) < curr->distance) {
                insert_in_path(&path, prec->distance, get_max_car(prec));
                curr = prec;
                prec = pprec;
                break;
            }
            prec = treePredecessor(prec);
        }
    }

    if(prec->distance + get_max_car(prec) < curr->distance){
        printf("nessun percorso");
        return;
    }
    insert_in_path(&path, prec->distance, get_max_car(prec));
    print_path(path);
    printf("\n");
    free(path);
}*/

int get_max_car(struct station* x){
    int max = x->cars[0];
    for(int i = 0; i<512; i++){
        if(x->cars[i] > max){
            max = x->cars[i];
        }
    }
    return max;
}

void insert_in_path(struct node **path, int distance){
    struct node *temp = malloc(sizeof(struct node));
    temp->distance = distance;
    temp->next = NULL;
    if(*path == NULL){
        *path = temp;
        return;
    } else {
        struct node *punt = *path;
        while(punt->next != NULL){
            punt = punt->next;
        }
        punt->next = temp;
    }
}

void print_path(struct node *path){
    if(path == NULL){
        return;
    }
    print_path(path->next);
    printf("%d ", path->distance);
}

void print_path_reverse(struct node *path){
    while(path != NULL){
        printf("%d ", path->distance);
        path = path->next;
    }
}

int is_in_path(struct node* path, int distance){
    while(path != NULL){
        if(path->distance == distance){
            return 1;
        }
        path = path->next;
    }
    return 0;
}

void remove_from_path(struct node** path, int distance){
    struct node *curr = *path;
    struct node *prec = NULL;

    while(curr != NULL && curr->distance != distance){
        prec = curr;
        curr = curr->next;
    }

    if(curr != NULL){
        if(prec == NULL){
            *path = curr->next;
        } else {
            prec -> next = curr->next;
        }
    }

    free(curr);
}

void clear_prec(struct station *root){
    if(root == NULL){
        return;
    }
    root->prec = NULL;
    clear_prec(root->left);
    clear_prec(root->right);
}

void plan_path_bfs(struct station *root, int start, int end){
    //inorder(root);
    clear_prec(root);
    struct station *curr = find(root, start);
    struct station *next = treeSuccessor(curr);
    struct node* path = NULL;
    insert_in_path(&path, curr->distance);
    curr->prec = NULL;

    while(path != NULL && curr != NULL){
        curr = find(root, path->distance);
        if(curr->distance == end){
            break;
        }
        while(next != NULL && curr->distance + get_max_car(curr) >= next->distance){
            if(next->distance > end){
                break;
            }
            if(is_in_path(path, next->distance) == 0){
                insert_in_path(&path, next->distance);
                next->prec = curr;
            }
            next = treeSuccessor(next);
        }
        remove_from_path(&path, curr->distance);
    }

    path = NULL;
    if (curr != NULL && curr->distance == end){
        while(curr != NULL){
            insert_in_path(&path, curr->distance);
            curr = curr->prec;
        }
    } else {
        printf("nessun percorso");
        return;
    }

    if(is_in_path(path, start) == 1){
        print_path(path);
    } else {
        printf("nessun percorso");
    }

    free(path);
}

void plan_path_bfs_reverse(struct station *root, int start, int end){
    clear_prec(root);
    struct station *curr = find(root, end);
    struct station *next;
    struct node* path = NULL;
    insert_in_path(&path, curr->distance);
    curr->prec = NULL;

    while(path != NULL && curr != NULL){
        curr = find(root, path->distance);
        next = treeSuccessor(curr);
        if(curr->distance == start){
            break;
        }
        while(next != NULL){
            if(next->distance > start){
                break;
            }
            if(next->distance - get_max_car(next) <= curr->distance && is_in_path(path, next->distance) == 0){
                insert_in_path(&path, next->distance);
                if(next->prec == NULL){
                    next->prec = curr;
                }
            }
            next = treeSuccessor(next);
        }
        remove_from_path(&path, curr->distance);
    }

    path = NULL;
    if (curr != NULL && curr->distance == start){
        while(curr != NULL){
            insert_in_path(&path, curr->distance);
            curr = curr->prec;
        }
    } else {
        printf("nessun percorso");
        return;
    }

    if(is_in_path(path, end) == 1){
        print_path_reverse(path);
    } else {
        printf("nessun percorso");
    }

    free(path);
}
