/*
 * Neuron.h
 *
 *  Created on: Jun 6, 2017
 *      Author: Ethan
 */

#pragma once
#include "Gene.h"
#include <vector>
#include <memory>
#include <cmath>
class Genome;

class Neuron {
		typedef std::shared_ptr<Genome> GenomePtr;
		typedef std::unique_ptr<Gene> GenePtr;
		typedef std::unique_ptr<Neuron> NeuronPtr;
		typedef std::pair<int, Gene*> GenePair;
	public:
		Neuron(int _id, bool isOutput, int outputId = -1);
		Neuron(){}
		void load(Genome* g, std::ifstream &in);
		virtual ~Neuron();
		bool marked = false;
		int id, outputId;
		std::vector<GenePtr> genes; //Incoming Genes
		double value = 0.0;
		void evaluate();
		void cloneFrom(Genome* genome, int maxNeuron, Neuron* neuron);
		void print(std::ostream &strm);
		void dumpToFile(std::ofstream &stream) const;
		bool isOutput;
	private:
		double sigmoid(double x){
			return 2.0/(1.0 + exp(-4.9 * x)) - 1.0;
		}
		inline Neuron* toRawPtr(NeuronPtr& ptr){
			return ((Neuron*)ptr.get());
		}
};

