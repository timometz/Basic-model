#include "network.h"

// Konstruktor für vordefinierte Anfangsnetze. 
/*Network::Network(){

	noofplants = 9;
	noofanimals = 9;

	MaxPlants = 30;
	MaxAnimals = 30;

	PlantTraits = {0, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1.0};
	AnimalTraits = {0.001, 0.201, 0.301, 0.401, 0.501, 0.601, 0.701, 0.801, 0.999};

	PlantArrivals = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	AnimalArrivals = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	AnimalVariances = {0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002};
	AnimalConnectivities = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	AdjacencyMatrix = {};

	PlantSurvived = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	AnimalSurvived = {1, 1, 1, 1, 1, 1, 1, 1, 1};

}*/

// Konstruktor für randomisierte Anfangsnetze. 
Network::Network(){

	noofplants = 0;
	noofanimals = 0;

	MaxPlants = 1;
	MaxAnimals = 1;

	PlantTraits = {};
	AnimalTraits = {};

	PlantArrivals = {};
	AnimalArrivals = {};

	AnimalVariances = {};
	AnimalConnectivities = {};
	AdjacencyMatrix = {};

	PlantSurvived = {};
	AnimalSurvived = {};

}

// Setter und Getter.

int Network::get_noofplants(){

	return noofplants;

}

int Network::get_noofanimals(){

	return noofanimals;
}

int Network::get_MaxPlants(){

	return MaxPlants;

}

int Network::get_MaxAnimals(){

	return MaxAnimals;

}

vector<double> Network::get_PlantTraits(){

	return PlantTraits;

}

vector<double> Network::get_AnimalTraits(){

	return AnimalTraits;
	
}

vector<double> Network::get_PlantArrivals(){

	return PlantArrivals;

}

vector<double> Network::get_AnimalArrivals(){

	return AnimalArrivals;

}

vector<double> Network::get_AnimalVariances(){

	return AnimalVariances;

}

vector<double> Network::get_AnimalConnectivities(){

	return AnimalConnectivities;

}

vector<vector<double>> Network::get_AdjacencyMatrix(){

	return AdjacencyMatrix;

}

vector<bool> Network::get_PlantSurvived(){

	return PlantSurvived;

}

vector<bool> Network::get_AnimalSurvived(){

	return AnimalSurvived;

}

double Network::get_AverageVariance(){

	double avg = 0;
	for(int j = 0; j < noofanimals; j++){

		avg = avg + AnimalVariances[j];

	};
	avg = avg/(double)noofanimals;

	return avg;

}

double Network::get_MaximumVariance(){

	double smax = 0;
	for(int j = 0; j < noofanimals; j++){

		if(AnimalVariances[j] > smax){smax = AnimalVariances[j];}

	};

	return smax;

}

double Network::get_MinimumVariance(){

	double smin = MaxVariance + 0.01;
	for(int j = 0; j < noofanimals; j++){

		if(AnimalVariances[j] < smin){smin = AnimalVariances[j];}

	};

	return smin;

}

double Network::get_StrategyDiversity(){

	double div = get_MaximumVariance() - get_MinimumVariance();
	return div;
	
}

double Network::get_AverageInverseVariance(){

	double avg = 0;
	for(int j = 0; j < noofanimals; j++){

		avg = avg + 1/AnimalVariances[j];

	};
	avg = avg/(double)noofanimals;

	return avg;

}

double Network::get_AverageDistance(){

	double avg = 0;

	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){

				avg = avg + abs(AnimalTraits[j] - PlantTraits[i])/((double)AnimalConnectivities[j]*(double)noofanimals);

			}

		};

	};

	return avg;

}

double Network::get_AverageDistance_Plant(int i){

	int nooflinks = 0;
	double avg = 0;

	for(int j = 0; j < noofanimals; j++){

		if(get_linkstrength(PlantTraits[i], AnimalTraits[j], AnimalVariances[j]) > 0){

			nooflinks++;
			avg = avg + abs(PlantTraits[i] - AnimalTraits[j]);

		}

	};

	avg = avg/(double)nooflinks;

	return avg;

}

double Network::get_AverageDistance_Animal(int j){

	int nooflinks = 0;
	double avg = 0;

	for(int i = 0; i < noofplants; i++){

		if(get_linkstrength(PlantTraits[i], AnimalTraits[j], AnimalVariances[j]) > 0){

			nooflinks++;
			avg = avg + abs(PlantTraits[i] - AnimalTraits[j]);

		}

	};

	avg = avg/(double)nooflinks;

	return avg;

}

double Network::get_AverageLinkStrength(){

	int nooflinks = 0;
	double avg = 0;

	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){

				nooflinks++;
				avg = avg + AdjacencyMatrix[j][i];

			}

		};

	};

	avg = avg/(double)nooflinks;

	return avg;

}

double Network::get_VarianceOfLinkStrength(){

	int nooflinks = 0;
	double mean = 0;

	// Bestimmung der durchschnittlichen Linkstärke.
	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){

				nooflinks++;
				mean = mean + AdjacencyMatrix[j][i];

			}

		};

	};

	mean = mean/(double)nooflinks;

	double deviation = 0;

	// Bestimmung der mittleren quadratischen Abweichung vom Mittelwert.
	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){

				deviation = deviation + pow(AdjacencyMatrix[j][i] - mean, 2)/(double)nooflinks;

			}

		};

	};

	return sqrt(deviation);

}

double Network::get_WidthOfCoveredTraitInterval_Plants(){

	double min = *min_element(PlantTraits.begin(), PlantTraits.end());
	double max = *max_element(PlantTraits.begin(), PlantTraits.end());

	return max - min;

}

double Network::get_WidthOfCoveredTraitInterval_Animals(){

	double min = *min_element(AnimalTraits.begin(), AnimalTraits.end());
	double max = *max_element(AnimalTraits.begin(), AnimalTraits.end());

	return max - min;

}

double Network::get_Connectance(){

	int nooflinks = 0;
	
	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){nooflinks++;}

		};

	};

	return (double)nooflinks/(noofanimals*noofplants);

}


double Network::get_AveragePlantArrival(){

	double sum = 0;

	for(int i = 0; i < noofplants; i++){

		sum = sum + PlantArrivals[i];

	};

	return sum/(double)noofplants;

}

double Network::get_AverageAnimalArrival(){

	double sum = 0;

	for(int j = 0; j < noofanimals; j++){

		sum = sum + AnimalArrivals[j];

	};

	return sum/(double)noofanimals;

}

double Network::get_AverageSpeciesArrival(){

	double sum = 0;

	for(int i = 0; i < noofplants; i++){

		sum = sum + PlantArrivals[i];

	};

	for(int j = 0; j < noofanimals; j++){

		sum = sum + AnimalArrivals[j];

	};

	return sum/(double)(noofplants + noofanimals);

}

double Network::get_DistanceToNextPlant(int i){

	if(i >= noofplants){

		cout << "get_DistanceToNextPlant: Pflanze existiert nicht! Ausgabewert 0." << endl;
		return 0;

	}

	else{

		double trait = PlantTraits[i];
		double min_dist = 1;

		for(int j = 0; j < noofplants; j++){

			if(abs(trait - PlantTraits[j]) < min_dist && trait != PlantTraits[j]){

				min_dist = abs(trait - PlantTraits[j]);

			}

		};

		return min_dist;

	}

}

int Network::get_PlantConnectivity(int i){

	if(i >= noofplants){

		cout << "get_PlantConnectivity: Pflanze existiert nicht! Ausgabewert -1." << endl;
		return -1;

	}

	else{

		int connec = 0;
		for(int j = 0; j < noofanimals; j++){

			if(AdjacencyMatrix[j][i] > 0){connec++;}

		};

		return connec;

	}

}

void Network::set_MaxPlants(int i){

	MaxPlants = i;

}

void Network::set_MaxAnimals(int i){

	MaxAnimals = i;

}

void Network::set_SurvivalOfMostRecentPlant(bool b){

	PlantSurvived.pop_back();
	PlantSurvived.push_back(b);

}

void Network::set_SurvivalOfMostRecentAnimal(bool b){

	AnimalSurvived.pop_back();
	AnimalSurvived.push_back(b);
	
}

// Hinzufügen und Entfernen von Spezies.

void Network::add_plant(){

	int ctr = -1;
	while(ctr < 500 && noofanimals > 0){

		double t = (double)rand()/(double)RAND_MAX;

		if(is_linked(t)){

			PlantTraits.push_back(t);
			PlantSurvived.push_back(0);

			// Aktualisiere das Netz. -> AnimalConnectivities, noofplants, AdjacencyMatrix.
			noofplants++;
			fill_adj();
			update_animal_connectivities();
		}

		ctr++;
		if(is_linked(t)){break;}

		if(ctr == 500){

			cout << "add_plant: Dem Netz konnte keine Pflanze hinzugefügt werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

	if(noofanimals == 0){

		cout << "add_plant: Keine Bestäuber zum Verbinden da." << endl;

	}

}

void Network::add_plant(double time){

	int ctr = -1;
	while(ctr < 500 && noofanimals > 0){

		double t = (double)rand()/(double)RAND_MAX;

		if(is_linked(t)){

			PlantTraits.push_back(t);
			PlantArrivals.push_back(time);
			PlantSurvived.push_back(0);


			// Aktualisiere das Netz. -> AnimalConnectivities, noofplants, AdjacencyMatrix.
			noofplants++;
			fill_adj();
			update_animal_connectivities();
		}

		ctr++;
		if(is_linked(t)){break;}

		if(ctr == 500){

			cout << "add_plant: Dem Netz konnte keine Pflanze hinzugefügt werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

	if(noofanimals == 0){

		cout << "add_plant: Keine Bestäuber zum Verbinden da." << endl;
		
	}

}

void Network::add_animal(boost::mt19937 &rng){

	std::uniform_real_distribution<> dis(0.0, 1.0); // U[0, 1).

	int ctr = -1;
	while(ctr < 500 & noofplants > 0){

		double t = (double)rand()/(double)RAND_MAX;
		double v = MinVariance + (MaxVariance - MinVariance)*(1 - dis(rng)); // So sind Werte aus (MinVariance, MaxVariance] erlaubt.
		// double v = MinVariance + (MaxVariance - MinVariance)*((double)rand()/(double)RAND_MAX);

		if(is_linked(t,v)){

			AnimalTraits.push_back(t);
			AnimalVariances.push_back(v);
			AnimalSurvived.push_back(0);

			// Aktualisiere das Netz. -> AnimalConnectivities, noofanimals, AdjacencyMatrix.
			noofanimals++;
			fill_adj();
			update_animal_connectivities();
		}

		ctr++;
		if(is_linked(t,v)){break;}

		if(ctr == 500){

			cout << "add_animal: Dem Netz konnte kein Tier hinzugefügt werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

	if(noofplants == 0){

		cout << "add_animal: Es sind keine Pflanzen zum Verbinden da." << endl;

	}

}

void Network::add_animal(boost::mt19937 &rng, double time){

	std::uniform_real_distribution<> dis(0.0, 1.0); // U[0, 1).

	int ctr = -1;
	while(ctr < 500 & noofplants > 0){

		double t = (double)rand()/(double)RAND_MAX;
		double v = MinVariance + (MaxVariance - MinVariance)*(1 - dis(rng)); // So sind Werte aus (MinVariance, MaxVariance] erlaubt.
		// double v = MinVariance + (MaxVariance - MinVariance)*((double)rand()/(double)RAND_MAX);

		if(is_linked(t,v)){

			AnimalTraits.push_back(t);
			AnimalVariances.push_back(v);
			AnimalArrivals.push_back(time);
			AnimalSurvived.push_back(0);

			// Aktualisiere das Netz. -> AnimalConnectivities, noofanimals, AdjacencyMatrix.
			noofanimals++;
			fill_adj();
			update_animal_connectivities();
		}

		ctr++;
		if(is_linked(t,v)){break;}

		if(ctr == 500){

			cout << "add_animal: Dem Netz konnte kein Tier hinzugefügt werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

	if(noofplants == 0){

		cout << "add_animal: Es sind keine Pflanzen zum Verbinden da." << endl;

	}

}

void Network::add_species(boost::mt19937 &rng){

	if(noofplants < MaxPlants && noofanimals < MaxAnimals){

		double p = (double)rand()/(double)RAND_MAX;
		if(p < 0.5){add_plant();}
		else{add_animal(rng);}

	}
	else if(noofplants < MaxPlants && noofanimals == MaxAnimals){

		add_plant();

	}
	else if(noofplants == MaxPlants && noofanimals < MaxAnimals){

		add_animal(rng); // Richtig so?

	}
	else{

		cout << "add_species: Dem Netz konnte keine Spezies hinzugefügt werden (maximale Kapazität erreicht)!" << endl;

	}

}

void Network::add_species(boost::mt19937 &rng, double time){

	if(noofplants < MaxPlants && noofanimals < MaxAnimals){

		double p = (double)rand()/(double)RAND_MAX;
		if(p < 0.5){add_plant(time);}
		else{add_animal(rng, time);}

	}
	else if(noofplants < MaxPlants && noofanimals == MaxAnimals){

		add_plant(time);

	}
	else if(noofplants == MaxPlants && noofanimals < MaxAnimals){

		add_animal(rng, time); // Richtig so?

	}
	else{

		cout << "add_species: Dem Netz konnte keine Spezies hinzugefügt werden (maximale Kapazität erreicht)!" << endl;

	}

}

void Network::add_InitialPair(boost::mt19937 &rng, double time){

	double pt, at, av;
	std::uniform_real_distribution<> dis(0.0, 1.0); // U[0, 1).

	pt = (double)rand()/(double)RAND_MAX;
	int ctr = -1;
	while(ctr < 500){

		at = (double)rand()/(double)RAND_MAX;
		av = MinVariance + (MaxVariance - MinVariance)*(1 - dis(rng)); // So sind Werte aus (MinVariance, MaxVariance] erlaubt.
		// av = MinVariance + (MaxVariance - MinVariance)*((double)rand()/(double)RAND_MAX);

		if(get_linkstrength(pt, at, av) > INITIAL_VIABILITY_THRESHOLD){

			PlantTraits.push_back(pt);
			PlantArrivals.push_back(time);
			noofplants++;

			AnimalTraits.push_back(at);
			AnimalVariances.push_back(av);
			AnimalArrivals.push_back(time);
			noofanimals++;

			fill_adj();
			update_animal_connectivities();

			// Neu. 
			PlantSurvived.push_back(0);
			AnimalSurvived.push_back(0);

		}

		ctr++;
		if(get_linkstrength(pt, at, av) > INITIAL_VIABILITY_THRESHOLD){break;}

		if(ctr == 500){

			cout << "add_InitialPair: Es konnte kein initiales Netz generiert werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

}

void Network::add_InitialPair(double at, double av, double time){

	// Würfle eine Pflanze aus.
	int ctr = -1;
	while(ctr < 500){

		double pt = (double)rand()/(double)RAND_MAX;
		if(get_linkstrength(pt, at, av) > INITIAL_VIABILITY_THRESHOLD){

			// Wenn die Linkstärke ausreichend stark ist, füge beide Spezies dem Netzwerk hinzu.
			PlantTraits.push_back(pt);
			PlantArrivals.push_back(time);
			noofplants++;

			AnimalTraits.push_back(at);
			AnimalVariances.push_back(av);
			AnimalArrivals.push_back(time);
			noofanimals++;

			fill_adj();
			update_animal_connectivities();

			// Neu. 
			PlantSurvived.push_back(0);
			AnimalSurvived.push_back(0);

		}

		ctr++;
		if(get_linkstrength(pt, at, av) > INITIAL_VIABILITY_THRESHOLD){break;}

		if(ctr == 500){

			cout << "add_InitialPair: Es konnte kein initiales Netz generiert werden (alle Versuche aufgebraucht)!" << endl;

		}

	}

}

void Network::delete_plants(vector<int> v){

	std::sort(v.begin(), v.end(), std::greater<>()); // Sortiert die Einträge so, dass "von hinten nach vorne" gelöscht wird. Das ist wichtig, damit die Ordnung in PlantTraits beibehalten wird.
	v.erase(unique(v.begin(), v.end()), v.end()); // Entfernt doppelte Einträge.

	if(v.size() <= noofplants)
	{

		vector<double> :: iterator it;
		vector<bool> :: iterator it2;

		for(int i = 0; i < v.size(); i++){

			if(v[i] < PlantTraits.size()){

				it = PlantTraits.begin();
				advance(it, v[i]);
				PlantTraits.erase(it);

				it2 = PlantSurvived.begin();
				advance(it2, v[i]);
				PlantSurvived.erase(it2);

				noofplants--;

			}
			else{

				cout << "delete_plants: Zu löschende Pflanze existiert nicht!" << endl;

			}

		}

		// Aktualisiere AdjacencyMatrix und AnimalConnectivities, nachdem alle Pflanzen gelöscht wurden.
		fill_adj();
		update_animal_connectivities();

	}
	else{

		cout << "delete_plants: Es sollen mehr Pflanzen gelöscht werden als im Netz vorhanden sind!" << endl;

	}

}

void Network::delete_plants(vector<int> v, double time){

	std::sort(v.begin(), v.end(), std::greater<>()); // Sortiert die Einträge so, dass "von hinten nach vorne" gelöscht wird. Das ist wichtig, damit die Ordnung in PlantTraits beibehalten wird.
	v.erase(unique(v.begin(), v.end()), v.end()); // Entfernt doppelte Einträge.

	if(v.size() <= noofplants)
	{

		vector<double> :: iterator it;
		vector<bool> :: iterator it2;		

		for(int i = 0; i < v.size(); i++){

			if(v[i] < PlantTraits.size()){

				it = PlantTraits.begin();
				advance(it, v[i]);
				PlantTraits.erase(it);

				it = PlantArrivals.begin();
				advance(it, v[i]);
				PlantArrivals.erase(it);

				it2 = PlantSurvived.begin();
				advance(it2, v[i]);
				PlantSurvived.erase(it2);

				noofplants--;

			}
			else{

				cout << "delete_plants: Zu löschende Pflanze existiert nicht!" << endl;

			}

		}

		// Aktualisiere AdjacencyMatrix und AnimalConnectivities, nachdem alle Pflanzen gelöscht wurden.
		fill_adj();
		update_animal_connectivities();

	}
	else{

		cout << "delete_plants: Es sollen mehr Pflanzen gelöscht werden als im Netz vorhanden sind!" << endl;

	}

}

void Network::delete_animals(vector<int> v){

	std::sort(v.begin(), v.end(), std::greater<>()); // Sortiert die Einträge so, dass "von hinten nach vorne" gelöscht wird. Das ist wichtig, damit die Ordnung in AnimalTraits usf. beibehalten wird.
	v.erase(unique(v.begin(), v.end()), v.end()); // Entfernt doppelte Einträge.

	if(v.size() <= noofanimals)
	{

		vector<double> :: iterator it;
		vector<bool> :: iterator it2;		

		for(int i = 0; i < v.size(); i++){

			if(v[i] < AnimalTraits.size()){

				it = AnimalTraits.begin();
				advance(it, v[i]);
				AnimalTraits.erase(it);

				it = AnimalVariances.begin();
				advance(it, v[i]);
				AnimalVariances.erase(it);

				it2 = AnimalSurvived.begin();
				advance(it2, v[i]);
				AnimalSurvived.erase(it2);

				noofanimals--;

			}
			else{

				cout << "delete_animals: Zu löschendes Tier existiert nicht!" << endl;

			}

		}

		// Aktualisiere AdjacencyMatrix und AnimalConnectivities, nachdem alle Tiere gelöscht wurden.
		fill_adj();
		update_animal_connectivities();

	}
	else{

		cout << "delete_animals: Es sollen mehr Tiere gelöscht werden als im Netz vorhanden sind!" << endl;

	}	

}

void Network::delete_animals(vector<int> v, double time){

	std::sort(v.begin(), v.end(), std::greater<>()); // Sortiert die Einträge so, dass "von hinten nach vorne" gelöscht wird. Das ist wichtig, damit die Ordnung in AnimalTraits usf. beibehalten wird.
	v.erase(unique(v.begin(), v.end()), v.end()); // Entfernt doppelte Einträge.

	if(v.size() <= noofanimals)
	{

		vector<double> :: iterator it;
		vector<bool> :: iterator it2;

		for(int i = 0; i < v.size(); i++){

			if(v[i] < AnimalTraits.size()){

				it = AnimalTraits.begin();
				advance(it, v[i]);
				AnimalTraits.erase(it);

				it = AnimalVariances.begin();
				advance(it, v[i]);
				AnimalVariances.erase(it);

				it = AnimalArrivals.begin();
				advance(it, v[i]);
				AnimalArrivals.erase(it);

				it2 = AnimalSurvived.begin();
				advance(it2, v[i]);
				AnimalSurvived.erase(it2);

				noofanimals--;

			}
			else{

				cout << "delete_animals: Zu löschendes Tier existiert nicht!" << endl;

			}

		}

		// Aktualisiere AdjacencyMatrix und AnimalConnectivities, nachdem alle Tiere gelöscht wurden.
		fill_adj();
		update_animal_connectivities();

	}
	else{

		cout << "delete_animals: Es sollen mehr Tiere gelöscht werden als im Netz vorhanden sind!" << endl;

	}	

}

// Funktionen, die mir mein Netzwerk aktualisieren, und Hilfsfunktionen. 

void Network::fill_adj(){

	AdjacencyMatrix.clear();
	for(int j = 0; j < noofanimals; j++){

		AdjacencyMatrix.push_back({});
		for(int i = 0; i < noofplants; i++){

			double ls = get_linkstrength(PlantTraits[i], AnimalTraits[j], AnimalVariances[j]);
			AdjacencyMatrix[j].push_back(ls);

		};
	};

}

void Network::update_animal_connectivities(){

	double c = 0;
	AnimalConnectivities.clear();

	for(int j = 0; j < noofanimals; j++){

		for(int i = 0; i < noofplants; i++){

			if(AdjacencyMatrix[j][i] > 0){c++;}

		};

		AnimalConnectivities.push_back(c);
		c = 0;

	};

}

void Network::print_adj(){

	cout << '\n' << "Adjazenzmatrix:" << '\n';
	for(int j = 0; j < noofanimals; j++)
	{

		for(int i = 0; i < noofplants; i++){

			cout << AdjacencyMatrix[j][i] << '\t';

		};

		cout << '\n';

	};

	cout << '\n' << endl;
}

void Network::get_information(){

	cout << endl;
	cout << "Anzahl Pflanzen: " << noofplants << endl;
	cout << "Anzahl Tiere: " << noofanimals << endl;

	cout << "\nPflanzen: ";
	for(int i = 0; i < noofplants; i++){

		cout << PlantTraits[i] << '\t';

	};
	cout << endl;

	cout << "\nTiere: ";
	for(int j = 0; j < noofanimals; j++){

		cout << "(" << AnimalTraits[j] << ", " << AnimalVariances[j] << ") ";

	};
	cout << endl;

	cout << "\nKonnektivitäten der Pflanzen: ";
	for(int i = 0; i < noofplants; i++){

		cout << get_PlantConnectivity(i) << '\t';

	};

	cout << "\nKonnektivitäten der Tiere: ";
	for(int j = 0; j < noofanimals; j++){

		cout << AnimalConnectivities[j] << '\t';

	};
	cout << endl;

	print_adj();

}

double Network::get_linkstrength(double pt, double at, double av){

	double ls = 1/(av*sqrt(2*M_PI))*exp(-pow((pt-at)/av,2));
	if(ls < 0.05){ls = 0;}
	return ls;

}

bool Network::is_linked(double t){

	bool is_linked = false;
	double ls;

	for(int j = 0; j < noofanimals; j++){

		ls = get_linkstrength(t, AnimalTraits[j], AnimalVariances[j]);
		if(ls > 0){is_linked = true;}

	};

	return is_linked;

}

bool Network::is_linked(double t, double v){

	bool is_linked = false;
	double ls;

	for(int i = 0; i < noofplants; i++){

		ls = get_linkstrength(PlantTraits[i], t, v);
		if(ls > 0){is_linked = true;}

	};

	return is_linked;

}