//
// Created by Caterina Motti on 10/07/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Node of the BST
struct station {
    int distance;
    int car_number;
    int cars[512];
    int max_autonomy;
    int visited;
    struct station *left, *right, *parent, *prev;
};

//Node of the queue
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
struct station* treeMinimum(struct station *x);
void inorder(struct station *root);
void add_car(struct station *root, int distance, int autonomy);
void remove_car(struct station *root, int distance, int autonomy);
void clear_prev(struct station *root);
void plan_path_bfs(struct station *root, int start, int end);
void plan_path_bfs_reverse(struct station *root, int start, int end);
void free_tree(struct station **root);

int main(){
    struct station *BST = NULL;
    char line[20];

    char c;
    int i;
    while((c = getc_unlocked(stdin)) != EOF){

        for(i=0; i<19 && c!=' ' && c!=EOF; i++){
            line[i] = c;
            c = getc_unlocked(stdin);
        }
        line[i] = '\0';

        if(strcmp(line, "aggiungi-stazione") == 0){
            int aut[512];
            for(i=0; i<512; i++){
                aut[i] = 0;
            }
            int distance = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                distance = distance * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            int car_number = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                car_number = car_number * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            for(i = 0; i<car_number; i++){
                while(!isdigit(c)){
                    c = getc_unlocked(stdin);
                }
                while(isdigit(c)){
                    aut[i] = aut[i] * 10 + (c - '0');
                    c = getc_unlocked(stdin);
                }
            }
            add_station(&BST, distance, car_number, aut);
        } else if(strcmp(line, "demolisci-stazione") == 0){
            int distance = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                distance = distance * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            remove_station(&BST, distance);
        } else if (strcmp(line, "aggiungi-auto") == 0){
            int distance = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                distance = distance * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            int autonomy = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                autonomy = autonomy * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            add_car(BST, distance, autonomy);
        } else if (strcmp(line, "rottama-auto") == 0){
            int distance = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                distance = distance * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            int autonomy = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                autonomy = autonomy * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            remove_car(BST, distance, autonomy);
        } else if (strcmp(line, "pianifica-percorso") == 0){
            int start = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                start = start * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            int end = 0;
            while(!isdigit(c)){
                c = getc_unlocked(stdin);
            }
            while(isdigit(c)){
                end = end * 10 + (c - '0');
                c = getc_unlocked(stdin);
            }
            if(start < end) {
                plan_path_bfs(BST, start, end);
                printf("\n");
            } else {
                plan_path_bfs_reverse(BST, start, end);
                printf("\n");
            }
        }
    }

    free_tree(&BST);
    return 0;
}

/**Prints the BST in order (from minimum to maximum value).*/
void inorder(struct station *root){
    if(root!=NULL) {
        inorder(root->left);
        printf("%d (%d)", root->distance, root->max_autonomy);
        inorder(root->right);
    }
}

/**Insert a new station at the given distance in the BST*/
void add_station(struct station **root, int distance, int car_number, int *autonomy){
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

    //create the node
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

/**Remove a station at the given distance from the BST.*/
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

/**Returns a pointer to the minimum of the BST.*/
struct station* treeMinimum(struct station *x){
    while(x->left != NULL){
        x = x->left;
    }
    return x;
}

/**Returns a pointer to the successor of the given node.*/
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

/**Returns a pointer to a station given its distance.*/
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

/**Adds a car to the station at the specified distance..*/
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

/**Remove a car from the station at the specified distance.*/
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

/**It resets all prev pointers and visited value of each node of the BST.*/
void clear_prev(struct station *root){
    if(root == NULL){
        return;
    }
    root->prev = NULL;
    root->visited = 0;
    clear_prev(root->left);
    clear_prev(root->right);
}

/**Enqueues a new node to the given queue.*/
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

/**Dequeues a node from the given queue.*/
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

/**It print the given path from start to end (with start < end)*/
void print_path_reverse(struct station *path){
    if(path == NULL){
        return;
    }
    print_path_reverse(path->prev);
    printf("%d ", path->distance);
}

/**Frees the given queue.*/
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

/**Frees the given BST.*/
void free_tree(struct station **root){
    if(*root == NULL){
        return;
    }
    free_tree(&(*root)->left);
    free_tree(&(*root)->right);
    free(*root);
}

/**BFS algorithm: find the shortest path from start to end in which start < end.*/
void plan_path_bfs(struct station *root, int start, int end){
    clear_prev(root);
    struct station *curr = find(root, start);
    struct station *next = treeSuccessor(curr);;
    struct queue queue = {NULL, NULL};
    enqueue(&queue, curr->distance);
    curr->prev = NULL;
    curr->visited = 1;

    while(queue.front != NULL){
        curr = find(root, queue.front->distance);
        if(curr->distance == end){
            print_path_reverse(curr);
            free_queue(&queue);
            return;
        }
        while(next != NULL && curr->distance + curr->max_autonomy >= next->distance){
            if(next->distance > end){
                break;
            }
            if(next->visited == 0){
                enqueue(&queue,next->distance);
                next->visited = 1;
                next->prev = curr;
            }
            next = treeSuccessor(next);
        }
        dequeue(&queue);
    }

    printf("nessun percorso");
    free_queue(&queue);
}

/**BFS algorithm: find the shortest path from start to end in which start > end.*/
void plan_path_bfs_reverse(struct station *root, int start, int end){
    clear_prev(root);
    struct station *curr = find(root, end);
    struct station *next, *succ = treeSuccessor(curr);
    struct queue queue = {NULL, NULL};
    enqueue(&queue, curr->distance);
    curr->prev = NULL;
    curr->visited = 1;
    int flag;

    while(queue.front != NULL){
        curr = find(root, queue.front->distance);
        if(curr->distance == start){
            while(curr != NULL){
                printf("%d ", curr->distance);
                curr = curr->prev;
            }
            free_queue(&queue);
            return;
        }
        next = succ;
        flag = 1;
        while(next != NULL){
            if(next->distance > start){
                break;
            }
            if(next->visited == 0 && curr->distance >= next->distance - next->max_autonomy) {
                enqueue(&queue, next->distance);
                next->visited = 1;
                next->prev = curr;
                if(flag == 1){
                    flag = 0;
                    succ = next;
                }
            }
            next = treeSuccessor(next);
        }
        dequeue(&queue);
    }

    printf("nessun percorso");
    free_queue(&queue);
}

