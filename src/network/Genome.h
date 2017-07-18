/*
 * Genome.h
 *
 *  Created on: Jun 6, 2017
 *      Author: Ethan
 */

#pragma once
#include "Neuron.h"
#include <vector>
#include <random>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <utility>

class Pool;

class Genome {
		typedef std::unique_ptr<Gene> GenePtr;
		typedef std::unique_ptr<Neuron> NeuronPtr;
		typedef std::pair<int, Gene*> GenePair;
		typedef std::unique_ptr<Genome> GenomePtr;
	public:
		Genome(Pool* p, Genome* seed, bool mutate = false);
		Genome(Pool* p, int _maxNeuron);
		Genome(Pool* p, Genome* g1, Genome* g2, bool mutate = false);
		Genome(Pool* p, std::ifstream &in);
		virtual ~Genome();

		int getFitness() const {
			return fitness;
		}
		
		void setFitness(int fitness) {
			this->fitness = fitness;
		}

		void increaseFitness(int fitness){
			this->fitness += fitness;
		}

		double** evaluate(double* inputs);

		void print(std::ostream &strm);

		const std::unordered_map<int, Gene*> getGenes() const {
			return genes;
		}
		
		bool operator<(const Genome& other) const{
			return fitness < other.fitness;
		}

		static bool pointerOperator(const GenomePtr &a, const GenomePtr &b){
			return a->fitness > b->fitness;
		}
		
		static bool rawPointerOperator(const Genome* a, const Genome* b){
			return a->fitness > b->fitness;
		}

		int getGlobalRank() const {
			return globalRank;
		}
		
		void setGlobalRank(int globalRank = 0) {
			this->globalRank = globalRank;
		}

		void dumpToFile(std::ofstream &stream) const;

		int id;
		bool toDelete = false;
	private:
		static int ids;
		static const int DEFAULT_GENES_PER_NEURON = 1;
		static const int INPUT_NEURONS = 5, OUTPUT_NEURONS = 4;
		static const int CONNECTIONS = 0, LINK = 1, BIAS = 2, NODE = 3, ENABLE =
				4, DISABLE = 5, STEP = 6, PERTURBCHANCE = 7, RATES = 8;
		double rates[RATES] = {0.25, 2.0, 0.4, 0.5, 0.4, 0.2, 0.1, 0.9};
		double* INPUTS[INPUT_NEURONS];
		double* OUTPUTS[OUTPUT_NEURONS];
		Pool* pool;

		int fitness = 0;
		int globalRank = 0;
		std::unordered_map<int, Gene*> genes;

		NeuronPtr* neurons; //Array of Neuron Pointers;
		int maxNeuron;

		void addGene(Gene* gene);
		bool find(const Neuron &source, const Neuron &target) const; //true if target is child of source
		int mark(Neuron &y, int yid, int xid);
		void sort(int minId, int length);

		//Mutate Methods
		void mutate();
		void pointMutate();
		void linkMutate(bool forceBias);
		void nodeMuate();
		void enableDisableMutate(bool enable);
		NeuronPtr& randomNeuron(bool nonInput);
		void createGenome();

//		int newInnovation();

		void initializeNeurons();

		void cloneFrom(Genome* seed);

		//Node Type
		bool isHidden(Neuron* x);
		bool isOutput(Neuron* x);
		bool isHidden(Neuron &x);
		bool isOutput(Neuron &x);
		bool isHidden(int x);
		bool isOutput(int x);
		Neuron* getOutput(int id);
		void cloneRatesFrom(Genome* seed);

		inline Neuron* toRawPtr(NeuronPtr& ptr){
			return ((Neuron*)ptr.get());
		}

		friend void Neuron::cloneFrom(Genome* genome, int maxNeuron, Neuron* neuron);
		friend void Neuron::load(Genome*, std::ifstream&);
};

