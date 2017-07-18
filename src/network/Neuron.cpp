/*
 * Neuron.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: Ethan
 */

#include "Neuron.h"
#include "Genome.h"
#include "Gene.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cmath>

Neuron::Neuron(int _id, bool isOutput, int outputId) :
		id(_id), isOutput(isOutput), outputId(outputId) {
}

void Neuron::cloneFrom(Genome* genome, int maxNeuron, Neuron* neuron) {
	marked = neuron->marked;
	value = neuron->value;
	for (GenePtr &g : neuron->genes) {
		Gene* ng = new Gene();
		ng->into = (
				g->into->isOutput ?
						genome->getOutput(g->into->outputId) :
						toRawPtr(genome->neurons[g->into->id]));
		ng->out = this;
		ng->weight = g->weight;
		ng->innovation = g->innovation;
		ng->enabled = g->enabled;
		ng->back = g->back;
		genes.push_back(GenePtr(ng));
		(genome->genes).insert(GenePair(ng->innovation, ng));
	}
}

void Neuron::evaluate() {
	if (genes.size()) {
		double sum = 0;
		for (auto &gene : genes) {
			sum += gene->weight * gene->into->value;
		}
		value = sigmoid(sum);
	}
}

void Neuron::print(std::ostream &out) {
	out << "Neuron " << id << ", Value: " << value << ": ";
	for (auto &gene : genes) {
		if (!gene->back) out << gene->into->id << ", ";
	}
	out << '\n';
}

void Neuron::load(Genome* genome, std::ifstream &in) {
	using namespace std;
	in >> id;
	in >> outputId;
	in >> marked;
	in >> value;
	in >> isOutput;
	int size = 0;
	in >> size;
	genes = std::vector<GenePtr>();
	genes.reserve(size);
	for (int i = 0; i < size; i ++){
		Gene* g = new Gene();
		int id = 0;
		in >> id;
		g->into = genome->neurons[id].get();
		in >> id;
		g->out = this;
		in >> g->weight;
		in >> g->innovation;
		in >> g->enabled;
		in>> g->back;
		genome->genes.insert(GenePair(g->innovation, g));
		genes.push_back(GenePtr(g));
	}
}

void Neuron::dumpToFile(std::ofstream &stream) const {
	using namespace std;
	stream << id << endl << outputId << endl << marked << endl << value << endl
			<< isOutput << endl;
	stream << genes.size() << endl;
	for (auto &gene : genes) {
		stream << gene->into->id << endl;
		stream << gene->out->id << endl;
		stream << gene->weight << endl;
		stream << gene->innovation << endl;
		stream << gene->enabled << endl;
		stream << gene->back << endl;
	}
}

Neuron::~Neuron() {
//	std::cout << "Neuron Deleted" << std::endl;
//	delete &genes;
}

