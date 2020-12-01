// M_Tree.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include <iostream>
#include<vector>
#include<algorithm>
#include<random>
#include<time.h>
#include"RandomData.h"
#include"PM_Tree.h"
#include"MyFunc.h"
#include<set>
#include"DataMetric.h"
#include"Timer.h"

using namespace std;
void Random_Test();
void Read_Data_Test();
void Printf_Result(ofstream& out, double query_time_, double cost_, double all_cal_distance_num_, double range_, double aver_result_num_, double all_pivot_filter_num_);
void Printf_Result_Csv(ofstream& out,vector<double> outData);
vector<vector<double>> Random_pivot(RandomData& raw_data_, int pivot_num_, int N_);
vector<vector<double>> Random_pivot(DataMetric& raw_data_, int pivot_num_, int N_);
double distance_Between_Piovt_Vector(vector<vector<double>>& pivot_vector_);

string data_file_path = "C:\\application\\workspace\\C++\\Data_High_To_Low_dimension\\Data_High_To_Low_dimension\\Data_High_To_Low_dimension\\dataset\\15_low_dim_gist_da_nq.txt";
string query_file_path = "C:\\application\\workspace\\C++\\Data_High_To_Low_dimension\\Data_High_To_Low_dimension\\Data_High_To_Low_dimension\\dataset\\15_low_gist_q.txt";
extern Timer time_buff[10];
extern double time_res[10];;

int main()
{
	Read_Data_Test();
	//Random_Test();
}
void Read_Data_Test()
{
	int M = 16;
	int KNN = 200;
	int Pivot_Num =3;
	int N = 100;//重复选取Pivot次数
	//double tmp_range_buff[]= { 100,110,115,120,125,130,140};
	double tmp_range_buff[] = { 80 };
	vector<double>range_buff(tmp_range_buff, tmp_range_buff + sizeof(tmp_range_buff) / sizeof(tmp_range_buff[0]));
	/*装载数据*/
	cout << "Load Data\n";
	DataMetric rawData;
	DataMetric queryData;
	rawData.load_Raw_Data(data_file_path);
	queryData.load_Query_Data(query_file_path, rawData[0].size());
	cout << "Load Data Ok\n";

	/*构建索引*/
	cout << "Coustruct Index\n";
	PM_Tree my_pm_tree(M);
	vector<vector<double>> pivot_vec = Random_pivot(rawData, Pivot_Num, N);
	my_pm_tree.Set_Pivot(Pivot_Num, Pivot_Num, Pivot_Num, pivot_vec);
	for (int i = 0; i < rawData.size(); ++i) {
		my_pm_tree.Insert(rawData[i], i);		
	}
	M_Node_St* tmp_root = my_pm_tree.Get_Root();
	tmp_root->level = 0;
	my_pm_tree.Update_Level(tmp_root, 1);
	cout << "Coustruct Index Ok\n";
	
	/*计算体积*/
	double PM_Tree_Volumn = my_pm_tree.Traverse_Get_Volume();
	cout << "the volume of PM-Tree is " << PM_Tree_Volumn << endl;

	/*输出节点信息*/
	my_pm_tree.Traverse_Bread_Tree("experiment\\node_info.txt");

	/*搜索*/
	ofstream outfile("experiment\\PM_Tree_result.csv", ios::app);
	if (outfile) {
		cout << "Search range \n";
		for (int range_id = 0; range_id < range_buff.size(); ++range_id) {
			vector <vector<pair<double, int>>> mtree_res(queryData.size());
			double aver_result_num = 0;
			int all_pivot_filter_num = 0;
			int all_cal_distance_num = 0;
			int all_Via_subNode_Num = 0;
			int all_Via_Node_Num = 0;

			double query_time = 0;
			Timer time1;
			time1.restart();
			for (int i = 0; i < queryData.size(); ++i) {
				/*if (i % 10 == 0)
					cout << "the " << i << "th query is being queried\n";*/
				my_pm_tree.Range_Search(queryData[i], range_buff[range_id], mtree_res[i]);
				all_pivot_filter_num += my_pm_tree.Get_Pivot_Filter_Num();
				all_cal_distance_num += my_pm_tree.Get_Cal_Distance_Num();
				all_Via_subNode_Num += my_pm_tree.Get_Via_subNode_Num();
				all_Via_Node_Num += my_pm_tree.Get_Via_Node_Num();
				aver_result_num += mtree_res[i].size();
			}
			query_time = time1.elapsed();
			cout << "Search range Ok\n";
			vector<double> outData;
			outData.push_back(range_buff[range_id]);
			outData.push_back(10);
			outData.push_back(query_time / queryData.size());
			outData.push_back(double(all_cal_distance_num) / rawData.size() / queryData.size());
			outData.push_back(double(all_cal_distance_num) / queryData.size());
			outData.push_back(aver_result_num / queryData.size() / rawData.size());
			outData.push_back(PM_Tree_Volumn);
			outData.push_back(all_pivot_filter_num / queryData.size());			
			outData.push_back(all_Via_Node_Num / queryData.size());
			outData.push_back(all_Via_subNode_Num / queryData.size());
			outData.push_back(M);
			outData.push_back(Pivot_Num);
			Printf_Result_Csv(outfile, outData);

			//for (int i = 0; i < 10; i++) {
			//	outfile << time_res[i] << endl;
			//	time_res[i] = 0;
			//}			
			//outfile << endl; 			
		}
	}
	else {
		cout << "error in ofstream outfile(experimnet\\result.txt) \n";
	}
}

void Printf_Result_Csv(ofstream& out, vector<double> outData)
{
//	out << "range is " << "," << "query_time range_" << "," << "cost" << "," << "distances_computed" << "," << "aver_result_num_" << endl;
	for (int i = 0; i < outData.size(); ++i) {
		out << outData[i] << ",";
	}
	out << endl;
	
}

void Printf_Result(ofstream& out,double query_time_,double cost_, double all_cal_distance_num_,double range_,double aver_result_num_,double all_pivot_filter_num_)
{
	out << "range is " << range_ << endl;
	out << "query_time is " << query_time_ << endl;
	out << "cost is " << cost_ << endl;
	out << "distances computed is " << all_cal_distance_num_ << endl;
	out << "all_pivot_filter_num_  is " << all_pivot_filter_num_ << endl;
	out << "aver_result_num_  is " << aver_result_num_ << endl;
}

void Random_Test()
{
	int KNN = 100;
	int Pivot_Num =0;
	int N = 10;//重复选取Pivot次数
	int all_pivot_filter_num = 0;
	int all_cal_distance_num = 0;
	RandomData rawData(2, 7);
	RandomData queryData(1, 7);
	cout << "Coustruct Index\n";
	PM_Tree my_pm_tree(5);
	vector<vector<double>> pivot_vec = Random_pivot(rawData, Pivot_Num, N);
	my_pm_tree.Set_Pivot(Pivot_Num, Pivot_Num, Pivot_Num, pivot_vec);
	for (int i = 0; i < rawData.size(); ++i) {
		my_pm_tree.Insert(rawData[i], i);
	}
	cout << "Coustruct Index Ok\n";
	/*计算体积*/
	cout << "the volume of PM-Tree is " << my_pm_tree.Traverse_Get_Volume() << endl;

	cout << "Calulate knn \n";
	/*计算真实距离*/
	vector < vector<pair<double, int>>> real_res(queryData.size());
	for (int i = 0; i < queryData.size(); ++i) {
		for (int j = 0; j < rawData.size(); ++j) {
			double dist = MyFunc::Cal_Euclidean_distance(rawData[j], queryData[i]);
			real_res[i].push_back(make_pair(dist, j));
		}
		sort(real_res[i].begin(), real_res[i].end());
		if (real_res[i].size() > KNN)
			real_res[i].resize(KNN);
	}
	cout << "Calulate knn Ok\n";

	cout << "Search range \n";
	/*搜索*/
	vector <vector<pair<double, int>>> mtree_res(queryData.size());
	for (int i = 0; i < queryData.size(); ++i) {
		if (i % 10 == 0)
			cout << "the " << i << "th query is being queried\n";
		my_pm_tree.Range_Search(queryData[i], real_res[i].back().first, mtree_res[i]);
		all_pivot_filter_num += my_pm_tree.Get_Pivot_Filter_Num();
		all_cal_distance_num += my_pm_tree.Get_Cal_Distance_Num();
	}
	cout << "Search range Ok\n";
	/*效果检测*/
	double recall = 0;
	for (int i = 0; i < real_res.size(); ++i) {
		for (int j = 0; j < real_res[i].size(); ++j) {
			for (int k = 0; k < mtree_res[i].size(); ++k) {
				if (real_res[i][j].second == mtree_res[i][k].second)
					recall++;
			}
		}
	}
	cout << "Recall is " << recall / real_res.size() / KNN << endl;
	cout << "Pivot_Filter_Num is" << all_pivot_filter_num / queryData.size() << endl;
	cout << "Cal_Distance_Num is" << all_cal_distance_num / queryData.size() << endl;

}

vector<vector<double>> Random_pivot(RandomData& raw_data_,int pivot_num_,int N_) 
{
	default_random_engine rng(0);
	uniform_int_distribution<int> u_random_int(0, raw_data_.size() - 1);
	vector<vector<vector<double>>> N_pivot_label(N_);
	for (int i = 0; i < N_; ++i) {
		set<int> repeat_flag_set;
		vector<vector<double>> pivot_label(pivot_num_);
		for (int j = 0; j < pivot_num_; ++j) {
			int label=u_random_int(rng);
			if (repeat_flag_set.count(label) == 0)
			{		
				pivot_label[j]=raw_data_[label];
				repeat_flag_set.insert(label);
			}
			else
				--j;
		}
		N_pivot_label.push_back(pivot_label);
	}

	/*maximum distantce*/
	pair<double, int> res_label(INT16_MIN,-1);
	for (int i = 0; i < N_pivot_label.size(); ++i) {
		double dist = distance_Between_Piovt_Vector(N_pivot_label[i]);
		if (dist > res_label.first)
			res_label.second = i;
	}
	return N_pivot_label[res_label.second];
}

vector<vector<double>> Random_pivot(DataMetric& raw_data_, int pivot_num_, int N_)
{
	default_random_engine rng(0);
	uniform_int_distribution<int> u_random_int(0, raw_data_.size() - 1);
	vector<vector<vector<double>>> N_pivot_label(N_);
	for (int i = 0; i < N_; ++i) {
		set<int> repeat_flag_set;
		vector<vector<double>> pivot_label(pivot_num_);
		for (int j = 0; j < pivot_num_; ++j) {
			int label = u_random_int(rng);
			if (repeat_flag_set.count(label) == 0)
			{
				pivot_label[j] = raw_data_[label];
				repeat_flag_set.insert(label);
			}
			else
				--j;
		}
		N_pivot_label.push_back(pivot_label);
	}

	/*maximum distantce*/
	pair<double, int> res_label(INT16_MIN, -1);
	for (int i = 0; i < N_pivot_label.size(); ++i) {
		double dist = distance_Between_Piovt_Vector(N_pivot_label[i]);
		if (dist > res_label.first)
			res_label.second = i;
	}
	return N_pivot_label[res_label.second];
}

double distance_Between_Piovt_Vector(vector<vector<double>>& pivot_vector_)
{
	double all_dist = 0;
	for (int i = 0; i < pivot_vector_.size(); ++i) {
		for (int j = i + 1; j < pivot_vector_.size(); ++j) {
			all_dist += MyFunc::Cal_Euclidean_distance(pivot_vector_[i], pivot_vector_[j]);
		}
	}
	return all_dist;
}