/*
 * Snake.cpp
 *
 *  Created on: Jun 28, 2017
 *      Author: Ethan
 */

#include "Snake.h"
#include <random>
#include <iostream>
#include <math.h>
class Node;
Snake::Snake(int width, int height) :
		width(width), height(height) {
	board = new char[width * height];
	for (int i = 0; i < width * height; i++)
		board[i] = 0;
	Node* n = new Node(Point(nextInt(width), nextInt(height)), nullptr,
			nullptr);
	first = n;
	last = n;
	board[n->p.x * width + n->p.y] = 2;
	generateFruit();
}

void Snake::generateFruit() {
	while (true) {
		fruit.x = nextInt(width);
		fruit.y = nextInt(height);
		if (board[fruit.x * width + fruit.y] == 0) {
			board[fruit.x * width + fruit.y] = 3;
			break;
		}
	}
}

void Snake::update() {
	lengthUpdated = false;
	Point p = first->p + dirs[dir];
	p.clamp(width, height);
	if (board[p.x * width + p.y] == 1) {
		finished = true;
	} else if (p != fruit && length >= minLength) {
		board[last->p.x * width + last->p.y] = 0;
		board[first->p.x * width + first->p.y] = 1;
		last->prev->next = nullptr;
		last->next = first;
		last->p = p;
		first = last;
		last->next->prev = last;
		last = last->prev;
		first->prev = nullptr;
		board[first->p.x * width + first->p.y] = 2;
	} else {
		board[first->p.x * width + first->p.y] = 1;
		Node* n = new Node(p, nullptr, first);
		first->prev = n;
		first = n;
		board[first->p.x * width + first->p.y] = 2;
		length++;
		lengthUpdated = true;
		if (p == fruit) generateFruit();
	}
}

int Snake::fruitDistance(){
	Point p = first->p;
	for(int i = 0; i < (dir == 0 || dir == 2 ? height : width); i ++, p+= dirs[dir], p.clamp(width, height)){
		if(board[p.x * width + p.y] == 3)
			return i;
	}
	return -1;
}

double Snake::fruitPythagoreanDistance(){
	int dx = first->p.x - fruit.x, dy = first->p.y - fruit.y;
	return sqrt((dx*dx) + (dy*dy));
}

int Snake::bodyDistance(){
	Point p = first->p;
	for(int i = 0; i < (dir == 0 || dir == 2 ? height : width); i ++, p+= dirs[dir], p.clamp(width, height)){
		if(board[p.x * width + p.y] == 1)
			return i;
	}
	return -1;
}

void Snake::printBoard(std::ostream &stream){
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			stream << (int) board[x * width + y] << " ";
		}
		stream << std::endl;
	}
}

int Snake::nextInt(int range) {
	static std::random_device rd; //Will be used to obtain a seed for the random number engine
	static std::mt19937 gen(0); //static_cast<long int>(time(NULL)));//gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	static std::uniform_real_distribution<> dis(0, 1);
	double d = dis(gen);
//	std::cout << d << std::endl;
	return d * range;
}

Snake::~Snake() {
	Node* n = first, *n2;
	while(n != last){
		n2 = n->next;
		delete n;
		n = n2;
	}
	delete last;
	delete[] board;
}
