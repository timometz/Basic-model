#include "dynamics.h"
#include "network.h"

double PlantDynamics(int i, const state_type &x, Network network){

	double temp1; // Hält den Ausdruck für die Konkurrenz der Pflanzen.
	double temp2; // Hält den Ausdruck, der in Zähler und Nenner der functional response der Pflanze vorkommt.

	int MaxPlants = network.get_MaxPlants();
	vector<double> traits = network.get_PlantTraits();
	vector<double> connecs = network.get_AnimalConnectivities();
	vector<vector<double>> adj = network.get_AdjacencyMatrix();

	// Bestimmung von temp1.

	temp1 = 0;

	for(int m = 0; m < network.get_noofplants(); m++){

		temp1 = temp1 +	(u + (w-u)*exp(-pow( (network.get_PlantTraits()[i] - network.get_PlantTraits()[m])/xi, 2)))*x[m];

	};

	// Bestimmung von temp2.

	temp2 = 0;

	for(int m = 0; m < network.get_noofanimals(); m++){ 
        
        if(connecs[m] > 0){temp2 = temp2 + adj[m][i]*x[MaxPlants + m];}

	};

	return r*(1 - temp1)*(temp2/(r + temp2))*x[i] - mu_P*x[i]; 

}

double AnimalDynamics(int j, const state_type &x, Network network){

	double temp1; // Hält den Ausdruck im Zähler der einzelnen functional responses, über die für den Bestäuber summiert wird.
	double temp2; // Hält den Ausdruck im Nenner der einzelnen functional responses, über die für den Bestäuber summiert wird.
	double temp3; // Hält die Summe über alle functional responses, über die für den Bestäuber summiert wird.

	int MaxPlants = network.get_MaxPlants();
	vector<double> AnimalTraits = network.get_AnimalTraits();
	vector<double> connecs = network.get_AnimalConnectivities();
	vector<vector<double>> adj = network.get_AdjacencyMatrix();

	if(connecs[j] == 0){ // Wenn das Tier nicht verbunden ist, gibt es keinen Wachstumsterm.

		return - mu_A*x[MaxPlants + j];

	}
	else{

		// Bestimmung von temp3.

		temp3 = 0;

		for(int i = 0; i < network.get_noofplants(); i++){

			temp1 = adj[j][i]*x[i];
			
			temp2 = 0;
			for(int m = 0; m < network.get_noofanimals(); m++){

				temp2 = temp2 + adj[m][i]*x[MaxPlants + m];

			};

			temp3 = temp3 + temp1/(r + temp2);

		};

		return lambda*x[MaxPlants + j]*temp3 - mu_A*x[MaxPlants + j];

	}

}

state_type MakeInitialCondition(int MaxPlants, int MaxAnimals){

	int MaxSpecies = MaxPlants + MaxAnimals;
	state_type x(MaxSpecies);
	x.assign(MaxSpecies + 1, 0);
	x[0] = 0.05; // 10.0*PLANT_EXTINCTION_THRESHOLD; // also... 0.1. vorher: 1.0;
	x[MaxPlants] = 0.001; // 10.0*EXTINCTION_THRESHOLD; // also... 0.001. vorher: 1.0;

	return x;

}

void AddInitialDensity(Network network, state_type &x){

	int noofplants = network.get_noofplants();

	// Gehe alle mit Pflanzen korrespondierenden Einträge von x durch. Falls einer davon 0 ist, setze ihn auf auf einen Wert leicht oberhalb der Pflanzen-Aussterbeschwelle.
	for(int i = 0; i < noofplants; i++){

		if(x[i] == 0){x[i] = 1.0001*PLANT_EXTINCTION_THRESHOLD;}

	};

	int MaxPlants = network.get_MaxPlants();
	int noofanimals = network.get_noofanimals();

	// Gehe alle mit Tieren korresnpondieren Einträge von x durch. Falls einer davon 0 ist, setze ihn auf 0.01.
	for(int j = MaxPlants; j < MaxPlants + noofanimals; j++){

		if(x[j] == 0){x[j] = 1.0001*EXTINCTION_THRESHOLD;}

	};

}

bool DoExtinctions(Network &network, state_type &x){

	bool extinction_detected = false; // Kontrollvariable, die ihren Wert zu true ändert, falls ein Aussterbeereignis zum Halt der Intgeration geführt hat.

	// Überprüfung der Pflanzen.

	int noofplants = network.get_noofplants(); // noofplants entspricht der Anzahl der Pflanzen im Netzwerk vor Beginn der Integration. 
	vector<int> extinct_plants = {}; // Vektor, der die ausgestorbenen Pflanzen speichert.

	for(int i = 0; i < noofplants; i++){

		if(x[i] != 0 && x[i] < PLANT_EXTINCTION_THRESHOLD){ // Wenn eine Spezies ausgestorben ist, ...

			extinction_detected = true; // ändere den Ausgabewert von DoExtinctions() zu true, ...
			extinct_plants.push_back(i); // füge die Pflanze zur Liste ausgestorbener Pflanzen hinzu und ...
			x[i] = 0; // setze ihre Populationsdichte auf 0.

		}

	};

	network.delete_plants(extinct_plants); // Löscht die ausgestorbenen Pflanzen aus dem Netz.

	// Jetzt passen wir noch den Pflanzen-Teil des Zustandsvektors an. Sonst stehen ggf. mittendrin Nullen und NetDynamics arbeitet nicht korrekt.

	int ctr = 0; // Zählt die ausgestorbenen Pflanzen.
	for(int i = 0; i < noofplants; i++){

		if(x[i] == 0){ctr++;}

	};

	for(int k = 0; k < ctr; k++){

		for(int i = 1; i < noofplants; i++){

			if(x[i-1] == 0){swap(x[i-1], x[i]);} // Wenn das Element links neben Dir 0 ist, tausche mit ihm den Platz. 

		};

	};

	// Überprüfung der Tiere.

	int MaxPlants = network.get_MaxPlants();
	int noofanimals = network.get_noofanimals(); // noofanimals entspricht der Anzahl der Tiere im Netzwerk vor Beginn der Integration.
	vector<int> extinct_animals = {}; // Vektor, der die ausgestorbenen Tiere speichert.

	for(int j = MaxPlants; j < MaxPlants + noofanimals; j++){

		if(x[j] != 0 && x[j] < EXTINCTION_THRESHOLD){ // Wenn eine Spezies ausgestorben ist, ...

			extinction_detected = true; // ändere den Ausgabewert von DoExtinctions zu true, ...
			extinct_animals.push_back(j - MaxPlants); // füge das Tier zur Liste ausgestorbener Tiere hinzu und ...
			x[j] = 0; // setze die Populationsdichte auf 0.

		}

	};

	network.delete_animals(extinct_animals); // Löscht die ausgestorbenen Tiere aus dem Netz.

	// Jetzt passen wir noch den Tiere-Teil des Zustandsvektors an. Sonst stehen ggf. mittendrin Nullen und NetDynamics arbeitet nicht korrekt.

	ctr = 0; // Zählt nun die ausgestorbenen Tiere.
	for(int j = MaxPlants; j < MaxPlants + noofanimals; j++){

		if(x[j] == 0){ctr++;}

	};

	for(int k = 0; k < ctr; k++){

		for(int j = MaxPlants + 1; j < MaxPlants + noofanimals; j++){

			if(x[j-1] == 0){swap(x[j-1], x[j]);} // Wenn das Element links neben Dir 0 ist tausche mit ihm den Platz.

		};

	};

	return extinction_detected;

}

bool DoExtinctions(Network &network, state_type &x, double time){

	bool extinction_detected = false; // Kontrollvariable, die ihren Wert zu true ändert, falls ein Aussterbeereignis zum Halt der Intgeration geführt hat.

	// Überprüfung der Pflanzen.

	int noofplants = network.get_noofplants(); // noofplants entspricht der Anzahl der Pflanzen im Netzwerk vor Beginn der Integration. 
	vector<int> extinct_plants = {}; // Vektor, der die ausgestorbenen Pflanzen speichert.

	for(int i = 0; i < noofplants; i++){

		if(x[i] != 0 && x[i] < PLANT_EXTINCTION_THRESHOLD){ // Wenn eine Spezies ausgestorben ist, ...

			extinction_detected = true; // ändere den Ausgabewert von DoExtinctions() zu true, ...
			extinct_plants.push_back(i); // füge die Pflanze zur Liste ausgestorbener Pflanzen hinzu und ...
			x[i] = 0; // setze ihre Populationsdichte auf 0.

		}

	};

	network.delete_plants(extinct_plants, time); // Löscht die ausgestorbenen Pflanzen aus dem Netz.

	// Jetzt passen wir noch den Pflanzen-Teil des Zustandsvektors an. Sonst stehen ggf. mittendrin Nullen und NetDynamics arbeitet nicht korrekt.

	int ctr = 0; // Zählt die ausgestorbenen Pflanzen.
	for(int i = 0; i < noofplants; i++){

		if(x[i] == 0){ctr++;}

	};

	for(int k = 0; k < ctr; k++){

		for(int i = 1; i < noofplants; i++){

			if(x[i-1] == 0){swap(x[i-1], x[i]);} // Wenn das Element links neben Dir 0 ist, tausche mit ihm den Platz. 

		};

	};

	// Überprüfung der Tiere.

	int MaxPlants = network.get_MaxPlants();
	int noofanimals = network.get_noofanimals(); // noofanimals entspricht der Anzahl der Tiere im Netzwerk vor Beginn der Integration.
	vector<int> extinct_animals = {}; // Vektor, der die ausgestorbenen Tiere speichert.

	for(int j = MaxPlants; j < MaxPlants + noofanimals; j++){

		if(x[j] != 0 && x[j] < EXTINCTION_THRESHOLD){ // Wenn eine Spezies ausgestorben ist, ...

			extinction_detected = true; // ändere den Ausgabewert von DoExtinctions zu true, ...
			extinct_animals.push_back(j - MaxPlants); // füge das Tier zur Liste ausgestorbener Tiere hinzu und ...
			x[j] = 0; // setze die Populationsdichte auf 0.

		}

	};

	network.delete_animals(extinct_animals, time); // Löscht die ausgestorbenen Tiere aus dem Netz.

	// Jetzt passen wir noch den Tiere-Teil des Zustandsvektors an. Sonst stehen ggf. mittendrin Nullen und NetDynamics arbeitet nicht korrekt.

	ctr = 0; // Zählt nun die ausgestorbenen Tiere.
	for(int j = MaxPlants; j < MaxPlants + noofanimals; j++){

		if(x[j] == 0){ctr++;}

	};

	for(int k = 0; k < ctr; k++){

		for(int j = MaxPlants + 1; j < MaxPlants + noofanimals; j++){

			if(x[j-1] == 0){swap(x[j-1], x[j]);} // Wenn das Element links neben Dir 0 ist tausche mit ihm den Platz.

		};

	};

	return extinction_detected;

}

double GetPlantGrowthRate(int i, const state_type &x, Network network){

	return PlantDynamics(i, x, network);

}

double GetAnimalGrowthRate(int j, const state_type &x, Network network){

	return AnimalDynamics(j, x, network);

}