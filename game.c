/*******************************************************************************
* tic-tac-toe
*
* overkill implementation. bit twiddling edition
*
*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct state {
  unsigned int mine;
  unsigned int yours;
  bool scored;
  double score;
};

struct node {
  struct state *s;
  struct node *child;
  struct node *sibling;
};

struct stack_node {
  struct node *key;
  struct stack_node *next;
};

struct stack_node * init_stack () {
  struct stack_node * head = (struct stack_node *)
    malloc(sizeof(struct stack_node));
  head->next = 0;
  return head;
}

bool stack_is_empty (struct stack_node *head) {
  return head->next == 0;
}

struct node * pop (struct stack_node *head) {
  struct node *ret = head->next->key;
  head->next = head->next->next;
  return ret;
}

struct node * peek (struct stack_node *head) {
  return head->next->key;
}


void push (struct stack_node *head, struct node *k) {
  struct stack_node * x = (struct stack_node *)
    malloc(sizeof(struct stack_node));
  x->key = k;
  x->next = head->next;
  head->next = x;
  return;
}

bool won (unsigned int *p) {
  if (*p & 1) {
    if ((*p & 2) && (*p & 4)) return true; // bottom row
    if ((*p & 16) && (*p & 256)) return true; // up diag
    if ((*p & 8) && (*p & 64)) return true; // left col

  }
  if (*p & 8) {
    if ((*p & 16) && (*p & 32)) return true; // middle row
  }
  if (*p & 64) {
    if ((*p & 128) && (*p & 256)) return true; // top row
    if ((*p & 16) && (*p & 4)) return true; // down diag
    return false;
  }
  if (*p & 128) {
    if ((*p & 16) && (*p & 2)) return true; // middle col
  }
  if (*p & 256) {
    if ((*p & 32) && (*p & 4)) return true; // middle col
  }
  return false;
}

bool draw (unsigned int *p1, unsigned int *p2) {
  return (*p1 | *p2) == 511;
}

bool valid_move (int *p1, int *p2, unsigned int *m) {
  return !((*p1 | *p2) & (1 << *m)) && *m < 9;
}

void move (unsigned int *p, unsigned int *m) {
  *p |= (1 << *m);
  return;
}

void turn (unsigned int *p1, unsigned int *p2, unsigned int *m) {
  while (true) {
    scanf("%d", m);
    if (valid_move(p1, p2, m)) {
      move(p1, m);
      break;
    } else {
      printf("People sometimes make mistakes.\n");
    }
  }
  return;
}

bool endgame (unsigned int *p1, unsigned int *p2) {
  //  printf("considering %d %d\n", *p1, *p2);
  return won(p1) || won(p2)
    || draw(p1, p2);
}

struct node * init_game_tree () {
  struct node *head = (struct node *) malloc(sizeof(struct node));
  head->child = 0; head->sibling = 0;
  return head;
}

void print_board (int *p1, int *p2) {
  char ret[9];
  for (int i = 0; i < 9; ++i) {
    if (*p1 & (1 << i)) ret[i] = 'X';
    else if (*p2 & (1 << i)) ret[i] = 'O';
    else ret[i] = ' ';
  }
  printf("%c|%c|%c\n", ret[6], ret[7], ret[8]);
  printf("-----\n");
  printf("%c|%c|%c\n", ret[3], ret[4], ret[5]);
  printf("-----\n");
  printf("%c|%c|%c\n", ret[0], ret[1], ret[2]);
}

void push_possible_moves (struct stack_node *head_sn,
                          struct node *parent_n,
                          unsigned int *p1, unsigned int *p2) {
  struct node * prev_child_n = 0;
  for (int i = 0; i < 9; ++i)
    if (valid_move(p1, p2, &i)) {
      struct stack_node * child_sn =
        (struct stack_node *) malloc(sizeof(struct stack_node));
      struct node * child_n = (struct node *) malloc(sizeof(struct node));
      struct state * child =
        (struct state *) malloc(sizeof(struct state));
      child->scored = false;
      child->mine = *p2; child->yours = *p1; // swap players each level
      move(&(child->mine), &i);
      child_n->s = child; child_n->child = 0; child_n->sibling = 0;
      child_sn->key = child_n;
      if (prev_child_n)
        prev_child_n->sibling = child_n;
      else
        parent_n->child = child_n;
      prev_child_n = child_n;
      //      printf("%d is a valid move!\n", i);
      push(head_sn, child_n);
    }
  return;
}

double score (int *p1, int *p2) {
  if (won(p1)) return 1.0;
  else if (won(p2)) return -1.0;
  else return 0.0;
}

double score_children (struct node *p) {
  double sum = 0;
  int count = 0;
  struct node * c = p->child;
  for (; c != 0; c = c->sibling, ++count) {
    sum += c->s->score;
  }
  return sum / (double) count;
}

struct node * build_game_tree (int *p1, int *p2) {
  struct node *head = init_game_tree();
  struct state *state = (struct state *) malloc(sizeof(struct state));
  state->mine = *p1; state->yours = *p2;
  head->s = state;
  struct stack_node *stack_head = init_stack();
  struct stack_node *upper_stack_head = init_stack();
  struct node *popped;
  bool possible_moves[9];
  push(stack_head, head);
  while(!stack_is_empty(stack_head)) {
    popped = pop(stack_head);
    //    print_board(&(popped->s->mine), &(popped->s->yours));
    if (!endgame(&(popped->s->mine), &(popped->s->yours))) {
      push_possible_moves(stack_head, popped,
                          &(popped->s->mine), &(popped->s->yours));
      push(upper_stack_head, popped);
    }
    else {
      popped->child = 0;
      popped->s->score =
        score(&(popped->s->mine), &(popped->s->yours));
      popped->s->scored = true;
      //      printf("score: %f\n", popped->s->score);
    }
  }
  while(!stack_is_empty(upper_stack_head)) {
    popped = pop(upper_stack_head);
    popped->s->score = score_children(popped);
    print_board(&(popped->s->mine), &(popped->s->yours));
    printf("score: %f\n", popped->s->score);
  }
  return head;
}

int main () {
  printf("Shall we play a game?\n");
  unsigned int p1, p2;
  unsigned int m;
  int x = 5; int y = 256;
  //int x = 261; int y = 0;
  /* int x = 341; int y = 170; */
  struct node * head = build_game_tree(&x, &y);
  printf("built tree\n");
  /* score_game_tree(head); */
  //printf("%d", endgame(&x, &y));
  /* while (!endgame(&p1, &p2)) { */
  /*   turn(&p1, &p2, &m); */
  /* } */
  return 0;
}
