/*
 * Display.h
 *
 *  Created on: Jul 1, 2017
 *      Author: Ethan
 */

#pragma once
#include <list>
#include <thread>
#include "../network/Pool.h"

class Display {
	public:
		Display(Pool* p);
		void feedState(int generation, int genomeId, char* board);
		void run();
		virtual ~Display();
	private:
		std::list<int> metadata;
		std::list<char> data;
		std::thread* t;
		void drawState();
		Pool* p;
};

