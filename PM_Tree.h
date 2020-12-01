#pragma once
#include<vector>
#include<set>
#include<iostream>
#include <stdint.h>


using namespace std;

enum PromoteFunc_E { RANDOM_e,m_RAD_2 };
struct M_Node_St
{
	/*common*/
	M_Node_St* patent_node;
	int node_category;  //0 represent routing node , 1:leaf node , 2: data entry
	vector<double> feature_val;
	double dist_to_parent;	//the distance between the node and parent node
	int level;//level of root is 0

	/*data node*/
	int id;
	vector<double> pivot_distance;

	/*routing or leaf node */
	double range;
	vector<M_Node_St*> ptr_sub_tree;
	vector<pair<double,double>> hyper_rings;//pair<min,max>

	M_Node_St() {		
	}
	M_Node_St(M_Node_St* patent_node_, int node_category_,
			double dist_to_parent_,int id_) {
		patent_node = patent_node_;
		node_category = node_category_;
		dist_to_parent = dist_to_parent_;
		id = id_;
	}
};



class PM_Tree
{
public:
	PM_Tree(int m_):M(m_){
		//promotefunc_e = m_RAD_2;
		promotefunc_e = RANDOM_e;
		root = nullptr;
		Max_Level=0;
	};
	~PM_Tree() {};
	
	void Insert(vector<double> feature_val_, int id_);
	void Range_Search(vector<double>& q_feature_val_,double search_range_,vector<pair<double, int>>& res_vec_);	
	void Set_Pivot(int pivot_num_,int HR_,int PD_, const vector<vector<double>>& pivot_vec_);
	int Get_Pivot_Filter_Num() { return Pivot_Filter_Num; }
	int Get_Cal_Distance_Num() { return Cal_Distance_Num; }
	int Get_Via_subNode_Num() { return Via_subNode_Num; }
	int Get_Via_Node_Num() { return Via_Node_Num; }
	double Traverse_Get_Volume();
	void Update_Level(M_Node_St * cur_node_, int level_);
	void Traverse_Bread_Tree(string out_file_path_);
	void Output_Node_Info(ofstream& out_,const M_Node_St* cur_node_);
	M_Node_St* Get_Root() { return root; }
	
	
private:
	void Sub_Traverse_Get_Volumem(M_Node_St * cur_node_, double & volume_);
	void Update_hyper_rings(vector<pair<double, double>>& hyper_rings_, vector<double>& data_feature_val_);
	void Update_pivot_distance(vector<double>& pivot_distance_, vector<double>& data_feature_val_);
	bool Is_Pivot_Filter(vector<double>& dist_q_pivot_, double range_, M_Node_St* cur_node_);
	void subRange_Search(M_Node_St ** cur_node_address_, vector<double>& q_feature_val_, double search_range_, vector<pair<double, int>>& res_vec_, double & dist_parent_q_, vector<double>& dist_q_pivot_);
	void Split(M_Node_St ** cur_node_ptr_address_, M_Node_St ** insert_node_address_);
	void Insert(M_Node_St ** cur_node_address_, M_Node_St ** insert_node_address_);
	void assign_Node_All_Value(M_Node_St* cur_node_, M_Node_St* new_node_);
	void Merge_subNode_HR(M_Node_St* cur_node_);
	M_Node_St ** Get_Next_Node_And_Update_Range(M_Node_St ** cur_node_address_, M_Node_St ** insert_node_address_);

	bool Is_Rounting_Node(const M_Node_St* node_) {
		return node_->node_category == 0;
	}
	bool Is_leaf_Node(const M_Node_St* node_) {
		return node_->node_category == 1;
	}
	bool Is_Root_Node(const M_Node_St* node_) {
		return node_->patent_node == nullptr;
	}
	bool Is_Data_Node(const M_Node_St* node_) {
		return node_->node_category == 2;
	}
	void Promote(vector<M_Node_St*>& entries_, M_Node_St* node_1_, M_Node_St* node_2_);
	void Promote_Random(vector<M_Node_St*>& entries_, M_Node_St* node_1_, M_Node_St* node_2_);
	void Promote_M_RAD_2(vector<M_Node_St*>& entries_, M_Node_St* node_1_, M_Node_St* node_2_);
	double minimum_Rad(vector<M_Node_St*>& entries_ , int pos1_, int pos2_);
	void Partition(vector<M_Node_St*>& entries_, M_Node_St* node_1_, M_Node_St* node_2_);
	double Cal_Cover_Radius(const M_Node_St* node_);
	double Cal_Cover_Radius(const M_Node_St * node_, const M_Node_St * cmp_node_);
	double Cal_Dist_To_Parent(const M_Node_St * node_);
	bool Is_Full(M_Node_St* node_) { return node_->ptr_sub_tree.size() >= M; }


	int M;
	int HR;
	int PD;
	int Piovt_Num;
	int Max_Level;
	vector<vector<double>> pivot_vec;
	M_Node_St* root;
	PromoteFunc_E promotefunc_e;
	int Pivot_Filter_Num=0;
	int Cal_Distance_Num=0;
	int Via_subNode_Num;
	int Via_Node_Num;

};