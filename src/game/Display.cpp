/*
 * Display.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: Ethan
 */

#include "Display.h"
#include "../network/Pool.h"
#include <iostream>
#include <thread>
#include <chrono>
#define LINE_SEPERATOR "|-|-|-|-|-|-|-|-|-|-|"

Display::Display(Pool* p):p(p) {
	t = std::thread(run);
}

void Display::feedState(int generation, int genomeId, char* board){
	metadata.push_back(generation);
	metadata.push_back(genomeId);
	for(int i = 0; i < 100; i++){
		data.push_back(board[i]);
	}
	drawState();
}

void Display::run(){
    using namespace std::chrono_literals;
    while(this->metadata.front() < p->getGeneration()){
    	metadata.pop_front();
    	metadata.pop_front();
    	for(int i = 0; i < 100; i ++)
    		data.pop_front();
    }
    drawState();
	std::this_thread::sleep_for(0.5s);
}

void Display::drawState(){
	using namespace std;
		for(int i = 0; i< 20; i ++)
			cout << endl;
		cout << "NEAT of a Neural Network to play Snake" << endl;
	if(data.size() != 0){
		cout << "Generation: " << metadata.front();
		metadata.pop_front();
		cout << ", Genome: " << metadata.front() << endl;
		metadata.pop_front();

		for(int y = 0; y < 10; y ++){
			cout << LINE_SEPERATOR << endl;
			for(int x = 0; x < 10; x ++){
				cout << "|" << (int)data.front();
				data.pop_front();
			}
			cout << "|" << endl;
		}
		cout << LINE_SEPERATOR << endl;
	}else{
		cout << "CREATING NEW GENERATION...." << endl;
	}
}

Display::~Display() {
}

