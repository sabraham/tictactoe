# Tic-Tac-Toe (with AI)

## How to run

Simply run make

```bash
make
```

and then execute the binary

```bash
./game
```

## What's inside

A tic-tac-toe game with a computer opponent. Not interesting, you say? Trivial by using a lookup table? Yes, you're absolutely correct! In an effort to add some elegance through complexity, the implementation here does search the state space by building a game tree.

## Anything else?

Yes -- the AI is not efficient, but not in a way that affects gameplay. Currently, after each move, the game tree is rebuilt from the current state. The only TODO is to fix this and free up the nodes that are no longer needed.

