#include "pch.h"
#include "RandomData.h"
#include<random>
#include<time.h>
RandomData::RandomData()
{
}


RandomData::~RandomData()
{
}

void RandomData::Random(int data_num_,int data_len_)
{
	default_random_engine rng(0);
	uniform_real_distribution<double> random_double_uniform(1,1.5);
	dataset.resize(data_num_);
	for (int i = 0; i < dataset.size(); ++i) {
		for (int j = 0; j < data_len_; ++j) {
			double random_data = random_double_uniform(rng);
			dataset[i].push_back(random_data);
		}
	}
}
