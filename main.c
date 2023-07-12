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
    struct station *left, *right, *parent;
};

struct node{
    int distance;
    struct node *next;
};

//All functions
struct station* find(struct station *x, int distance);
void add_station(struct station **root, int distance, int car_number, int autonomy[512]);
void remove_station(struct station *root, int distance);
struct station* treeSuccessor(struct station *x);
struct station* treeMinimum(struct station *x);
void inorder(struct station *root);
void add_car(struct station *root, int distance, int autonomy);
void remove_car(struct station *root, int distance, int autonomy);
int get_max_car(struct station *x);
void plan_path(struct station *root, int start, int end);
void insert_in_path(struct node **path, int distance);
void print_path(struct node *path);

int main(){
    struct station *BST = NULL;
    int aut[512];
    for(int i=0; i<512; i++){
        aut[i] = 0;
    }
    FILE *file = stdin;
    char line[20];

    if(file == NULL){
        perror("Errore nell'apertura del file");
        return 1;
    }

    while(fscanf(file, "%s", line) != EOF){
        if(strcmp(line, "aggiungi-stazione") == 0){
            int distance, car_number;
            fscanf(file, "%d", &distance);
            fscanf(file, "%d", &car_number);
            for(int i = 0; i<car_number; i++){
                fscanf(file, "%d", &aut[i]);
            }
            add_station(&BST, distance, car_number, aut);
        } else if(strcmp(line, "demolisci-stazione") == 0){
            int distance;
            fscanf(file, "%d", &distance);
            remove_station(BST, distance);
        } else if (strcmp(line, "aggiungi-auto") == 0){
            int distance, autonomy;
            fscanf(file, "%d", &distance);
            fscanf(file, "%d", &autonomy);
            add_car(BST, distance, autonomy);
        } else if (strcmp(line, "rottama-auto") == 0){
            int distance, autonomy;
            fscanf(file, "%d", &distance);
            fscanf(file, "%d", &autonomy);
            remove_car(BST, distance, autonomy);
        } else if (strcmp(line, "pianifica-percorso") == 0){
            int start, end;
            fscanf(file, "%d", &start);
            fscanf(file, "%d", &end);
            plan_path(BST, start, end);
        }
    }

    inorder(BST);

    fclose(file);
    free(BST);
    return 0;
}

void inorder(struct station *root){
    if(root!=NULL) {
        inorder(root->left);
        printf(" %d ", root->distance);
        inorder(root->right);
    }
}

void add_station(struct station **root, int distance, int car_number, int autonomy[512]){
    //If the station is already present
    if(find(*root, distance) != NULL){
        printf("non aggiunta \n");
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
    printf("aggiunta \n");
}

void remove_station(struct station *root, int distance){
    struct station *z = find(root, distance);
    if(z == NULL){
        printf("non demolita \n");
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
            root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }

        if (y != z) {
            z->distance = y->distance;
        }

        free(y);
        printf("demolita \n");
    }
}

struct station* treeMinimum(struct station *x){
    while(x->left != NULL){
        x = x->left;
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
        printf("non aggiunta \n");
    } else {
        for(int i = 0; i<512; i++){
            if(x->cars[i] == 0){
                x->cars[i] = autonomy;
                printf("aggiunta \n");
                return;
            }
        }
    }
    printf("non aggiunta \n");
}

void remove_car(struct station *root, int distance, int autonomy){
    struct station *x = find(root, distance);
    if(x == NULL){
        printf("non rottamata \n");
    } else {
        for(int i = 0; i<512; i++){
            if(x->cars[i] == autonomy){
                x->cars[i] = 0;
                printf("rottamata \n");
                return;
            }
        }
    }
    printf("non rottamata \n");
}

void plan_path(struct station* root, int start, int end){
    struct node *path = NULL;
    struct station *x = find(root, start);
    //printf(" %d ", x->distance);
    int car_autonomy = get_max_car(x);
    struct station *y = treeSuccessor(x);
    int first = 0;

    while(y->distance < end){
        if(y->distance <= x->distance + car_autonomy){
            if(first == 0){
                insert_in_path(&path, start);
                first = 1;
            }
            insert_in_path(&path, y->distance);
        }
        x = y;
        car_autonomy = get_max_car(x);
        y = treeSuccessor(x);
    }

    if(path == NULL){
        printf("nessun percorso \n");
        return;
    }

    insert_in_path(&path, y->distance);
    print_path(path);
    free(path);
}

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
    while(path != NULL){
        printf("%d ", path->distance);
        path = path->next;
    }
    printf("\n");
}