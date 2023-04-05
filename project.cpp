#include <iostream>
#include <chrono> // For time elapsed measurement
#include <pthread.h>

typedef int value_t;


typedef struct Node
{
    value_t data;
    struct Node *next;
} StackNode;

pthread_mutex_t mutex_lock; 

// Stack function declarations
void push(value_t v, StackNode **top); // Push a value onto the stack
value_t pop(StackNode **top); // Pop a value off the stack
int is_empty(StackNode *top); // Check if the stack is empty

void *testStack(void *arg); // Function to test the stack

int main()
{
    pthread_mutex_init(&mutex_lock, NULL); 

    StackNode *top = NULL; // Initialize the top of the stack

    pthread_t threads[200]; 

    auto start_time = std::chrono::high_resolution_clock::now(); // Record start time

    // Create 200 threads to test the stack
    for (int i = 0; i < 200; i++) {
        pthread_create(&threads[i], NULL, testStack, (void *)&top);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 200; i++) {
        pthread_join(threads[i], NULL);
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // Record end time
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); // Calculate time elapsed

    std::cout << "Time elapsed: " << elapsed_time.count() 
        << " milliseconds" << std::endl; // Print time elapsed

    pthread_mutex_destroy(&mutex_lock); 

    return 0;
}

// Stack function definitions
void push(value_t v, StackNode **top)
{
    pthread_mutex_lock(&mutex_lock); 

    StackNode *new_node = (StackNode *)malloc(sizeof(StackNode)); // Allocate memory for a new node

    new_node->data = v; // Set the node's data to the value to be pushed
    new_node->next = *top; // Set the node's next pointer to the current top of the stack
    *top = new_node; // Set the top of the stack to the new node

    std::cout << "Pushed: " << v << std::endl; // Print the value that was pushed

    pthread_mutex_unlock(&mutex_lock); 
}

value_t pop(StackNode **top)
{
    pthread_mutex_lock(&mutex_lock); 

    if (is_empty(*top)) { // Check if the stack is empty
        pthread_mutex_unlock(&mutex_lock); 
        return (value_t)0; // If the stack is empty, return 0
    }

    value_t data = (*top)->data; // Get the value at the top of the stack
    StackNode *temp = *top; // Create a temporary pointer to the top of the stack
    *top = (*top)->next; // Set the top of the stack to the next node

    free(temp); // Free the memory for the node that was popped

    std::cout << "Popped: " << data << std::endl; // Print the value that was popped

    pthread_mutex_unlock(&mutex_lock); 

    return data; // Return the value that was popped
}

// Check if the stack is empty
int is_empty(StackNode *top)
{
    if (top == NULL)
        return 1;
    else
        return 0;
}

// Function executed by threads to test the stack
void *testStack(void *arg)
{
    StackNode **top = (StackNode **)arg; // Cast the argument as a pointer to a pointer to a StackNode

    for (int i = 0; i < 500; i++) {
        push(i, top); // Push i onto the stack
        push(i + 1, top); // Push i+1 onto the stack
        push(i + 2, top); // Push i+2 onto the stack

        pop(top); // Pop the top element from the stack
        pop(top); 
        pop(top); 
    }

    pthread_exit(NULL); 
}