#ifndef GILLESPIE_H
#define GILLESPIE_H

#include <cmath>
#include <random>
#include <boost/random.hpp>

using namespace std;

double get_next_step(boost::mt19937 &rng, double MIG_RATE); // Gibt Zeitspanne bis zum nächsten Ereignis aus. Benötigt eine definierten Wert von MIG_RATE.

#endif