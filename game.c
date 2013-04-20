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

struct player {
  char name[10];
  unsigned int state;
  struct player *next;
};

bool won (unsigned int *p) {
  switch (*p) {
  case 7: return true; // bottom row
  case 56: return true; // middle row
  case 448: return true; // top row
  case 292: return true; // left col
  case 146: return true; // middle col
  case 73: return true; // right col
  case 273: return true; // down diag
  case 84: return true; // up diag
  default: return false;
  }
}

bool draw (unsigned int *p1, unsigned int *p2) {
  return (*p1 | *p2) == 511;
}

bool valid_move (struct player *head, unsigned int *m) {
  return !((head->state | head->next->state) & (1 << *m)) && *m < 9;
}

void move (struct player *p, unsigned int *m) {
  (p->state) |= (1 << *m);
  return;
}

void turn (struct player *head, unsigned int *m) {
  while (true) {
    scanf("%d", m);
    if (valid_move(head, m)) {
      move(head, m);
      break;
    } else {
      printf("People sometimes make mistakes.\n");
    }
  }
  printf("%d\n", head->state);
  return;
}

bool endgame (struct player *head) {
  return won(&(head->state)) || won(&(head->next->state))
    || draw(&(head->state), &(head->next->state));
}

void who_won(struct player *head) {
  if (won(&(head->state))) printf("%s won\n", head->name);
  else if (won(&(head->next->state))) printf("%s won\n", head->next->name);
  else printf("The only winning move is not to play.\n");
}

int main () {
  printf("Shall we play a game?\n");
  struct player *p1 = (struct player *) malloc(sizeof(struct player));
  struct player *p2 = (struct player *) malloc(sizeof(struct player));
  p1->state = 0; p1->next = p2; p2->state = 0; p2->next = p1;
  strcpy(p1->name,  "Player 1"); strcpy(p2->name, "Player 2");
  struct player *head = p1;
  unsigned int m;
  while (!endgame(head)) {
    turn(head, &m);
    head = head->next;
  }
  who_won(head);
  return 0;
}
