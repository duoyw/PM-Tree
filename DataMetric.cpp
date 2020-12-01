#include"pch.h"
#include "DataMetric.h"
#include<algorithm>
#include<fstream>
void DataMetric::load_Raw_Data(const string & data_file_)
{
	ifstream infile(data_file_);
	if (infile) {
		int datasize = 0;
		int ignore;
		infile >> dimension >> datasize >> ignore;
		//datasize = 100;
		this->dataset.resize(datasize);
		for (int i = 0; i < this->dataset.size(); ++i) {
			vector<double> tmp;
			for (int j = 0; j < dimension; ++j) {
				double data;
				infile >> data;
				tmp.push_back(data);
			}
			dataset[i]=tmp;
		}
	}
	else {
		cout << "error in load_Raw_Data \n";
		while (1);
	}
}

void DataMetric::load_Query_Data(const string & data_file_,int dimension_)
{
	this->dimension = dimension_;
	ifstream infile(data_file_);
	if (infile) {
		while(infile) {		
			vector<double> tmp;
			for (int j = 0; j < dimension_; ++j) {
				double data;
				infile >> data;
				if (!infile) break;
				tmp.push_back(data);
			}
			if (!infile) break;
			dataset.push_back(tmp);
		}
	}
	else {
		cout << "error in load_Query_Data \n";
		while (1);
	}
}
