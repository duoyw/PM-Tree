#pragma once

#include<string>
#include<iostream>
#include<vector>
#include<fstream>
#include<assert.h>
#include <io.h>
#include <iomanip>

using namespace std;

class DataMetric
{
public:
	DataMetric() {
		
	};

	~DataMetric() {};
	void load_Raw_Data(const string & data_file_);
	void load_Query_Data(const string & data_file_, int dimension_);
	int size()const { return dataset.size(); }
	const vector<vector<double>>& getData() const { return dataset; }
	const vector<double>& operator[](int i) const {
		return dataset[i];
	}
	vector<double>& operator[](int i) {
		return dataset[i];
	}
private:
	vector<vector<double>> dataset;  //第一个坐标代表经度，第二个坐标代表纬度
	int dimension;
};






