#include "Snake.h"
#include "Display.h"
#include <iostream>
#include "../network/Pool.h"
#include "../network/Genome.h"
//#include <fstream>

using namespace std;
std::ofstream stream;
Display* display;
void evaluate (int generation, int species, Genome* genome){
//	std::cout;
	std::cout << "Evaluating Generation " << generation << ", Genome " << genome->id << std::endl;
//	::stream << "Evaluating Generation " << geneartion << ", Genome " << genome->id << std::endl;
	Snake s(10, 10);
	double inputs[4];
	for(int i = 0; i < 100 && !s.isFinished(); i ++){
//		s.printBoard(::stream);
//		::stream << endl;
		if(s.lengthUpdated)
			i = 0;
		inputs[0] = s.fruitDistance();
		inputs[1] = s.bodyDistance();
		inputs[2] = s.fruitPythagoreanDistance();
		inputs[3] = s.getDir();
		double** res = genome->evaluate(inputs);
		int maxId = 0;
		double max = 0;
		for(int i = 0; i < 4; i ++)
			if(*res[i] > max){
				max = *res[i];
				maxId = i;
			}
		s.setDir(maxId);
		s.update();
		display->feedState(generation, genome->id, s.getBoard());
	}
	genome->setFitness(s.getLength());
	cout << "Evaluation Finished. Fitness: " << s.getLength() << endl;
//	::stream << "Evaluation Finished. Fitness: " << s.getLength() << std::endl;
//	::stream.flush();
}
int main(){
//	std::cout << "a";
//	::stream.open("output.txt");
	Pool p(4, evaluate);
	display = new Display(&p);
	p.startFromFile();
//	Snake snake(10, 10);
//	int dir = 0;
//	while(!snake.isFinished()){
//		snake.printBoard(cout);
//		cin >> dir;
//		snake.setDir(dir);
//		snake.update();
//		cout << snake.fruitDistance() << " " << snake.bodyDistance() << endl;
//	}
//	cout << snake.getLength()<<endl;
}
