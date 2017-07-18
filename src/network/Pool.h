/*
 * Pool.h
 *
 *  Created on: Jun 15, 2017
 *      Author: Ethan
 */

#pragma once
#include <vector>
#include <thread>
//#include <set>
#include <fstream>
#include "Species.h"
#include "Genome.h"
#include <memory>

class Pool {
		typedef std::unique_ptr<Genome> GenomePtr;
		typedef std::unique_ptr<Species> SpeciesPtr;
	public:
		Pool(int outputs, void (*evaluate) (int, int, Genome*));
		Pool(int outputs, void (*evaluateAll) (int, std::vector<GenomePtr>*, int*));
		virtual ~Pool();
		static const int Population = 300;

		bool start();
		bool startFromFile();
		bool stop();

		void print(std::ostream &strm);

		int getGeneration() const {
			return generation;
		}
		void setGeneration(int generation = 0) {
			this->generation = generation;
		}
		int getInnovation() const {
			return innovation;
		}
		void setInnovation(int innovation) {
			this->innovation = innovation;
		}
		int getMaxFitness() const {
			return maxFitness;
		}
		void setMaxFitness(int maxFitness = 0) {
			this->maxFitness = maxFitness;
		}
		void loadFromFile();
		const std::vector<SpeciesPtr>& getSpecies() const {
			return species;
		}
		int newInnovation(){
			return innovation++;
		}
	private:
		std::vector<SpeciesPtr> species;
//		std::ofstream stream, stream2;
		int generation = 0;
		int innovation;
		int maxFitness = 0;
		static constexpr double DeltaDisjoint = 2.0;
		static constexpr double DeltaWeights = 0.4;
		static constexpr double DeltaThreshold = 1.0;
		static constexpr double CorssoverChance = 0.75;

		bool running = false;
		bool iterateGenome;

		void (*evaluate) (int, int, Genome*);
		void (*evaluateAll) (int, std::vector<GenomePtr>*, int*);

		std::vector<GenomePtr>* genomes;
		static const int StaleSpecies = 15;
		int DEFAULT_NEURON_COUNT = 5;

		void run();

		void newGeneration();
		void cullSpecies(bool cutToOne);
		void rankGlobally();
		void removeStallSpecies();
		void calculateAverageFitness(Species* species);
		void removeWeakSpecies();
		int totalAverageFitness();
		void updateTopFitness();

		void initPool();

		Genome* breedChild(Species* species);
		void addToSpecies(Genome* child);
		bool sameSpecies(Genome* genome1, Genome* genome2);
		double disjoint(Genome* genome1, Genome* genome2);
		double weights(Genome* genome1, Genome* genome2);

		void dumpToFile();

		inline Genome* toRawPtr(GenomePtr& ptr){
			return ((Genome*)ptr.get());
		}

		inline Species* toRawPtr(SpeciesPtr& ptr){
			return ((Species*)ptr.get());
		}

		friend Species::Species(std::ifstream &in, Pool* p);
};

