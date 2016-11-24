

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100
#define MAX_VERTEX 40

typedef struct stack
{
    int data[MAX_SIZE];
    int head;
} stack;

void init(stack *st)
{
    st->head = -1;
}

void push(stack *st, int num)
{
    st->head++;
    st->data[st->head] = num;
}

int pop(stack *st)
{
    return st->data[(st->head)--];
}

int top(stack *st)
{
    return st->data[st->head];
}

int is_empty(stack *st)
{
    if (st->head == -1) return 1;
    else return 0;
}

stack vertex;
int graph[MAX_VERTEX][MAX_VERTEX];
int visited[MAX_VERTEX];
int num_visited;

void dfs(int num_vertex)
{
    int i;
    int top;
    
    num_visited = 0;
    init(&vertex);
    for (i = 0; i < num_vertex; i++)
        visited[i] = 0;
    while (num_visited < num_vertex)
    {
        for (i = 0; i < num_vertex; i++)
            if (!visited[i]) break;
        visited[i] = 1;
        num_visited++;
        push(&vertex, i);
        while (!is_empty(&vertex))
        {
            top = pop(&vertex);
            printf("Visit vertex %d\n", top);
            for (i = 0; i < num_vertex; i++)
            {
                if ((graph[top][i]) && (!visited[i]))
                {
                    visited[i] = 1;
                    num_visited++;
                    push(&vertex, i);
                }
            }
        }
    }
}

int main()
{
    int i, j;
    int num_vertex, num_edge;
    int first, second;
    
    for (i = 0; i < MAX_VERTEX; i++)
        for (j = 0; j < MAX_VERTEX; j++)
            graph[i][j] = 0;
    
    FILE* file;
    file = fopen("graph.txt", "r");
    fscanf(file, "%d", &num_vertex);
    fscanf(file, "%d", &num_edge);
    for (i = 0; i < num_edge; i++)
    {
        fscanf(file, "%d", &first);
        fscanf(file, "%d", &second);
        graph[first][second] = 1;
        graph[second][first] = 1;
    }
    fclose(file);
    dfs(num_vertex);
}





