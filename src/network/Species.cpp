/*
 * Species.cpp
 *
 *  Created on: Jun 28, 2017
 *      Author: Ethan
 */
#include <fstream>
#include <vector>
#include "Species.h"
#include "Pool.h"
int Species::ids = 0;

Species::Species(std::ifstream &in, Pool* p) {
	using namespace std;
	in >> id;
	in >> topFitness;
	in >> staleness;
	in >> averageFitness;
	int size = 0;
	in >> size;
	genomes = std::vector<Genome*>();
	genomes.reserve(size);
	for(int i = 0; i < size; i ++){
		Genome* g = new Genome(p, in);
		this->genomes.push_back(g);
		p->genomes->push_back(GenomePtr(g));
	}
}

void Species::print(std::ostream &strm){
	using namespace std;
	strm << "Species " << id << ": " << endl;
	strm << "	Top Fitness: " << topFitness << ", Staleness: " << staleness << ", Average Fitness: " << averageFitness << endl;
	for(auto &genome:genomes){
		genome->print(strm);
	}
	strm << endl;
	strm << endl;
}

void Species::dumpToFile(std::ofstream &stream) const {
	using namespace std;
	stream << id << endl << topFitness << endl << staleness << endl
			<< averageFitness << endl;
	stream << genomes.size() << endl;
	for (auto &genome : genomes) {
		genome->dumpToFile(stream);
	}
}

