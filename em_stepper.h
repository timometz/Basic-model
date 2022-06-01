#ifndef EM_STEPPER_H
#define EM_STEPPER_H

#include <vector>
#include <iostream>
#include <boost/random.hpp>
#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>

// PROGRAMM: Definition eines Euler-Maruyama-Steppers.

using namespace std;
using namespace boost::numeric::odeint;
typedef std::vector<double> state_type;

class em_stepper
{
public:

    typedef std::vector<double> state_type;
    typedef std::vector<double> deriv_type;
    typedef double value_type;
    typedef double time_type;
    typedef unsigned short order_type;

    typedef boost::numeric::odeint::stepper_tag stepper_category;

    static order_type order(void){return 1;} // Gibt die Ordnung des Steppers an. 

    template<class System> // Der Stepper arbeitet mit Systemen. 
    void do_step(System system, state_type &x, time_type t, time_type dt) const
    {

        deriv_type det, stoch;
        system.first(x, det);
        system.second(x, stoch);

        for(int i = 0; i < x.size(); i++){

            x[i] = x[i] + det[i]*dt + sqrt(x[i])*stoch[i];

        };

    }

};

// Definition eines Observers, der berechnete Zustände ausgibt.
struct streaming_observer
{

    template<class State> // Die Struktur arbeitet auf "Zuständen".
    void operator()(const State &x, double t) const
    {
        
        cout << t << '\t';
        for(int i = 0; i < x.size(); i++){

            cout << x[i] << '\t';

        };
        cout << endl;

    }

};

#endif