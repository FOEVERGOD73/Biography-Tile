#include "Global.h"
#include <random>

double nextDouble() {
	static std::random_device rd; //Will be used to obtain a seed for the random number engine
	static std::mt19937 gen(0); //static_cast<long int>(time(NULL)));//gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	static std::uniform_real_distribution<> dis(0, 1);
	double d = dis(gen);
//	std::cout << d << std::endl;
	return d;
}

double max(const double &a, const double &b) {
	return a > b ? a : b;
}
