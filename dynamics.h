#ifndef DYNAMICS_H
#define DYNAMICS_H

#include "network.h"
#include <boost/random.hpp>
#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>

typedef std::vector<double> state_type; // Zustandsvektoren des dynamischen Systems werden später vom Typ state_type sein.

const double r = 1.0; // max. Pro-Kopf-Wachstumsrate der Pflanzen.
const double lambda = 0.8; // max. Pro-Kopf-Wachstumsrate der Bestäuber.
const double w = 0.8; // intraspezifische Konkurrenz der Pflanzen.
const double u = 0.2; // interspezifische Konkurrenz der Pflanzen.
const double mu_A = 0.1; // Pro-Kopf-Mortalitätsrate der Bestäuber.
const double mu_P = 0.002; // Pro-Kopf-Mortalitätsrate der Pflanzen.

const double xi = 0.1; // Größe einer Pflanzennische.

const double EXTINCTION_THRESHOLD = 1.0e-4; // Aussterbeschwelle der Bestäuber. 
const double PLANT_EXTINCTION_THRESHOLD = 0.01; // Aussterbeschwelle der Pflanzen.

static int MaxPlants, MaxAnimals; // Maximale Anzahl an Pflanzen bzw. Bestäubern im Netz.

// FUNKTIONEN.

double PlantDynamics(int i, const state_type &x, Network network); // Gibt für ein gegebenes Netz die Dynamik für Pflanze i aus.
double AnimalDynamics(int j, const state_type &x, Network network); // Gibt für ein gegebenes Netz die Dynamik für Bestäuber j aus.

state_type MakeInitialCondition(int MaxPlants, int MaxAnimals); // Generiert die Anfangsbedingung für das Startnetzwerk mit 1 Pflanze und 1 Tier. 
void AddInitialDensity(Network network, state_type &x); // Nimmt ein Netzwerk und fügt für neue Spezies (Spezies mit x[i] = 0) eine kleine Populationsdichte hinzu.

bool DoExtinctions(Network &network, state_type &x); // Übernimmt Zustandsvektor am Ende der Integration und kontrolliert, ob eine/mehrere Spezies ausgestorben ist/sind. 
													 // Falls ja, werden Zustandsvektor und Netzwerk entsprechend angepasst. Ausgabe: true, falls es ein Aussterbeereignis gab. Andernfalls false. 
bool DoExtinctions(Network &network, state_type &x, double time); // Übernimmt Zustandsvektor am Ende der Integration und kontrolliert, ob eine/mehrere Spezies ausgestorben ist/sind. 
																  // Falls ja, werden Zustandsvektor und Netzwerk entsprechend angepasst. Dies inkludiert PlantArrivals und AnimalArrivals. Ausgabe: true, falls es ein Aussterbeereignis gab. Andernfalls false. 

double GetPlantGrowthRate(int i, const state_type &x, Network network); // Gibt die momentane Wachstumsrate der Pflanze i aus.
double GetAnimalGrowthRate(int j, const state_type &x, Network network); // Gibt die momentane Wachstumsrate des Bestäubers j aus. 

// Die Struktur NetDynamics hält die Dynamik des aktuellen Netzes.
// Später wird das dynamische System zum aktuellen Netz über NetDynamics(Netzwerk) referenziert.
struct NetDynamics{

	Network network;
	NetDynamics(Network N){network = N;}

	void operator()(state_type const& x, state_type& dxdt)
	{

		dxdt.assign(x.size(), 0.0);
		for(int i = 0; i < network.get_noofplants(); i++){

			dxdt[i] = PlantDynamics(i, x, network);

		};

		for(int j = network.get_MaxPlants(); j < network.get_MaxPlants() + network.get_noofanimals(); j++){

			dxdt[j] = AnimalDynamics(j - network.get_MaxPlants(), x, network); 

		};

		dxdt[network.get_MaxPlants() + network.get_MaxAnimals()] = 1; // Zeitkoordinate, welche die Zeit explizit mittrackt. 

	}

};

struct Noise{

	// Objekte, mit denen Noise arbeitet.
	boost::mt19937 &m_rng;
	boost::normal_distribution<> m_dist; 
	double m_amp; // Amplitude des Rauschens.
	Network network;

	Noise(boost::mt19937 &rng, double sigma, double noise_strength, Network N): m_rng(rng), m_dist(0.0, sigma), m_amp(noise_strength), network(N){ }

	void operator()(const state_type &x, state_type &dxdt)
	{

		dxdt.assign(x.size(), 0.0);
		for(int i = 0; i < network.get_noofplants(); i++){

			dxdt[i] = m_amp*m_dist(m_rng);

		};

		for(int j = network.get_MaxPlants(); j < network.get_MaxPlants() + network.get_noofanimals(); j++){

			dxdt[j] = m_amp*m_dist(m_rng);

		};

		dxdt[network.get_MaxPlants() + network.get_MaxAnimals()] = 0; // Zeitkoordinate, welche nicht rauscht. 

	}

};

#endif