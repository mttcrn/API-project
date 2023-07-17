//
// Created by motti on 10/07/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Node of the BST
struct station {
    int distance;
    int car_number;
    int cars[512];
    int max_autonomy;
    int visited;
    struct station *left, *right, *parent, *prev;
};

//Node of the path
struct node{
    int distance;
    struct node *next;
};

struct queue {
    struct node* front;
    struct node* rear;
};

//All functions
struct station* find(struct station *x, int distance);
void add_station(struct station **root, int distance,  int car_number, int *autonomy);
void remove_station(struct station **root, int distance);
struct station* treeSuccessor(struct station *x);
struct station* treePredecessor(struct station *x);
struct station* treeMinimum(struct station *x);
struct station* treeMaximum(struct station *x);
void inorder(struct station *root);
void add_car(struct station *root, int distance, int autonomy);
void remove_car(struct station *root, int distance, int autonomy);
int get_max_car(struct station *x);
void clear_prec(struct station *root);
void plan_path_bfs(struct station *root, int start, int end);
void plan_path_bfs_reverse(struct station *root, int start, int end);
void free_tree(struct station **root);

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
            if (fscanf(file, "%d", &distance) != EOF && fscanf(file, "%d", &autonomy) != EOF){
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
    free_tree(&BST);
    return 0;
}

void free_tree(struct station **root){
    if(*root == NULL){
        return;
    }
    free_tree(&(*root)->left);
    free_tree(&(*root)->right);
    free(*root);
}

void inorder(struct station *root){
    if(root!=NULL) {
        inorder(root->left);
        printf(" %d (%d) ", root->distance, get_max_car(root));
        inorder(root->right);
    }
}

void add_station(struct station **root, int distance, int car_number, int *autonomy){
    //Insert the new station in the BST
    struct station *y = NULL;
    struct station *x = *root;
    while(x != NULL){
        y = x;
        if(distance < x->distance){
            x = x->left;
        } else if (distance > x->distance){
            x = x->right;
        } else {
            printf("non aggiunta\n");
            return;
        }
    }

    //Create the node
    struct station *temp;
    temp = (struct station*)malloc(sizeof(struct station));
    temp->distance = distance;
    temp->car_number = car_number;
    int max = 0;
    for(int i = 0; i < car_number; i++){
        temp->cars[i] = autonomy[i];
        if(autonomy[i] > max){
            max = autonomy[i];
        }
    }
    temp->visited = 0;
    temp->max_autonomy = max;
    temp->left = NULL;
    temp->right = NULL;
    temp->prev = NULL;
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
            z->car_number = y->car_number;
            z->max_autonomy = y->max_autonomy;
            for(int i=0; i<y->car_number; i++){
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
    } else {
        x->cars[x->car_number] = autonomy;
        x->car_number = x->car_number + 1;
        if(autonomy > x->max_autonomy){
            x->max_autonomy = autonomy;
        }
        printf("aggiunta\n");
    }
}

void remove_car(struct station *root, int distance, int autonomy){
    struct station *x = find(root, distance);
    if(x == NULL){
        printf("non rottamata\n");
        return;
    } else {
        if (autonomy == x->max_autonomy) {
            int new_max = 0;
            int k = -1;
            for (int i = 0; i < x->car_number; i++) {
                if (x->cars[i] == autonomy) {
                    k = i;
                }
                if (x->cars[i] > new_max && x->cars[i] != x->max_autonomy) {
                    new_max = x->cars[i];
                }
            }
            if (k != x->car_number - 1) {
                x->cars[k] = x->cars[x->car_number - 1];
            }
            x->cars[x->car_number - 1] = 0;
            x->car_number = x->car_number - 1;
            x->max_autonomy = new_max;
            printf("rottamata\n");
            return;
        } else {
            for (int i = 0; i < x->car_number; i++) {
                if (x->cars[i] == autonomy) {
                    if (i != x->car_number - 1) {
                        x->cars[i] = x->cars[x->car_number - 1];
                    }
                    x->cars[x->car_number - 1] = 0;
                    x->car_number = x->car_number - 1;
                    printf("rottamata\n");
                    return;
                }
            }
        }
    }
    printf("non rottamata\n");
}

int get_max_car(struct station* x){
    int max = x->cars[0];
    for(int i = 0; i<x->car_number; i++){
        if(x->cars[i] > max){
            max = x->cars[i];
        }
    }
    return max;
}

void clear_prec(struct station *root){
    if(root == NULL){
        return;
    }
    root->prev = NULL;
    root->visited = 0;
    clear_prec(root->left);
    clear_prec(root->right);
}

void enqueue(struct queue *queue, int distance){
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->distance = distance;
    temp->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = temp;
        return;
    }

    queue->rear->next = temp;
    queue->rear = temp;
}

void dequeue(struct queue *queue){
    if (queue->front == NULL){
        return;
    }

    struct node* temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL){
        queue->rear = NULL;
    }

    free(temp);
}

void insert_in_path(struct node **path, int distance){
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->distance = distance;
    temp->next = *path;
    *path = temp;
}

void print_path_reverse(struct node *path){
    while(path != NULL){
        printf("%d ", path->distance);
        path = path->next;
    }
}

void print_path(struct node *path){
    if(path == NULL){
        return;
    }
    print_path(path->next);
    printf("%d ", path->distance);
}

void free_path(struct node **path){
    while (*path != NULL) {
        struct node *temp = *path;
        *path = (*path)->next;
        free(temp);
    }
}

void free_queue(struct queue *queue){
    struct node* current = queue->front;
    while (current != NULL) {
        struct node* next = current->next;
        free(current);
        current = next;
    }
    queue->front = NULL;
    queue->rear = NULL;
}

void plan_path_bfs(struct station *root, int start, int end){
    //inorder(root);
    clear_prec(root);
    struct station *curr = find(root, start);
    struct station *next = treeSuccessor(curr);
    struct queue queue = { NULL, NULL };
    struct node* path = NULL;
    enqueue(&queue, curr->distance);
    curr->prev = NULL;
    curr->visited = 1;

    while(queue.front != NULL){
        curr = find(root, queue.front->distance);
        if(curr->distance == end){
            while(curr != NULL){
                insert_in_path(&path, curr->distance);
                curr = curr->prev;
            }
            print_path_reverse(path);
            free_path(&path);
            free_queue(&queue);
            return;
        }
        while(next != NULL && curr->distance + curr->max_autonomy >= next->distance){
            if(next->distance > end){
                break;
            }
            if(next->visited == 0){
                next->visited = 1;
                enqueue(&queue, next->distance);
                next->prev = curr;
            }
            next = treeSuccessor(next);
        }
        dequeue(&queue);
    }

    printf("nessun percorso");
    free_queue(&queue);
}

void plan_path_bfs_reverse(struct station *root, int start, int end){
    clear_prec(root);
    struct station *curr = find(root, end);
    struct station *next;
    struct queue queue = { NULL, NULL };
    enqueue(&queue, curr->distance);
    curr->prev = NULL;
    curr->visited = 1;

    while(queue.front != NULL){
        curr = find(root, queue.front->distance);
        next = treeSuccessor(curr);
        if(curr->distance == start){
            while(curr != NULL){
                printf("%d ", curr->distance);
                curr = curr->prev;
            }
            free_queue(&queue);
            return;
        }
        while(next != NULL){
            if(next->distance > start){
                break;
            }
            if(next->distance - next->max_autonomy <= curr->distance && next->visited == 0){
                enqueue(&queue, next->distance);
                next->visited = 1;
                if(next->prev == NULL){
                    next->prev = curr;
                    //break;
                }
            }
            next = treeSuccessor(next);
        }
        dequeue(&queue);
    }

    printf("nessun percorso");
    free_queue(&queue);
}

