/*******************************************************************************
* tic-tac-toe
*
* overkill implementation. bit twiddling edition
*
*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>

static unsigned int p1, p2;

bool won (unsigned int *p) {
  switch (*p) {
  case 7: return true; // bottom row
  case 56: return true; // middle row
  case 448: return true; // top row
  case 292: return true; // left col
  case 146: return true; // middle col
  case 73: return true; // right col
  case 273: return true; // \
  case: 84: return true; // /
  default: return false;
  }
}

bool valid_move (unsigned int *m) {
  return !((p1 | p2) & (1 << *m));
}

void move (unsigned int *p, unsigned int *m) {
  (*p) |= (1 << *m);
  return;
}

void turn (unsigned int *p, unsigned int *m) {
  while (true) {
    scanf("%d", m);
    if (valid_move(m)) {
      move(p, m);
      break;
    } else {
      printf("People sometimes make mistakes.\n");
    }
  }
  return;
}

void endgame() {
  if (won(&p1)) printf("Player 1 won\n");
  else if (won(&p2)) printf("Player 2 won\n");
  else printf("The only winning move is not to play.\n");
}

int main () {
  printf("Shall we play a game?\n");
  unsigned int m;
  while (!won(&p1) && !won(&p2)) {
    turn(&p1, &m);
    turn(&p2, &m);
  }
  endgame();
  return 0;
}
