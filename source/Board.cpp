#include "../include/Board.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <iostream>

int notempty(SDL_Color color) {
	if (color.g != 25) return 1;
	return 0;
}
// black board
Board::Board() : curPiece() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) board[i][j] = grey;
	}
}

Piece Board::getcurPiece() { return curPiece; }

void Board::setcurPiece(Piece p) { curPiece = p; }

void Board::gravity(int i) {
	if (i >= 0) {
		for (int k = i; k >= 0; k--)
			for (int j = 0; j < WIDTH; j++) {
				board[k][j] = board[k - 1][j];
			}
		for (int j = 0; j < WIDTH; j++) {
			board[0][j] = grey;
		}
	}
	return;
}

// looks for a completed line
int Board::line() {
	for (int i = 0; i < HEIGHT; i++) {
		bool res = true;
		for (int j = 0; j < WIDTH; j++) {
			if (!notempty(board[i][j])) res = false;
		}
		if (res == true) return i;
	}

	return -1;
}

bool Board::update(int move, SDL_Renderer* renderer,
				   pair<int, int>* correct_line) {
	int x = curPiece.getX();
	int y = curPiece.getY();
	Shape s = curPiece.getShape();

	if (move == LEFT) {
		bool b = true;
		for (int i = 0; i < s.size; i++)
			for (int j = 0; j < s.size; j++)
				if (s.matrix[i][j]) {
					if (x + j - 1 < 0)
						b = false;
					else if (notempty(board[i + y][j + x - 1]))
						b = false;
				}
		if (b) curPiece.setX(x - 1);
	}

	if (move == RIGHT) {
		bool b = true;
		for (int i = 0; i < s.size; i++)
			for (int j = 0; j < s.size; j++)
				if (s.matrix[i][j]) {
					if (x + j + 1 == WIDTH)
						b = false;
					else if (notempty(board[i + y][j + x + 1]))
						b = false;
				}
		if (b) curPiece.setX(x + 1);
	}

	if (move == DOWN) {
		bool b = true;
		for (int i = 0; i < s.size; i++)
			for (int j = 0; j < s.size; j++)
				if (s.matrix[i][j]) {
					if (i + y + 1 == HEIGHT)
						b = false;
					else if (notempty(board[i + y + 1][j + x]))
						b = false;
				}
		if (b) curPiece.setY(y + 1);
	}

	if (move == UP) {
		rotate2();
	}

	bool b2 = absorb();
	while (line() != -1) {
		gravity(line());
		correct_line->first++;
		correct_line->second++;
	}
	return b2;
}

bool Board::fit(Piece p) {
	Shape s = p.getShape();
	int x = p.getX();
	int y = p.getY();
	bool res = true;
	for (int i = 0; i < s.size; i++)
		for (int j = 0; j < s.size; j++)
			if (s.matrix[i][j]) {
				if (i + y < HEIGHT && i + y >= 0 && x + j < WIDTH &&
					x + j >= 0) {
					if (notempty(board[i + y][j + x])) res = false;
				} else
					res = false;
			}
	return res;
}

void Board::rotate2() {
	Shape s = curPiece.getShape();
	Shape s2 = rotate(s);
	int x = curPiece.getX();
	int y = curPiece.getY();
	Piece p = {s2, x, y};
	bool b = fit(p);
	if (b) curPiece.setShape(s2);
}

void Board::draw_board(SDL_Renderer* renderer, int border) {
	SDL_Rect rect;
	rect.w = tile_size - 2;
	rect.h = tile_size - 2;

	for (int x = 0; x < WIDTH; x++)
		for (int y = 0; y < HEIGHT; y++) {
			SDL_SetRenderDrawColor(renderer, board[y][x].r, board[y][x].g,
								   board[y][x].b, 255);
			rect.x = x * tile_size + 2 + border;
			rect.y = y * tile_size;
			SDL_RenderFillRect(renderer, &rect);
		}
}
// absorb and replace in collision
bool Board::absorb() {
	int x = curPiece.getX();
	int y = curPiece.getY();
	Shape s = curPiece.getShape();
	bool b = false;
	for (int i = 0; i < s.size; i++)
		for (int j = 0; j < s.size; j++)
			if (s.matrix[i][j]) {
				if (i + y + 1 < HEIGHT) {
					if (notempty(board[i + y + 1][j + x])) b = true;
				} else
					b = true;
			}
	if (b) {
		for (int i = 0; i < s.size; i++)
			for (int j = 0; j < s.size; j++)
				if (s.matrix[i][j]) {
					board[i + y][j + x] = s.color;
				}
		Piece p;
		curPiece = p;

		if (!fit(p)) {	// game over
			return false;
		}
	}
	return true;
}

void Board::gravity_piece(SDL_Renderer* renderer,
						  pair<int, int>* correct_line) {
	update(DOWN, renderer, correct_line);
}

void Board::shift() {
	for (int i = 0; i < HEIGHT; ++i) board[i] = board[i + 1];
}

void Board::clear_line(int line) {
	int hole = rand() % WIDTH - 1;

	for (int j = 0; j < WIDTH; ++j) board[line][j] = clear_grey;
	board[line][hole] = grey;
}

void Board::adjust_board(int nb_line) {
	switch (nb_line) {
		case 1:
			shift();
			clear_line(HEIGHT - 1);
			break;
		case 3:
			shift();
			shift();
			clear_line(HEIGHT - 1);
			clear_line(HEIGHT - 2);
			break;

		case 4:
			shift();
			shift();
			shift();
			shift();
			clear_line(HEIGHT - 1);
			clear_line(HEIGHT - 2);
			clear_line(HEIGHT - 3);
			clear_line(HEIGHT - 4);
			break;
		default:;
	}
}
