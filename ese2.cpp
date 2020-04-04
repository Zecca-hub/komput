#include "vector_help.h"
#include "integrate.h"
#include "findzero.h"
#include "ode_solvers.h"
#include "functions.h"

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <boost/format.hpp>

// dtheta_t/dxi = phi(x,theta,phi)
double f1(double x,double theta,double phi){
	return phi;
}

// dphi/dzeta = f2(x,theta,phi)
// double f2(double x,double theta,double phi){
// 	return -2/x*phi-theta;
// }
double f2_tmp(double x,double theta,double phi,
				double n){
	return -2/x*phi-std::pow(theta,n);
}

#define PART 2
int main(int argc, char const *argv[]){
	#if PART == 0
	// mathematical portion

	double theta_0{1};
	double phi_0{0};
	// start of integrating interval
	double x_0{0+1e-5};

	// end of the interval (exluded)
	double x_end{7};
	uint32_t M{100};
	double h{(x_end-x_0)/M};

	std::vector<double> n(7);
	linespace(n,1.5,3);

	std::vector<double> theta(M);
	std::vector<double> phi(M);
	std::vector<double> x(M);
	theta[0]=theta_0;
	phi[0]=phi_0;
	arange(x,x_0,h);

	std::cout<<boost::format("Initial configuration\n\
		\r\tM: %d\n\tintegration step: %f\n\ttheta_0: %f\n\tphi_0: %f\n") %M % h % theta_0 % phi_0
		<<std::endl;

	// for(uint32_t j=0;j<static_cast<uint32_t>(n.size());++j){
	for(uint32_t j=0;j<1;++j){
		std::cout<<"Integrating n: "<<n[j]<<std::endl;

		std::function<double(double,double,double)> f2{
			[&] (double x,double theta,double phi)->double { return f2_tmp(x,theta,phi,n[j]); }
		};


		for(uint32_t i=1;i<M;++i){
			RK_2_step(f1,f2,x[i],theta[i-1],phi[i-1],h,&(theta[i]),&(phi[i]));
		}

		tocsv(
			{{"x",x},
			{"theta",theta},
			{"phi",phi}},
			(boost::format("output_data/neutron_n_%.2f.csv") % n[j]).str());
	}
	return 0;
	n.clear();
	n.push_back(1.0);
	n.push_back(4.5);
	x_end=40;
	h=(x_end-x_0)/M;
	arange(x,x_0,h);

	for(uint32_t j=0;j<static_cast<uint32_t>(n.size());++j){
		std::cout<<"Integrating n: "<<n[j]<<std::endl;

		std::function<double(double,double,double)> f2{
			[&] (double x,double theta,double phi)->double { return f2_tmp(x,theta,phi,n[j]); }
		};


		for(uint32_t i=1;i<M;++i){
			RK_2_step(f1,f2,x[i],theta[i-1],phi[i-1],h,&(theta[i]),&(phi[i]));
		}

		tocsv(
			{{"x",x},
			{"theta",theta}},
			(boost::format("output_data/neutron_n_%.2f.csv") % n[j]).str() );
	}
	#elif PART==2

	double n=3.0/2;
	int M=100;
	// taken from the file
	// double theta_0{2.39488e-3};
	double phi_0{-2.05235e-1};
	double x_0{3.57};
	std::vector<double> mass(M); // M0
	linespace(mass,1.5,3);

	double h_bar=1.05457148e-34; // J*s
	double m_n=1.67492749804e-27; // Kg
	double k_1_5=std::pow(h_bar,2)*std::pow(3*M_PI*M_PI,2.0/3)/(2*std::pow(m_n,8.0/3)); //
	double G=6.67430e-11; // Jm/Kg^2

	std::cout<<"Setted n: "<<n<<"\nphi_0: "<<phi_0<<std::endl;

	std::function<double(double)> radius_from_mass_index{
		[&] (uint32_t i)->double {
			return std::pow( -4*M_PI*std::pow(5.0*k_1_5/8/M_PI/G,3.0)*std::pow(x_0,5)/(1.989e30*mass[i])*phi_0 ,1.0/3);
		}
	};

	std::vector<double> radius(M); // meters
	map(radius,radius_from_mass_index);

	tocsv(
		{{"radius",radius},
		{"mass",mass}},
		"output_data/mass_radius_1_5.csv" );

	#endif
	return 0;
}
