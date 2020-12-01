#pragma once
#include<vector>

using namespace std;
class RandomData
{
public:
	RandomData();
	RandomData(int data_num_,int data_len_) { Random(data_num_, data_len_); };
	~RandomData();
	void Random(int data_num_, int data_len_);
	int size() { return dataset.size(); }
	const vector<double>& operator[](int i) const {
		return dataset[i];
	}
	vector<double>& operator[](int i) {
		return dataset[i];
	}
	vector<vector<double>> dataset;  //第一个坐标代表经度，第二个坐标代表纬度
};

