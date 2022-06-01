#include "network.h"
#include "dynamics.h"
#include "gillespie.h"
#include "em_stepper.h"
#include <boost/numeric/odeint.hpp>
#include <boost/range/algorithm.hpp>
#include <fstream>
#include <iomanip>

// DATUM: 2021-02-19
// VERSION: Simulation System mit demographischem Rauschem für Paper-Zusammenstellung.
// TEILVERSION: Initiale Dichte von Pflanzen und Bestäubern angepasst. Output angepasst.
//
// Genutzter Code:
// network.h und network.cpp: Klasse Network, welche Struktur der community speichert
// dynamics.h und dynamics.cpp: Funktionen zur Erstellung, Lösung und Verwaltung der Dynamik / Parameter der Dynamik (!)
// gillespie.h: Einbindung stochastischer Migrationsereignisse.
// em_stepper.h: Beinhaltet den Euler-Maruyama-Stepper.
//
// NOTE: Die structs und Funktionen wurden ggü. der Version aus Q3 2020 teilweise angepasst (z. B. Argumente der Gillespie-Funktion, Wechsel auf den Mersenne-Twister, ...).
// Die verschiedenen Versionen aus den Quartalen sind, wahrscheinlich, nicht mehr kompatibel! Das hier ist nun der aktuelle branch. 
//
// In dieser Version herausgeschrieben: Anzahl an Pflanzen, Bestäubern, Spezies im Netzwerk über die Zeit. Entwicklung der Spezialisierung über die Zeit inkl. Strategiediversität.
//								 		Eigenschaften der Spezies im Netzwerk zu bestimmten Zeiten (Korinna-Tobi-Plot). Anzahl an Eindringlingen mit positiver initialer Wachstumsrate.
//										Daten für den Plot des Netzwerks via Python.

using namespace std; 
using namespace boost::numeric::odeint;

int main(int argc, char** argv){

	// Überprüft, ob die richtige Anzahl an Argumenten bei der Ausführung des Programms angegeben wurde.
	if(argc != 9){

		cout << "Falsche Parameteranzahl! Aufruf mit: (MaxPlants) (MaxAnimals) (T_MAX) (Schrittweite) (Rauschstärke) (MIG_RATE) (Seed) (NUMMER DES RUNS)." << endl; 

	}

	const clock_t begin_time = clock(); // Für Kontrolle der vom Programm benötigten Zeitspanne.

	double dt, dt1, Amp, mu;
	double T_NEXT, T_MAX;

	// Übernahme der eingegebenen Parameter.
	MaxPlants = atoi(argv[1]);
	MaxAnimals = atoi(argv[2]);
	const static int MaxSpecies = MaxPlants + MaxAnimals;
	T_MAX = atof(argv[3]);
	dt = atof(argv[4]);
	Amp = atof(argv[5]); 
	mu = atof(argv[6]);
	int seed = atoi(argv[7]);
	std::string rn = argv[8];

	std::string smu = argv[6];
	std::string sAmp = argv[5];

	boost::mt19937 rng(seed); // Seeden des Zufallsgenerators.
	srand(seed); // Seeden des Zufallsgenerators (für network.cpp).

	// Output in das slurm-File.
	cout << "Verwendete Parameter in dieser Simulation: " << endl << endl;
	cout << "MaxPlants: " << MaxPlants << endl;
	cout << "MaxAnimals: " << MaxAnimals << endl;
	cout << "T_MAX: " << T_MAX << endl;
	cout << "dt: " << dt << endl;
	cout << "Amp: " << Amp << endl;
	cout << "µ: " << mu << endl;
	cout << "Seed: " << seed << endl;

	// Hier ggf. Erstellung der Ausgabe-Dateien...

	std::string name_of_file = "NoOfSpeciesOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat";
	std::string name_of_file2 = "SpecializationOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat";
	std::string name_of_file3 = "PlantDataTraitPlotOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat"; // Daten für den Ameisenfußball-Plot für Pflanzen.
	std::string name_of_file4 = "AnimalDataTraitPlotOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat"; // Daten für den Ameisenfußball-Plot für Bestäuber.
	std::string name_of_file5 = "AverageAgeOfResidentSpeciesOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat"; // Daten für den Ameisenfußball-Plot für Bestäuber.
	std::string name_of_file8 = "InvadersWithPositiveInitialGrowthRateOverTime_mu_" + smu + "_Amp_" + sAmp + "_" + rn + ".dat";

    // ... und Öffnen der Dateien zum Reinschreiben. 

   	ofstream ofs(name_of_file.c_str(), ios::out | ios::trunc);
   	ofstream ofs2(name_of_file2.c_str(), ios::out | ios::trunc);   	
   	ofstream ofs3(name_of_file3.c_str(), ios::out | ios::trunc);
   	ofstream ofs4(name_of_file4.c_str(), ios::out | ios::trunc);
   	ofstream ofs5(name_of_file5.c_str(), ios::out | ios::trunc);
   	ofstream ofs8(name_of_file8.c_str(), ios::out | ios::trunc);

	int fail_ctr = 0; // Zählt die gescheiterten Assemblierungen.
		
	for(int counter = 0; counter < 1; counter++){

	// Aufsetzen des Netzwerks und der Assemblierung.
	Network network; // Generierung des (leeren) Netzwerks.
	network.add_InitialPair(rng, 0); // Generierung eines zufälligen Startnetzes.
	network.set_MaxPlants(MaxPlants);
	network.set_MaxAnimals(MaxAnimals);

	state_type x = MakeInitialCondition(MaxPlants, MaxAnimals); // Initialisiert Anfangszustand des Netzes.
																// Hat die Dimension (MaxSpecies + 1). Die letzte Koordinate trackt die Zeit. 

	// Output zu Beginn der Simulation. (Zeitpunkt 0, aber zwecks logarithmischer Darstellung 1.)
	ofs << 1 << '\t' << network.get_noofplants() << '\t' << network.get_noofanimals() << '\t' << network.get_noofplants() + network.get_noofanimals() << endl;
	ofs2 << 1 << '\t' << network.get_AverageDistance() << '\t' << network.get_AverageVariance() << '\t' << network.get_AverageInverseVariance() << '\t' << network.get_MaximumVariance() << '\t' << network.get_MinimumVariance() << '\t' << network.get_StrategyDiversity() << endl;
	
	ofs3 << 1 << '\t' << network.get_PlantTraits()[0] << endl;
	ofs4 << 1 << '\t' << network.get_AnimalTraits()[0] << '\t' << network.get_AnimalVariances()[0] << endl;

	ofs5 << 1 << '\t' << 0 << '\t' << 0 << '\t' << 0 << endl;

	ofs8 << 1 << '\t' << 0 << '\t' << 0 << '\t' << 0 << endl;

	double T_NEXT = get_next_step(rng, mu); // Zeitpunkt des ersten Migrationsereignisses.

	int mig_ctr = 0; // Zählt die Anzahl erfolgter Migrationen für die aktuelle Assemblierung.
	int pigr = 0; // Zählt Spezies, deren initiale Wachstumsrate positiv ist.
	int pigr_plants = 0; // Zählt Pflanzen, deren initiale Wachstumsrate positiv ist.
	int pigr_animals = 0; // Zählt Bestäuber, deren initiale Wachstumsrate positiv ist.

	bool mig_successful = 0; 
	double tola = 0;

	while(x.back() < T_MAX){

		// Wenn im nächsten Zeitschritt ein Migrationsereignis passiert, passe die Schrittgröße an und lasse eine Spezies migrieren.
		if(x.back() + dt >= T_NEXT){

			dt1 = T_NEXT - x.back();

			em_stepper().do_step(make_pair(NetDynamics(network), Noise(rng, sqrt(dt1), Amp, network)), x, 0.0, dt1);
			DoExtinctions(network, x, x.back()); // Zeitangabe im letzten Argument ist egal. Wichtig ist, dass dort ein double steht. 

			// Output unmittelbar vor der Migration der nächsten Spezies.

			if((mig_ctr <= 10) || (mig_ctr > pow(10, 1) && mig_ctr <= pow(10, 2) && mig_ctr % 1 == 0) || (mig_ctr > pow(10, 2) && mig_ctr <= pow(10, 3) && mig_ctr % 10 == 0) || (mig_ctr > pow(10, 3) && mig_ctr <= pow(10, 4) && mig_ctr % 100 == 0) || (mig_ctr > pow(10, 4) && mig_ctr <= pow(10, 5) && mig_ctr % 1000 == 0) || (mig_ctr > pow(10, 5) && mig_ctr <= pow(10, 6) && mig_ctr % 10000 == 0) || (mig_ctr > pow(10, 6) && mig_ctr <= pow(10, 7) && mig_ctr % 100000 == 0)){

				ofs << x.back()*mu + 1 << '\t' << network.get_noofplants() << '\t' << network.get_noofanimals() << '\t' << network.get_noofplants() + network.get_noofanimals() << endl;
				ofs2 << x.back()*mu + 1 << '\t'  << network.get_AverageDistance() << '\t' << network.get_AverageVariance() << '\t' << network.get_AverageInverseVariance() << '\t' << network.get_MaximumVariance() << '\t' << network.get_MinimumVariance() << '\t' << network.get_StrategyDiversity() << endl;
				
				for(int k = 0; k < network.get_noofplants(); k++){

					ofs3 << x.back()*mu + 1 << '\t' << network.get_PlantTraits()[k] << endl;

				}

				for(int k = 0; k < network.get_noofanimals(); k++){

					ofs4 << x.back()*mu + 1 << '\t' << network.get_AnimalTraits()[k] << '\t' << network.get_AnimalVariances()[k] << endl;

				}

				ofs5 << x.back()*mu + 1 << '\t' << (x.back() - network.get_AveragePlantArrival())*mu << '\t' << (x.back() - network.get_AverageAnimalArrival())*mu << '\t' << (x.back() - network.get_AverageSpeciesArrival())*mu << endl;

				ofs8 << x.back()*mu + 1 << '\t' << pigr_plants << '\t' << pigr_animals << '\t' << pigr << endl;

			}

			// Hier befüllen wir die Akkumulatoren.
			// Wenn die zuletzt migrierte Spezies bis zur nächsten Migration überlebt hat, generiere einen Eintrag 1. Ansonsten generiere einen Eintrag 0.
				// Wenn die zuletzt migrierte Spezies noch im Netzwerk ist, ist tola enthalten in einem der ArrivalTime-Vektoren. 

			// Falls noch keine Migration passiert ist, ist das Überleben der beiden Anfangsspezies bis hierhin separat zu überprüfen (Sonderfall).
			if(mig_ctr == 0){

				if(network.get_noofplants() > 0){

					network.set_SurvivalOfMostRecentPlant(1);
					// acc20k(1); acc50k(1); acc100k(1);

				}

				if(network.get_noofplants() == 0){

					// acc20k(0); acc50k(0); acc100k(0);

				}

				if(network.get_noofanimals() > 0){

					network.set_SurvivalOfMostRecentAnimal(1);
					// acc20k(1); acc50k(1); acc100k(1);

				}

				if(network.get_noofanimals() == 0){

					// acc20k(0); acc50k(0); acc100k(0);

				}

			}

			// Falls bereits eine Migration geschehen ist, müssen wir lediglich prüfen, ob der jüngste Bestäuber/die jüngste Pflanze tola als ArrivalTime hat.
			mig_successful = 0; // Default-Annahme: Die zuletzt migrierte Spezies hat nicht bis hierhin (T_NEXT) überlebt.
			if(mig_ctr > 0){

				if(network.get_noofplants() > 0){

					if(network.get_PlantArrivals().back() == tola){mig_successful = 1; network.set_SurvivalOfMostRecentPlant(1);}

				}

				if(network.get_noofanimals() > 0){

					if(network.get_AnimalArrivals().back() == tola){mig_successful = 1; network.set_SurvivalOfMostRecentAnimal(1);}

				}

				if(mig_successful == 1){

					// acc20k(1); acc50k(1); acc100k(1);

				}

				if(mig_successful == 0){

					// acc20k(0); acc50k(0); acc100k(0);

				}

			}

			// Output für python. Immer nach 5*10^n und 10^n Migrationen, mit n = 1, 2, 3, ...
			if(seed % 50 == 0 && (mig_ctr == 5 || mig_ctr == 50 || mig_ctr == 500 || mig_ctr == 5000 || mig_ctr == 50000 || mig_ctr == 500000 || mig_ctr == 5000000 || mig_ctr == 50000000 || mig_ctr == 10 || mig_ctr == 100 || mig_ctr == 1000 || mig_ctr == 10000 || mig_ctr == 100000 || mig_ctr == 1000000 || mig_ctr == 10000000)){

				string str = to_string(mig_ctr);

				std::string name_of_file6 = "py_Animals_mu_" + smu + "_Amp_" + sAmp + "_" + str + "_" + rn + ".txt"; 
				std::string name_of_file7 = "py_Plants_mu_" + smu + "_Amp_" + sAmp + "_" + str + "_" + rn + ".txt"; 
				std::string name_of_file9 = "py_Matrix_mu_" + smu + "_Amp_" + sAmp + "_" + str + "_" + rn + ".txt"; 

				ofstream ofs6(name_of_file6.c_str(), ios::out | ios::trunc);
				ofstream ofs7(name_of_file7.c_str(), ios::out | ios::trunc);
				ofstream ofs9(name_of_file9.c_str(), ios::out | ios::trunc);

				for(int k = 0; k < network.get_noofanimals(); k++){

				ofs6 << network.get_AnimalTraits()[k] << '\t' << x[MaxPlants + k] << endl;

				};

				for(int k = 0; k < network.get_noofplants(); k++){

				ofs7 << network.get_PlantTraits()[k] << '\t' << x[k] << endl;

				};

				for(int j = 0; j < network.get_noofanimals(); j++){

					for(int m = 0; m < network.get_noofplants(); m++){

						ofs9 << network.get_AdjacencyMatrix()[j][m] << '\t';

					};

					ofs9 << endl;

				};

				ofs6.close();
				ofs7.close();
				ofs9.close();

			}

			network.add_species(rng, x.back()); mig_ctr++;
			AddInitialDensity(network, x);
			T_NEXT = x.back() + get_next_step(rng, mu);

			tola = x.back();

			// Spezies ist gerade ins Netzwerk migriert.
			if(network.get_noofplants() > 0){

				if(network.get_PlantSurvived().back() == 0 && network.get_PlantArrivals().back() == tola){

					// cout << "Initiale Wachstumsrate: " << GetPlantGrowthRate(network.get_noofplants() - 1, x, network) << endl; 

					if(GetPlantGrowthRate(network.get_noofplants() - 1, x, network) > 0){pigr++; pigr_plants++;};

				};

			}
			
			if(network.get_noofanimals() > 0){

				if(network.get_AnimalSurvived().back() == 0 && network.get_AnimalArrivals().back() == tola){

					// cout << "Initiale Wachstumsrate: " << GetAnimalGrowthRate(network.get_noofanimals() - 1, x, network) << endl;

					if(GetAnimalGrowthRate(network.get_noofanimals() - 1, x, network) > 0){pigr++; pigr_animals++;};

				}; 

			}

		}

		// Ansonsten mache einen regulären dt-Schritt.
		if(x.back() + dt < T_NEXT){

			em_stepper().do_step(make_pair(NetDynamics(network), Noise(rng, sqrt(dt), Amp, network)), x, 0.0, dt);
			DoExtinctions(network, x, x.back()); // Zeitangabe im letzte Argument ist egal. Wichtig ist, dass dort ein double steht. 

		}

		// Abbruch der Assemblierung und Clearen der Output-Files, wenn # Spezies = 0.
		if(network.get_noofplants() + network.get_noofanimals() == 0){

			fail_ctr++; counter--; 

			// Schließen der Dateien, die den Verlauf einer Assemblierung protokollieren, und Öffnen dieser von Neuem.

			ofs.close(); ofs.open(name_of_file.c_str(), ios::out | ios::trunc);
			ofs2.close(); ofs2.open(name_of_file2.c_str(), ios::out | ios::trunc);
			ofs3.close(); ofs3.open(name_of_file3.c_str(), ios::out | ios::trunc);
			ofs4.close(); ofs4.open(name_of_file4.c_str(), ios::out | ios::trunc);

			ofs5.close(); ofs5.open(name_of_file5.c_str(), ios::out | ios::trunc);

			ofs8.close(); ofs8.open(name_of_file8.c_str(), ios::out | ios::trunc);

			break;

		}

		// Hier abschließenden Output für x.back() >= T_MAX ausgeben.
		if(x.back() >= T_MAX && counter == 0){

			ofs << x.back()*mu + 1 << '\t' << network.get_noofplants() << '\t' << network.get_noofanimals() << '\t' << network.get_noofplants() + network.get_noofanimals() << endl;
			ofs2 << x.back()*mu + 1 << '\t'  << network.get_AverageDistance() << '\t' << network.get_AverageVariance() << '\t' << network.get_AverageInverseVariance() << '\t' << network.get_MaximumVariance() << '\t' << network.get_MinimumVariance() << '\t' << network.get_StrategyDiversity() << endl;

			for(int k = 0; k < network.get_noofplants(); k++){

				ofs3 << x.back()*mu + 1 << '\t' << network.get_PlantTraits()[k] << endl;

			}

			for(int k = 0; k < network.get_noofanimals(); k++){

				ofs4 << x.back()*mu + 1 << '\t' << network.get_AnimalTraits()[k] << '\t' << network.get_AnimalVariances()[k] << endl;
					
			}

			ofs5 << x.back()*mu + 1 << '\t' << (x.back() - network.get_AveragePlantArrival())*mu << '\t' << (x.back() - network.get_AverageAnimalArrival())*mu << '\t' << (x.back() - network.get_AverageSpeciesArrival())*mu << endl;

			ofs8 << x.back()*mu + 1 << '\t' << pigr_plants << '\t' << pigr_animals << '\t' << pigr << endl;	

		}

	}

	// Hier kommt ggf. der Output hin, der immer dann ausgegeben wird, wenn ein Netzwerk vollständig assembliert wurde. 
	if(counter == 0){

		cout << endl;
		cout << "Ausgabe von Informationen zum Ende der Assemblierung: " << endl; 

		network.get_information();

		cout << "Benötigte Zeit: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " Sekunden" << endl; 

		cout << "Anzahl zuvor gescheiterter Assemblierungen: " << fail_ctr << endl;

		cout << "Anzahl erfolgreicher Assemblierungen: " << counter + 1 << endl;

		cout << "Simulierte Zeitspanne: " << x.back() << endl;

		cout << "Anzahl Migrationsereignisse: " << mig_ctr << endl;

		cout << "Anteil Migrationsereignisse mit positivem initialen Wachstum: " << (double)pigr/(double)mig_ctr << endl;

		for(int i = 0; i < network.get_noofplants(); i++){

			cout << x[i] << '\t';

		}
		cout << endl;

		// cout << "Anzahl Migrationen bisher insg.: " << tot_ctr << endl;*/

	}

	}

	return 0;

}