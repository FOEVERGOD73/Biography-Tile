/*
 * GUI.h
 *
 *  Created on: Jun 30, 2017
 *      Author: Ethan
 */

#pragma once
#include "Genome.h"
#include "Species.h"

class GUI {
		typedef std::unique_ptr<Genome> GenomePtr;
		typedef std::unique_ptr<Species> SpeciesPtr;
	public:
		GUI();
		void feedGeneration(std::vector<SpeciesPtr>* species,
				std::vector<GenomePtr>* genomes);
		virtual ~GUI();
	private:
		std::vector<int> percentiles;
		std::vector<int> sizes;
		std::vector<int> speciesSize;
};

