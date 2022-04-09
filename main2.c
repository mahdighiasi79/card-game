#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int n, m, k;
int created = 0, expanded = 0;

typedef struct card {
    int number;
    char color;
} Card;

typedef struct stack {
    int length;
    Card *cards;
} Stack;

typedef struct action {
    int source;
    int destination;
} Action;

typedef struct node {
    Stack *stacks;
    Action parentAction;
    struct node *parentNode;
} Node;

Card Pop(Stack *stack) {

    stack->length--;
    Card output = stack->cards[stack->length];
    stack->cards = (Card *) realloc(stack->cards, stack->length * sizeof(Card));
    return output;
}

void Push(Stack *stack, Card element) {

    stack->length++;
    stack->cards = (Card *) realloc(stack->cards, stack->length * sizeof(Card));
    stack->cards[stack->length - 1] = element;
}

Action *AddAction(Action *actions, Action element, int *length) {

    if (*length == 0) {

        actions = (Action *) malloc(sizeof(Action));
        *length = 1;
        actions[0] = element;
        return actions;
    }

    (*length)++;
    actions = (Action *) realloc(actions, (*length) * sizeof(Action));
    actions[(*length) - 1] = element;
    return actions;
}

Action *ActionGenerator(Node node) {

    int length = 0;
    Action *actions = (Action *) malloc(sizeof(Action));

    for (int i = 0; i < k; i++) {

        Stack stackI = node.stacks[i];

        for (int j = 0; j < k; j++) {

            Stack stackJ = node.stacks[j];

            if (stackI.cards[stackI.length - 1].number < stackJ.cards[stackJ.length - 1].number) {
                Action element;
                element.source = i;
                element.destination = j;
                actions = AddAction(actions, element, &length);
            }
        }
    }

    Action last;
    last.destination = 0;
    last.source = 0;
    actions = AddAction(actions, last, &length);

    return actions;
}

Node NodeCreation(Node node) {

    Node output;
    output.stacks = (Stack *) malloc(k * sizeof(Stack));

    for (int i = 0; i < k; i++) {

        output.stacks[i].cards = (Card *) malloc(node.stacks[i].length * sizeof(Stack));
        output.stacks[i].length = node.stacks[i].length;

        for (int j = 0; j < node.stacks[i].length; j++) {

            output.stacks[i].cards[j].color = node.stacks[i].cards[j].color;
            output.stacks[i].cards[j].number = node.stacks[i].cards[j].number;
        }
    }

    return output;
}

Node ChildNode(Node parent, Action action) {

    Node child = NodeCreation(parent);
    Card c = Pop(&child.stacks[action.source]);
    Push(&child.stacks[action.destination], c);
    return child;
}

bool GoalTest(Node node) {

    Stack temp;
    int nonEmptyStacks = 0;

    for (int i = 0; i < k; i++) {

        temp = node.stacks[i];

        if (temp.length == n + 1) {

            for (int j = n; j > 1; j--) {
                if ((temp.cards[j].number != n - (j - 1)) || (temp.cards[j].color != temp.cards[j - 1].color))
                    return false;
            }

            if (temp.cards[1].number != n)
                return false;

            nonEmptyStacks++;
        }
    }

    if (nonEmptyStacks == m)
        return true;
    else
        return false;
}

Node DLS(Node initial, int limit) {

    Node failure;
    failure.stacks = NULL;
    failure.parentAction.destination = 0;

    Node cutoff;
    cutoff.stacks = NULL;
    cutoff.parentAction.destination = 1;

    created++;

    if (GoalTest(initial))
        return initial;
    else if (limit == 0)
        return cutoff;
    else {

        Node *node = (Node *) malloc(sizeof(Node));
        *node = initial;

        bool cutoff_occurred = false;

        Action *actions = ActionGenerator(initial);
        Action action = actions[0];

        expanded++;

        int i = 1;

        while ((action.destination != 0) || (action.source != 0)) {

            Node child = ChildNode(*node, action);
            child.parentNode = (struct node *) malloc(sizeof(struct node));
            child.parentNode = node;
            child.parentAction = action;

            Node result = DLS(child, limit - 1);
            if ((result.stacks == NULL) && (result.parentAction.destination == 1))
                cutoff_occurred = true;
            else if (result.stacks != NULL)
                return result;

            action = actions[i];
            i++;
        }

        if (cutoff_occurred)
            return cutoff;
        else
            return failure;
    }
}

Node IDS(Node initial, int depth) {

    Node result;

    for (int i = depth; i < 999999999; i++) {

        result = DLS(initial, i);

        if ((result.stacks != NULL) || (result.parentAction.destination != 1))
            return result;
    }

    result.stacks = NULL;
    result.parentAction.destination = 0;
    return result;
}

void PrintNode(Node node) {

    for (int i = 0; i < k; i++) {

        if (node.stacks[i].length == 1)
            printf("#\n");
        else {
            for (int j = 1; j < node.stacks[i].length; j++)
                printf("%d%c ", node.stacks[i].cards[j].number, node.stacks[i].cards[j].color);
            printf("\n");
        }
    }
}

void DisplayResult(Node goal) {

    if (goal.stacks == NULL) {

        printf("failure\n");
        return;
    }

    Action *actions = (Action *) malloc(sizeof(Action));
    int length = 0;
    Node *node;
    node = &goal;
    if (node->parentNode != NULL) {
        actions[0] = node->parentAction;
        length++;
    }

    while (node->parentNode != NULL) {

        node = node->parentNode;
        length++;
        actions = (Action *) realloc(actions, length * sizeof(Action));
        actions[length - 1] = node->parentAction;
    }

    if (length == 0) {
        printf("depth of answer: %d\n", length);
    } else {
        printf("depth of answer: %d\n", length - 1);
        for (int i = length - 2; i >= 0; i--)
            printf("source of action: %d       destination of action: %d\n", actions[i].source + 1,
                   actions[i].destination + 1);
    }

    printf("the goal reached by the algorithm:\n");
    PrintNode(goal);
    printf("number of expanded nodes: %d\n", expanded);
    printf("number of created nodes: %d\n", created);
}

int *AddSpace(int *spaces, int element, int *length) {

    (*length)++;
    spaces = (int *) realloc(spaces, (*length) * sizeof(int));
    spaces[(*length) - 1] = element;
    return spaces;
}

int main() {

    scanf("%d %d %d", &k, &m, &n);

    Node initial;
    initial.stacks = (Stack *) malloc(k * sizeof(Stack));

    int *spaces;
    int length;
    int start;
    int end;
    int number;
    int temp;

    char *buff = (char *) malloc(sizeof(char));

    gets(buff);

    for (int i = 0; i < k; i++) {

        gets(buff);

        if (buff[0] == '#') {

            initial.stacks[i].cards = (Card *) malloc(sizeof(Card));
            Card card;
            card.number = 999999999;
            card.color = '#';
            initial.stacks[i].cards[0] = card;
            initial.stacks[i].length = 1;

        } else {

            spaces = (int *) malloc(sizeof(int));
            spaces[0] = -1;
            length = 1;

            for (int j = 0; j < strlen(buff); j++) {

                if (buff[j] == ' ')
                    AddSpace(spaces, j, &length);
            }

            AddSpace(spaces, strlen(buff), &length);

            initial.stacks[i].cards = (Card *) malloc(length * sizeof(Card));
            Card card;
            card.number = 999999999;
            card.color = '#';
            initial.stacks[i].cards[0] = card;
            initial.stacks[i].length = length;

            for (int j = 0; j < length - 1; j++) {

                start = spaces[j] + 1;
                end = spaces[j + 1] - 1;

                number = 0;

                for (int l = start; l < end; l++) {

                    temp = (int) buff[l];
                    temp -= 48;
                    number *= 10;
                    number += temp;
                }

                initial.stacks[i].cards[j + 1].number = number;
                initial.stacks[i].cards[j + 1].color = buff[end];
            }
        }
    }

    int depth;
    scanf("%d", &depth);

    initial.parentAction.destination = 0;
    initial.parentAction.source = 0;
    initial.parentNode = NULL;

    Node goal = IDS(initial, depth);

    DisplayResult(goal);

    return 0;
}