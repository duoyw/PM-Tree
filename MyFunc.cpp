#include "pch.h"
#include "MyFunc.h"
#include <iostream>
#include<algorithm>
#define _USE_MATH_DEFINES
#include<math.h>

using namespace std;


double MyFunc::Cal_Euclidean_distance(const vector<double>& a, const vector<double>& b)
{
	if (a.size() != b.size()) { cout << "the size don't match in Cal_Euclidean_distance" << endl; }
	double sum = 0;
	for (int i = 0; i < a.size(); ++i) {
		sum += (a[i] - b[i])*(a[i] - b[i]);
	}
	return sqrt(sum);
}

double MyFunc::Cal_Hypersphere_Volume(double radius_, int dim_)
{
	double a = 2 * pow(M_PI, double(dim_) /2.0);
	double b = double(dim_) * tgamma(double(dim_) / 2.0);
	double c = pow(radius_, double(dim_));
	return (a*c)/b;

}
