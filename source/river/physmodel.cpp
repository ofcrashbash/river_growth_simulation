#include <iostream>

#include "physmodel.hpp"

using namespace std;

namespace River
{
    bool Model::q_biffurcate(vector<double> a)
    {
        cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << biffurcationThreshold << endl;
        return a.at(2)/a.at(0) < biffurcationThreshold && a.at(0) > 0.08;
    }

    function<double(double, double)> Model::Gain(int index)
    {
        switch(index)
        {
            case 0:
                return [](double R, double phi)
                    {return cos(phi/2)/M_PI/sqrt(R)/(Model::Rmax - Model::Rmin);};
            case 1:
                return [](double R, double phi)
                    {return sin(phi)/M_PI/R/(Model::Rmax - Model::Rmin);};
            case 2:
                return [](double R, double phi)
                    {return cos(3*phi/2)/M_PI/sqrt(R)/R/(Model::Rmax - Model::Rmin);};
            default:
                throw std::invalid_argument( "index should be 0 or 1 or 2");
                break;
        }
    }

    Polar Model::next_point(vector<double> series_params)
    {
        //FIXME: should here be - or +????
        double phi = atan(2 * series_params[2]/series_params[1] * sqrt(ds));
        return {ds, phi};
    }

}