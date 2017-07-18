/*
 * Pool.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: Ethan
 */

#include "Pool.h"
#include "Genome.h"
#include <unordered_map>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include "Species.h"
#include "Genome.h"
#include "Global.h"

Pool::Pool(int outputs, void (*evaluate)(int, int, Genome*)) :
		innovation(outputs), evaluate(evaluate), iterateGenome(true) {
	genomes = new std::vector<GenomePtr>();
	species = std::vector<SpeciesPtr>();
}

Pool::Pool(int outputs, void (*evaluateAll)(int, std::vector<GenomePtr>*, int*)) :
		innovation(outputs), evaluateAll(evaluateAll), iterateGenome(false) {
	genomes = new std::vector<GenomePtr>();
	species = std::vector<SpeciesPtr>();
}

bool Pool::start() {
	if (running) return false;
	running = true;
	initPool();
	run();
	return true;
}

bool Pool::startFromFile() {
	if (running) return false;
	running = true;
	loadFromFile();
	run();
	return true;
}
bool Pool::stop() {
	if (!running) return false;
	running = false;
	return true;
}

void Pool::run() {
	while (running) {
		if (iterateGenome) {
			int c = 0;
			for (auto &genome : *genomes) {
				evaluate(generation, c++, toRawPtr(genome));
				if(genome->getFitness() > maxFitness)
					maxFitness = genome->getFitness();
			}
		} else
			evaluateAll(generation, genomes, &maxFitness);
//		rankGlobally();
//		for (double i = 0; i <= 1; i += 0.1) {
//			stream << (genomes->at(Population * i))->getFitness() << ", ";
//		}
//		stream << std::endl;
//		stream.flush();
//		std::cout << species.size() << " " << Species::ids << std::endl;
//		for (int i = 0; i < Species::ids; i++) {
//			bool finished = false;
//			for (auto &species : this->species)
//				if (species->id == i) {
//					stream2 << species->genomes.size() << ", ";
//					finished = true;
//					break;
//				}
//			if(!finished)
//				stream2 << 0 << ", ";
//		}
//		stream2 << std::endl;
//		stream2.flush();
		newGeneration();
	}
}

void Pool::newGeneration() {
	if (generation == 0) updateTopFitness();
	cullSpecies(false);
	rankGlobally();
	removeStallSpecies();
	rankGlobally();
	for (auto &species : this->species)
		calculateAverageFitness(toRawPtr(species));
	removeWeakSpecies();
	double sum = totalAverageFitness();
	static std::vector<GenomePtr> children;
	for (auto &species : this->species) {
		double breed = (int) ((species->getAverageFitness() / sum) * Population)
				- 1;
		for (int i = 0; i < breed; i++)
			children.push_back(GenomePtr(breedChild(toRawPtr(species))));
	}
	cullSpecies(true);
	while (children.size() + species.size() < Population)
		children.push_back(
				GenomePtr(
						breedChild(
								toRawPtr(
										species[(int) (nextDouble()
												* species.size())]))));
	int oldStart = children.size();
	for (unsigned int i = 0; i < genomes->size(); i++)
		if (!genomes->at(i)->toDelete)
			children.push_back(std::move(genomes->at(i)));
	for (int i = 0; i < oldStart; i++)
		addToSpecies(children.at(i).get());
	genomes->clear();
	for (auto &child : children)
		genomes->push_back(std::move(child));
	children.clear();
	generation++;
	if (generation % 10 == 0) dumpToFile();
}

void Pool::dumpToFile() {
	using namespace std;
	static string prefix = "Generation_Backups/", suffix = ".dat";
	ofstream out;
	out.open(prefix + to_string(generation) + suffix);
	out << generation << endl << innovation << endl << maxFitness << endl;
	out << species.size() << endl;
	out << Species::ids << endl;
	for (auto &species : this->species) {
		species->dumpToFile(out);
	}
	out.close();
	out.open(prefix + "Generation" + suffix);
	out << generation;
	out.close();
}

void Pool::loadFromFile() {
	using namespace std;
	static string prefix = "Generation_Backups/", suffix = ".dat";
	ifstream in;
	in.open(prefix + "Generation" + suffix);
	in >> generation;
	in.close();
	in.open(prefix + to_string(generation) + suffix);
	in >> generation;
	in >> innovation;
	in >> maxFitness;
	int size = 0;
	in >> size;
	species = std::vector<SpeciesPtr>();
	species.reserve(size);
	in >> Species::ids;
	for (int i = 0; i < size; i++) {
		species.push_back(SpeciesPtr(new Species(in, this)));
	}
}

void Pool::initPool() {
	for (int i = 0; i < Population; i++) {
		Genome* g = new Genome(this, DEFAULT_NEURON_COUNT);
		addToSpecies(g);
		genomes->push_back(GenomePtr(g));
	}
}

void Pool::cullSpecies(bool cutToOne) {
	for (auto &species : this->species) {
		std::sort(species->genomes.begin(), species->genomes.end(),
				Genome::rawPointerOperator);

		unsigned int remaining =
				cutToOne ? 1 : (species->genomes.size() / 2.0 + 1.0);
		while (species->genomes.size() > remaining) {
			species->genomes.back()->toDelete = true;
			species->genomes.pop_back();
		}
	}
}
void Pool::rankGlobally() {
	std::sort(genomes->begin(), genomes->end(), Genome::pointerOperator);
	int i = 0;
	for (auto &genome : *genomes) {
//		std::cout << genome->getFitness() << std::endl;
		genome->setGlobalRank(i++);
	}
//	std::cout << "done" << std::endl;
}
void Pool::removeStallSpecies() {
	for (std::vector<SpeciesPtr>::iterator i = this->species.begin();
			i != this->species.end();) {
		Species* species = i.base()->operator ->();
		std::vector<Genome*>& genomes = species->genomes;
		std::sort(genomes.begin(), genomes.end(), Genome::rawPointerOperator);
		if (genomes.size() > 0
				&& genomes[0]->getFitness()
						> i.base()->operator ->()->getTopFitness()) {
			species->setStaleness(0);
			species->setTopFitness(genomes[0]->getFitness());
		} else
			species->incrementStaleness();
		if (species->getStaleness() < StaleSpecies
				|| species->getTopFitness() >= getMaxFitness())
			i++;
		else {
			for (auto &genome : species->genomes)
				genome->toDelete = true;
			this->species.erase(i);
		}
	}
}
void Pool::calculateAverageFitness(Species* species) {
	int total = 0;
	for (auto genome : species->genomes)
		total += genome->getFitness();
	species->setAverageFitness(total / species->genomes.size());
}
void Pool::removeWeakSpecies() {
	double sum = totalAverageFitness();
	for (std::vector<SpeciesPtr>::iterator i = this->species.begin();
			i != this->species.end();) {
		Species* species = i.base()->operator ->();
//		std::vector<Genome*>& genomes  = species->genomes;
		double breed = (int) ((species->getAverageFitness() / sum) * Population);
		if (breed >= 1)
			i++;
		else {
			for (auto &genome : species->genomes)
				genome->toDelete = true;
			this->species.erase(i);
		}
	}
}

int Pool::totalAverageFitness() {
	int total = 0;
	for (auto &species : this->species)
		total += species->getAverageFitness();
	return total;
}

void Pool::addToSpecies(Genome* child) {
	for (auto &_species : species) {
		if (sameSpecies(child, _species->genomes[0])) {
			_species->genomes.push_back(child);
			return;
		}
	}
	Species* childSpecies = new Species();
	childSpecies->genomes.push_back(child);
	species.push_back(SpeciesPtr(childSpecies));
}

Genome* Pool::breedChild(Species* species) {
	if (nextDouble() < CorssoverChance) {
		Genome* g1 = species->genomes.at(
				(int) (nextDouble() * species->genomes.size()));
		Genome* g2 = species->genomes.at(
				(int) (nextDouble() * species->genomes.size()));
		return new Genome(this, g1, g2, true);
	} else
		return new Genome(this,
				species->genomes.at(
						(int) (nextDouble() * species->genomes.size())), true);
}

void Pool::updateTopFitness() {
	for (auto &species : this->species) {
		int max = 0;
		for (auto g : species->genomes)
			max = std::max(max, g->getFitness());
		species->setTopFitness(max);
	}
}

bool Pool::sameSpecies(Genome* genome1, Genome* genome2) {
	double dd = DeltaDisjoint * disjoint(genome1, genome2);
	double dw = DeltaWeights * weights(genome1, genome2);
	return dd + dw < DeltaThreshold;
}

double Pool::disjoint(Genome* genome1, Genome* genome2) {
	int disjoint = 0;
	for (auto gene : genome1->getGenes())
		if (genome2->getGenes().find(gene.first) == genome2->getGenes().end())
			disjoint++;
	for (auto gene : genome2->getGenes())
		if (genome1->getGenes().find(gene.first) == genome1->getGenes().end())
			disjoint++;
	return disjoint
			/ max(genome1->getGenes().size(), genome2->getGenes().size());
}
double Pool::weights(Genome* genome1, Genome* genome2) {
	double sum = 0;
	int c = 0;
	for (auto gene : genome1->getGenes())
		if (genome2->getGenes().find(gene.first) != genome2->getGenes().end()) {
			sum += abs(
					gene.second->weight
							- genome2->getGenes().at(gene.first)->weight);
			c++;
		}
	return c ? sum / c : 0;
}

void Pool::print(std::ostream &strm) {
	using namespace std;
	strm << "Generation: " << generation << ", Innovation: " << innovation
			<< ", Max Fitness: " << maxFitness << endl;
	for (auto &species : this->species) {
		species->print(strm);
	}
	strm << "Genome Order: ";
	for (auto &genome : *genomes) {
		strm << genome->id << ", ";
	}
}

Pool::~Pool() {
}

