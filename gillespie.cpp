#include "gillespie.h"
#include <iostream> 

double get_next_step(boost::mt19937 &rng, double MIG_RATE){

	std::uniform_real_distribution<> dis(0.0, 1.0); // U[0, 1).
	double rnd = dis(rng);

	return (-1/MIG_RATE)*log(1-rnd); // Argument aus U(0, 1]. 

}