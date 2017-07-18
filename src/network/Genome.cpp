/*
 * Genome.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: Ethan
 */

#include "Genome.h"
#include "Neuron.h"
#include "Gene.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <ctime>
#include "Pool.h"
#include "Global.h"

int Genome::ids = 0;

Genome::Genome(Pool* p, Genome* seed, bool mutate) :
		maxNeuron(seed->maxNeuron), pool(p), id(ids++) {
//	genes = new std::unordered_map<int, Gene*>();
	cloneRatesFrom(seed);
	if (mutate) {
		pointMutate();
		initializeNeurons();
		this->mutate();
	} else
		initializeNeurons();
	cloneFrom(seed);
}

Genome::Genome(Pool* p, Genome* g1, Genome* g2, bool mutate) :
		maxNeuron(max(g1->maxNeuron, g2->maxNeuron)), pool(p), id(ids++) {
	if (g2->getFitness() > g1->getFitness()) {
		Genome* t = g1;
		g1 = g2;
		g2 = t;
	}
//	pointMutate();
	initializeNeurons();
	cloneRatesFrom(g1);
	for (auto &gene1 : g1->getGenes()) {
		if (g2->getGenes().find(gene1.first) != g2->getGenes().end()
				&& nextDouble() < 0.5
				&& g2->getGenes().at(gene1.first)->enabled) {
			Gene *g = g2->getGenes().at(gene1.first), *ng = new Gene();
			ng->into = (
					g->into->isOutput ?
							getOutput(g->into->outputId) :
							toRawPtr(neurons[g->into->id]));
			ng->out = (
					g->out->isOutput ?
							getOutput(g->out->outputId) :
							toRawPtr(neurons[g->out->id]));
			ng->weight = g->weight;
			ng->innovation = g->innovation;
			ng->enabled = g->enabled;
			ng->back = g->back;
			addGene(ng);
		}else{
			Gene *g =gene1.second, *ng = new Gene();
			ng->into = (
					g->into->isOutput ?
							getOutput(g->into->outputId) :
							toRawPtr(neurons[g->into->id]));
			ng->out = (
					g->out->isOutput ?
							getOutput(g->out->outputId) :
							toRawPtr(neurons[g->out->id]));
			ng->weight = g->weight;
			ng->innovation = g->innovation;
			ng->enabled = g->enabled;
			ng->back = g->back;
			addGene(ng);
		}
	}
	this->mutate();
}

Genome::Genome(Pool* p, int _maxNeuron) :
		maxNeuron(_maxNeuron + INPUT_NEURONS + OUTPUT_NEURONS), pool(p), id(
				ids++) {
//	genes = new std::unordered_map<int, Gene*>();
	pointMutate();
	initializeNeurons();
	createGenome();
	mutate();
}

bool Genome::isHidden(Neuron* x) {
	return INPUT_NEURONS <= (x->id) && (x->id) < (maxNeuron - OUTPUT_NEURONS);
}

bool Genome::isOutput(Neuron* x) {
	return (x->id) >= (maxNeuron - OUTPUT_NEURONS);
}

bool Genome::isHidden(Neuron &x) {
	return INPUT_NEURONS <= (x.id) && (x.id) < (maxNeuron - OUTPUT_NEURONS);
}

bool Genome::isOutput(Neuron &x) {
	return (x.id) >= (maxNeuron - OUTPUT_NEURONS);
}

bool Genome::isHidden(int x) {
	return INPUT_NEURONS <= (x) && (x) < (maxNeuron - OUTPUT_NEURONS);
}

bool Genome::isOutput(int x) {
	return (x) >= (maxNeuron - OUTPUT_NEURONS);
}

void Genome::addGene(Gene* gene) {
	Neuron* x = gene->into, *y = gene->out;
//	std::cout << "Adding Gene From " << x->id << " To " << y->id << std::endl;
	if ((y->id) < INPUT_NEURONS) return;
	if (x == y) {
//		std::cout << "Back Edge" << std::endl;
		gene->back = true;
	} else if (isHidden(*x) && isHidden(*y)) {
		if (!find(*y, *x)) {
			if (y->id < x->id) {
				int yid = y->id, xid = x->id;
				int length = xid - yid + 1;
				int count = length - mark(*y, yid, xid), c = 0;
				for (int i = 0; i < length; i++) {
					Neuron* n = toRawPtr(neurons[yid + i]);
					toRawPtr(neurons[yid + i])->id = yid
							+ (n->marked ? count++ : c++);
					n->marked = false;
				}
				sort(yid, length);
			}
		} else {
//			std::cout << "Back Edge" << std::endl;
			gene->back = true;
		}
	} else if (isOutput(*x)) {
//		std::cout << "Back Edge" << std::endl;
		gene->back = true;
	}
	(y->genes).push_back(GenePtr(gene));
	genes.insert(GenePair(gene->innovation, gene));
}

int Genome::mark(Neuron &y, int id, int id2) {
	y.marked = true;
	int count = 1;
	for (int i = id + 1; i <= id2; i++) {
		Neuron* n = toRawPtr(neurons[i]);
		if (isHidden(*n)) {
			for (GenePtr &gene : n->genes) {
				if (gene->into->marked) {
					n->marked = true;
					count++;
					break;
				}
			}
		}
	}
	return count;
//	if (!y->marked && isHidden(y) && (id <= y->id && y->id <= id2)) {
//		int count = 1;
//		y->marked = true;
//		for (Gene gene : y->genes) {
//			count += mark(gene.out, id, id2);
//		}
//		return count;
//	}
//	return 0;
}

bool Genome::find(const Neuron &target, const Neuron &source) const {
//	std::cout << "Looking for " << target->id << " From " << source -> id << std::endl;
	if (source.id == target.id) return true;
	for (const GenePtr &gene : source.genes) {
		if (!gene->back && find(target, *(gene->into))) return true;
	}
	return false;
}

void Genome::sort(int minId, int length) {
//	std::cout << "sorting..." << std::endl;
//	for (int i = 0; i < length; i++) {
//		std::cout << neurons[minId+i]->id << std::endl;
//	}
	NeuronPtr temp;
	for (int i = 0; i < length; i++) {
		while (toRawPtr(neurons[minId + i])->id != minId + i) {
			temp = std::move(neurons[minId + i]);
			neurons[minId + i] = std::move(neurons[toRawPtr(temp)->id]);
			neurons[toRawPtr(temp)->id] = std::move(temp);
		}
	}
}

void Genome::mutate() {
	for (int i = 0; i < RATES; i++) {
		if (nextDouble() < 0.5)
			rates[i] *= 0.95;
		else
			rates[i] *= 1.05263;
	}
	double p;
	for (int i = 0; i < INPUT_NEURONS; i++)
		INPUTS[i] = &(toRawPtr(neurons[i])->value);
	*INPUTS[INPUT_NEURONS - 1] = 1.0;
	for (int i = 0; i < OUTPUT_NEURONS; i++)
		OUTPUTS[i] =
				&(toRawPtr(neurons[maxNeuron - OUTPUT_NEURONS + i])->value);

//	std::cout<<"Pre-Printing: ";
//	print(std::cout);

	if (nextDouble() < rates[CONNECTIONS]) pointMutate();

	for (p = rates[LINK]; p > 0; p--)
		if (nextDouble() < p) linkMutate(false);

	for (p = rates[BIAS]; p > 0; p--)
		if (nextDouble() < p) linkMutate(true);

	for (p = rates[ENABLE]; p > 0; p--)
		if (nextDouble() < p) enableDisableMutate(true);

	for (p = rates[DISABLE]; p > 0; p--)
		if (nextDouble() < p) enableDisableMutate(false);
}

void Genome::nodeMuate() {
	double p;
	for (p = rates[NODE]; p > 0; p--)
		if (nextDouble() < p) maxNeuron++;
}
void Genome::pointMutate() {
	for (auto &gene : genes) {
		if (nextDouble() < rates[PERTURBCHANCE])
			gene.second->weight += nextDouble() * rates[STEP] * 2 - rates[STEP];
		else
			gene.second->weight = nextDouble() * 4 - 2;
	}
}

void Genome::linkMutate(bool forceBias) {
	NeuronPtr& neuron1 = randomNeuron(false);
	NeuronPtr& neuron2 = randomNeuron(true);

	Gene* newLink = new Gene();
	newLink->into = (forceBias ? neurons[INPUT_NEURONS - 1] : neuron1).get();
	newLink->out = neuron2.get();
	newLink->innovation = pool->newInnovation();
	newLink->weight = nextDouble() * 4 - 2;

	addGene(newLink);
}

void Genome::enableDisableMutate(bool enable) {
	int r = nextDouble() * genes.size(), c = r;
//	std::cout << genes->size()<<std::endl;

	for (auto i = genes.begin(); r >= 0; ++i) {
		if (i == genes.end()) {
			i = genes.begin();
			if (r == c) return;
		}
		Gene* gene = i->second;
		if (gene->enabled == !enable) {
			r--;
			if (r == 0) {
				gene->enabled = enable;
				return;
			}
		}
	}
}

void Genome::initializeNeurons() {
	neurons = new NeuronPtr[maxNeuron];
	for (int i = 0; i < maxNeuron; i++)
		neurons[i] = std::make_unique<Neuron>(i, isOutput(i),
				isOutput(i) ? i - (maxNeuron - OUTPUT_NEURONS) : -1);
}

Neuron* Genome::getOutput(int id) {
	return neurons[maxNeuron - OUTPUT_NEURONS + id].get();
}

Genome::NeuronPtr& Genome::randomNeuron(bool nonInput) {
	if (nonInput)
		return neurons[(int) ((nextDouble() * (maxNeuron - INPUT_NEURONS))
				+ INPUT_NEURONS)];
	else
		return neurons[(int) (nextDouble() * maxNeuron)];
}

double** Genome::evaluate(double* inputs) {
//	if ((sizeof(inputs) / sizeof(double*)) == INPUT_NEURONS - 1) {
	for (int i = 0; i < INPUT_NEURONS - 1; i++) {
		*INPUTS[i] = inputs[i];
	}
//	}
	for (int i = INPUT_NEURONS; i < maxNeuron; i++) {
		toRawPtr(neurons[i])->evaluate();
	}
	return OUTPUTS;
}

//int Genome::newInnovation() {
//	static int innov = 0;
//	return innov++;
//}

void Genome::cloneRatesFrom(Genome* seed) {
	for (int i = 0; i < RATES; i++) {
		rates[i] = seed->rates[i];
	}
}

void Genome::cloneFrom(Genome* seed) {
//	maxNeuron = seed->maxNeuron;
	initializeNeurons();
	int delta = maxNeuron - seed->maxNeuron;
	for (int i = 0; i < seed->maxNeuron; i++) {
		toRawPtr(
				neurons[(i >= seed->maxNeuron - OUTPUT_NEURONS) ? i + delta : i])->cloneFrom(
				this, seed->maxNeuron, toRawPtr(seed->neurons[i]));
	}
}

void Genome::createGenome() {
	for (int i = 0; i < DEFAULT_GENES_PER_NEURON * maxNeuron; i++)
		linkMutate(false);
	pointMutate();
}

void Genome::print(std::ostream &out) {
	out << "Genome Neurons: " << std::endl;
	for (int i = 0; i < maxNeuron; i++) {
		((Neuron*) neurons[i].get())->print(out);
	}
}

Genome::Genome(Pool* p, std::ifstream &in) : pool(p){
	using namespace std;
	in >> id;
	in >> toDelete;
	in >> fitness;
	in >> globalRank;
	in >> maxNeuron;
	neurons = new NeuronPtr[maxNeuron];
	for(int i = 0; i < RATES; i ++)
		in >> rates[i];
	for(int i = 0; i < maxNeuron; i ++){
		neurons[i] = NeuronPtr(new Neuron());
	}
	for(int i = 0; i < maxNeuron; i ++){
		neurons[i]->load(this, in);
	}
	for (int i = 0; i < INPUT_NEURONS; i++)
		INPUTS[i] = &(toRawPtr(neurons[i])->value);
	*INPUTS[INPUT_NEURONS - 1] = 1.0;
	for (int i = 0; i < OUTPUT_NEURONS; i++)
		OUTPUTS[i] =
				&(toRawPtr(neurons[maxNeuron - OUTPUT_NEURONS + i])->value);
}

void Genome::dumpToFile(std::ofstream &stream) const {
	using namespace std;
	stream << id << endl << toDelete << endl << fitness << endl << globalRank
			<< endl << maxNeuron << endl;
	for (int i = 0; i < RATES; i++)
		stream << rates[i] << endl;
	for (int i = 0; i < maxNeuron; i++) {
		neurons[i]->dumpToFile(stream);
	}
}

Genome::~Genome() {
//	std::cout << "Genome Deleted" << std::endl;
//	for (auto gene : *genes) {
//		std::cout << "GENE " << gene.second.use_count() << std::endl;
//	}
	for (int i = 0; i < maxNeuron; i++) {
		neurons[i].reset();
//		std::cout << "Neuron " << toRawPtr(neurons[i])->id << std::endl;
	}
//	ptr.reset();
}

