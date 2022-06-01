#ifndef NETWORK_H
#define NETWORK_H

#include <iostream> 
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <random>
#include <boost/random.hpp>

using namespace std; 

class Network{

public:

	void add_species(boost::mt19937 &rng); // Fügt eine neue Spezies zum Netz hinzu.
	void add_species(boost::mt19937 &rng, double time); // Fügt eine neue Spezies zum Netz hinzu und legt den Zeitpunkt ihrer Generierung in PlantArrivals bzw. AnimalArrivals ab.

	// void add_InitialPair(double time); // Generiert ein Startnetzwerk mit zufälligen Spezies.
	void add_InitialPair(boost::mt19937 &rng, double time); // Generiert ein zufälliges Startnetz. 
	void add_InitialPair(double at, double av, double time); // Generiert ein Startnetz mit vorgegebenem Bestäuber.

	void delete_plants(vector<int> v); // Entfernt die im Vektor gelisteten Pflanzen aus dem Netz.
	void delete_plants(vector<int> v, double time); // Entfernt die im Vektor gelisteten Pflanzen aus dem Netz und aktualisiert PlantArrivals.
	void delete_animals(vector<int> v); // Entfernt die im Vektor gelisteten Tiere aus dem Netz.
	void delete_animals(vector<int> v, double time); // Entfernt die im Vektor gelisteten Tiere aus dem Netz und aktualisiert AnimalArrivals.

	int get_noofplants(); // Gibt die Anzahl Pflanzen im Netz aus.
	int get_noofanimals(); // Gibt die Anzahl Tiere im Netz aus.
	int get_MaxPlants(); // Gibt die max. Anzahl Pflanzen im Netz aus.
	int get_MaxAnimals(); // Gibt die max. Anzahl Tiere im Netz aus.
	vector<double> get_PlantTraits(); // Gibt PlantTraits aus. 
	vector<double> get_AnimalTraits(); // Gibt AnimalTraits aus. 
	vector<double> get_PlantArrivals(); // Gibt PlantArrivals aus.
	vector<double> get_AnimalArrivals(); // Gibt AnimalArrivals aus. 
	vector<double> get_AnimalVariances(); // Gibt AnimalVariances aus. 
	vector<double> get_AnimalConnectivities(); // Gibt AnimalConnectivities aus.
	vector<vector<double>> get_AdjacencyMatrix(); // Gibt AdjacencyMatrix aus.
	/* Neu. */ 
	vector<bool> get_PlantSurvived(); // Gibt PlantSurvived aus.
	vector<bool> get_AnimalSurvived(); // Gibt AnimalSurvived aus.

	double get_AverageVariance(); // Gibt die durchschnittliche Varianz der Bestäuber aus. 
	double get_MaximumVariance(); // Gibt die größte Standardabweichung eines Bestäubers im Netz aus.
	double get_MinimumVariance(); // Gibt die kleinste Standardabweichung eines Bestäubers im Netz aus.
	double get_StrategyDiversity(); // Gibt die Differenz zwischen kleinster und größter vorhandener Standardabweichung im Netzwerk aus.
	double get_AverageInverseVariance(); // Gibt den Durchschnitt der Kehrwerte der Varianzen der Bestäuber aus.
	double get_AverageDistance(); // Gibt den durchschnittlichen Abstand einer Spezies zu einem ihrer Partner im Trait-Raum an.
	double get_AverageLinkStrength(); // Gibt die durchschnittliche Stärke realisierter Links aus.
	double get_VarianceOfLinkStrength(); // Gibt die Streuung der Stärken realisierter Links aus.
	double get_WidthOfCoveredTraitInterval_Plants(); // Gibt die Breite des abgedeckten Intervalls an Pflanzen-Traits aus.
	double get_WidthOfCoveredTraitInterval_Animals(); // Gibt die Breite des abgedeckten Intervalls an Tier-Traits aus.
	double get_Connectance(); // Gibt die Connectance des Netzwerks aus.

	double get_AveragePlantArrival(); // Gibt den Durchschnitt der Ankunftszeiten der Pflanzen im Netzwerk aus.
	double get_AverageAnimalArrival(); // Gibt den Durchschnitt der Ankunftszeiten der Bestäuber im Netzwerk aus.
	double get_AverageSpeciesArrival(); // Gibt den Durchschnitt der Ankunftszeiten aller Spezies im Netzwerk aus.

	double get_DistanceToNextPlant(int i); // Gibt den Abstand zur nächstgelegenen Pflanze im Trait-Raum für die i. Pflanze aus.
	int get_PlantConnectivity(int i); // Gibt die Konnektivität der i. Pflanze aus.

	void set_MaxPlants(int i); // Setzt die max. Anzahl Pflanzen im Netz auf i.
	void set_MaxAnimals(int i); // Setzt die max. Anzahl Tiere im Netz auf i.
	/* Neu. */
	void set_SurvivalOfMostRecentPlant(bool b);
	void set_SurvivalOfMostRecentAnimal(bool b);

	void print_adj(); // Gibt die Adjazenzmatrix aus.
	void get_information(); // Gibt eine Übersicht der Eigenschaften des Netzwerks aus. 

	Network(); // Konstruktor. 

private:

	double MinVariance = 0.0; // Minimale Varianz der Bestäuber.
	double MaxVariance = 0.275; // Maximale Varianz der Bestäuber. 
	double INITIAL_VIABILITY_THRESHOLD = 3.0; // 0.109582; // Linkstärke, die gegeben sein muss, damit ein initiales Paar überlebt und für die gewählten Startbedingungen immer in den FP > 0 läuft. Parameterabhängig!

	int noofplants, noofanimals, MaxPlants, MaxAnimals;
	vector<double> PlantTraits, AnimalTraits, PlantArrivals, AnimalArrivals, AnimalVariances, AnimalConnectivities; 
	/*Neu.*/ vector<bool> PlantSurvived, AnimalSurvived;
	vector<vector<double>> AdjacencyMatrix;

	void add_plant(); // Generiert eine neue Pflanze.
	void add_plant(double time); // Generiert eine neue Pflanze und legt den Zeitpunkt ihrer Generierung in PlantArrivals ab.
	void add_animal(boost::mt19937 &rng); // Generiert ein neues Tier.
	void add_animal(boost::mt19937 &rng, double time); // Generiert ein neues Tier und legt den Zeitpunkt seiner Generierung in AnimalArrivals ab.
	
	double get_linkstrength(double pt, double at, double av); // Berechnet die Linkstärke. 
	double get_AverageDistance_Plant(int i); // Gibt durchschnittlichen Abstand der i. Pflanze zu ihren Partnern an.
	double get_AverageDistance_Animal(int j); // Gibt den durchschnittlichen Abstand des j. Tiers zu seinen Partnern an.

	void fill_adj(); // Bestimmt die Adjazenzmatrix. 
	void update_animal_connectivities(); // Aktualisiert AnimalConnectivities.

	bool is_linked(double t); // Hilfsfunktion für add_plant().
	bool is_linked(double t, double v); // Hilfsfunktion für add_animal().

};

#endif