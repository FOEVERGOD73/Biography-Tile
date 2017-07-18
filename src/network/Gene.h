/*
 * Gene.h
 *
 *  Created on: Jun 6, 2017
 *      Author: Ethan
 */

#pragma once
#include "Neuron.h"
#include <memory>
#include <iostream>

class Neuron;
struct Gene {
		typedef std::unique_ptr<Gene> GenePtr;
	public:
		Neuron* into;
		Neuron* out;
		double weight;
		bool enabled;
		int innovation;
		bool back;
		~Gene(){
//			std::cout << "Gene Deleted" << std::endl;
		}
	private:
};

