/*
 * Species.h
 *
 *  Created on: Jun 15, 2017
 *      Author: Ethan
 */

#pragma once
#include <vector>
#include "Genome.h"
class Species {
		typedef std::unique_ptr<Genome> GenomePtr;
		typedef std::shared_ptr<Species> SpeciesPtr;
	public:
		int id;
		std::vector<Genome*> genomes;
		Species(): id(ids++){
			genomes = std::vector<Genome*>();
		}

		Species(std::ifstream &in, Pool* p);
		int getAverageFitness() const {
			return averageFitness;
		}
		
		void setAverageFitness(int averageFitness) {
			this->averageFitness = averageFitness;
		}
		
		int getStaleness() const {
			return staleness;
		}
		
		void setStaleness(int staleness) {
			this->staleness = staleness;
		}
		
		void incrementStaleness() {
			staleness++;
		}

		int getTopFitness() const {
			return topFitness;
		}
		
		void setTopFitness(int topFitness) {
			this->topFitness = topFitness;
		}

		void dumpToFile(std::ofstream &stream) const;
		void print(std::ostream &strm);

		static int ids;
	private:
		int topFitness = 0;
		int staleness = 0;
		int averageFitness = 0;
};
