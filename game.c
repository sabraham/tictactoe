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

const double WIN_VALUE = 1.0;
const double DRAW_VALUE = 0.5;
const double LOSS_VALUE = 0.0;

struct state {
  unsigned int s[2];
  double score;
  unsigned int player;
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

bool won (unsigned int p) {
  if (p & 1) {
    if ((p & 2) && (p & 4)) return true; // bottom row
    if ((p & 16) && (p & 256)) return true; // up diag
    if ((p & 8) && (p & 64)) return true; // left col
  }
  if (p & 8) {
    if ((p & 16) && (p & 32)) return true; // middle row
  }
  if (p & 64) {
    if ((p & 128) && (p & 256)) return true; // top row
    if ((p & 16) && (p & 4)) return true; // down diag
  }
  if (p & 128) {
    if ((p & 16) && (p & 2)) return true; // middle col
  }
  if (p & 256) {
    if ((p & 32) && (p & 4)) return true; // right col
  }
  return false;
}

bool no_openings (unsigned int p1, unsigned int p2) {
  return (p1 | p2) == 511;
}

bool valid_move (unsigned int p1, unsigned int p2, unsigned int m) {
  return !((p1 | p2) & (1 << m)) && m < 9;
}

unsigned int move (unsigned int p, unsigned int m) {
  return p | (1 << m);
}

unsigned int turn (unsigned int p1, unsigned int p2, unsigned int m) {
  while (true) {
    scanf("%d", &m);
    if (valid_move(p1, p2, m))
      break;
    else
      printf("People sometimes make mistakes.\n");
  }
  return move(p1, m);;
}

bool endgame (unsigned int p1, unsigned int p2) {
  return won(p1) || won(p2)
    || no_openings(p1, p2);
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
                          struct node *parent_n) {
  unsigned int p1 = parent_n->s->s[0], p2 = parent_n->s->s[1];
  struct node *prev_child_n = 0;
  for (int i = 0; i < 9; ++i)
    if (valid_move(p1, p2, i)) {
      struct stack_node * child_sn =
        (struct stack_node *) malloc(sizeof(struct stack_node));
      struct node * child_n = (struct node *) malloc(sizeof(struct node));
      struct state * child =
        (struct state *) malloc(sizeof(struct state));
      child->s[0] = p1; child->s[1] = p2;
      child->player = (parent_n->s->player + 1) % 2;
      child->s[child->player] = move(child->s[child->player], i);
      child_n->s = child; child_n->child = 0; child_n->sibling = 0;
      child_sn->key = child_n;
      if (prev_child_n)
        prev_child_n->sibling = child_n;
      else
        parent_n->child = child_n;
      prev_child_n = child_n;
      push(head_sn, child_n);
    }
  return;
}

double score (unsigned int p1, unsigned int p2) {
  if (won(p1)) return LOSS_VALUE;
  else if (won(p2)) return WIN_VALUE;
  else return DRAW_VALUE;
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

struct node * build_game_tree (int p1, int p2) {
  struct node *head = init_game_tree();
  struct state *state = (struct state *) malloc(sizeof(struct state));
  state->player = 0;
  state->s[0] = p1; state->s[1] = p2;
  head->s = state;
  struct stack_node *stack_head = init_stack();
  struct stack_node *upper_stack_head = init_stack();
  struct node *popped;
  bool possible_moves[9];
  push(stack_head, head);
  while(!stack_is_empty(stack_head)) {
    popped = pop(stack_head);
    if (!endgame(popped->s->s[0], popped->s->s[1])) {
      push_possible_moves(stack_head, popped);
      push(upper_stack_head, popped);
    }
    else {
      popped->child = 0;
      popped->s->score =
        score(popped->s->s[0], popped->s->s[1]);
    }
  }
  while(!stack_is_empty(upper_stack_head)) {
    popped = pop(upper_stack_head);
    popped->s->score = score_children(popped);
  }
  return head;
}

struct node * choose_between_equal_scores (struct node *node1,
                                           struct node *node2) {
  // right now return first -- TODO choose randomly
  return node1;
}

struct node * optimal_child (struct node *head) {
  struct node *c = head->child;
  struct node *ret = c;
  double best_score = c->s->score;
  for (struct node *sib = c->sibling; sib != 0; sib = sib->sibling) {
    double sib_score = sib->s->score;
    if (sib_score > best_score) {
      best_score = sib_score;
      ret = sib;
    } else if (sib_score == best_score) {
      ret = choose_between_equal_scores(ret, sib);
    }
  }
  return ret;
}

unsigned int ai_play (int p1, int p2) {
  struct node *game_tree = build_game_tree(p1, p2);
  struct node *optimal = optimal_child(game_tree);
  return optimal->s->s[1];
}

int main () {
  printf("Shall we play a game?\n");
  unsigned int p1 = 0, p2 = 0;
  unsigned int m;
  print_board(&p1, &p2);
  while (!endgame(p1, p2)) {
    p1 = turn(p1, p2, m);
    printf("\n");
    printf("Player 1's move\n");
    print_board(&p1, &p2);
    if (endgame(p1, p2)) break;
    p2 = ai_play(p1, p2);
    printf("\n");
    printf("Computer's move\n");
    print_board(&p1, &p2);
    printf("\n");
  }
  return 0;
}
