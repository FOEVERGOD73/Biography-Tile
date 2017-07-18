/*
 * GUI.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: Ethan
 */

#include "GUI.h"
#include "Species.h"
#include "Genome.h"
#include <vector>
#include <memory>

GUI::GUI() {
	
}

void GUI::feedGeneration(std::vector<SpeciesPtr>* species,
		std::vector<GenomePtr>* genomes){
	for(double i = 0; i < 1.0; i += 0.1){
		percentiles.push_back(genomes->at(i * genomes->size())->getFitness());
	}
	percentiles.push_back(genomes->front()->getFitness());
	speciesSize.push_back(species->back()->id);
	for(int i = 0, j = 0; i < speciesSize.back(); i ++){
		if(species->at(j)->id == i){
			sizes.push_back(species->at(j)->genomes.size());
			j++;
		}else
			sizes.push_back(0);
	}
}

GUI::~GUI() {
}

