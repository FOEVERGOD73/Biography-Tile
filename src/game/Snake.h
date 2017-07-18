/*
 * Snake.h
 *
 *  Created on: Jun 28, 2017
 *      Author: Ethan
 */

#pragma once
#include <iostream>

class Snake {
	public:
		Snake(int width, int height);
		virtual ~Snake();

		int getDir() const {
			return dir;
		}
		
		void setDir(int dir) {
			if (0 <= dir && dir <= 3) this->dir = dir;
		}
		int fruitDistance();
		double fruitPythagoreanDistance();
		int bodyDistance();
		void printBoard(std::ostream &stream);
		void update();
		bool isFinished() const {
			return finished;
		}

		int getLength() const {
			return length;
		}
		
		char* getBoard() const {
			return board;
		}

		bool lengthUpdated = false;
	private:
		class Node;
		class Point {
			public:
				Point(int x, int y) :
						x(x), y(y) {

				}
				Point() {

				}
				Point(const Point &p) {
					x = p.x;
					y = p.y;
				}
				int x, y;

				void clamp(int width, int height) {
					if (x < 0) x = width - 1;
					if (x >= width) x = 0;
					if (y < 0) y = height - 1;
					if (y >= height) y = 0;
				}

				Point operator+(Point &p) {
					return Point(x + p.x, y + p.y);
				}
				void operator+=(Point &p) {
					x += p.x;
					y += p.y;
				}

				bool operator==(Point &p) {
					return (x == p.x) && (y == p.y);
				}

				bool operator!=(Point &p) {
					return !operator ==(p);
				}
				
		};
		int width, height;
		char* board; //0 blank, 1 body 2 head 3 fruit;
		int length = 1;
		const static int minLength = 3;
		Point fruit;
		int dir = 0; //0 +y 1 +x 2 -y 3 -x
		static int nextInt(int range);
		void generateFruit();
		bool finished = false;
		Node* first, *last;
		Point dirs[4] = { Point(0, 1), Point(1, 0), Point(0, -1), Point(-1, 0) };

		class Node {
			public:
				Point p;
				Node* prev, *next;
				Node(Point p, Node* prev, Node* next) :
						p(p), prev(prev), next(next) {
				}
		};

};

