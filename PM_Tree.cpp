#include "pch.h"
#include "PM_Tree.h"
#include"MyFunc.h"
#include <stdint.h>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<random>
#include<queue>
#include<time.h>
#include"Timer.h"


Timer time_buff[10];
double time_res[10] = {0};

using namespace std;


/*给定*/
void PM_Tree::Split(M_Node_St ** cur_node_ptr_address_, M_Node_St ** insert_node_address_)
{
	M_Node_St * cur_node_ = *cur_node_ptr_address_;
	M_Node_St * insert_node_ = *insert_node_address_;
	/*spilt two new nodes */
	M_Node_St* new_Mnode_1 = new M_Node_St(nullptr, 0, -1, -1);
	M_Node_St* new_Mnode_2 = new M_Node_St(nullptr, 0, -1, -1);

	vector<M_Node_St*>  entries = cur_node_->ptr_sub_tree;
	entries.push_back(insert_node_);
	
	/*decide featuer value of node_1 and node_2  basing on entries*/
	Promote(entries, new_Mnode_1, new_Mnode_2);
	
	/*divide ertries into node_1 and node_2  */
	Partition(entries, new_Mnode_1, new_Mnode_2);

	if (Is_Root_Node(cur_node_)) {
		M_Node_St* new_root = new M_Node_St(nullptr,0,-1,-1);
		new_root->feature_val = new_Mnode_1->feature_val;/*update root feature value*/

		/*update information*/
		new_Mnode_1->patent_node = new_root;
		new_Mnode_1->dist_to_parent = Cal_Dist_To_Parent(new_Mnode_1);
		new_Mnode_2->patent_node = new_root;
		new_Mnode_2->dist_to_parent = Cal_Dist_To_Parent(new_Mnode_2);
		Merge_subNode_HR(new_Mnode_1);
		Merge_subNode_HR(new_Mnode_2);


		new_root->ptr_sub_tree.push_back(new_Mnode_1);
		new_root->ptr_sub_tree.push_back(new_Mnode_2);
		new_root->range = Cal_Cover_Radius(new_root);
		Merge_subNode_HR(new_root);
		root = new_root;
	}
	else {
		/*update information*/
		new_Mnode_1->patent_node = cur_node_->patent_node;
		new_Mnode_1->dist_to_parent = Cal_Dist_To_Parent(new_Mnode_1);
		new_Mnode_2->patent_node = cur_node_->patent_node;
		new_Mnode_2->dist_to_parent = Cal_Dist_To_Parent(new_Mnode_2);
		/*将new_Mnode_1中元素替换cur_node_元素，并更新父节点*/
		assign_Node_All_Value(cur_node_, new_Mnode_1);
		Merge_subNode_HR(cur_node_);
		Merge_subNode_HR(new_Mnode_2);
		/*delete memory*/
		delete(new_Mnode_1);

		if (Is_Full(new_Mnode_2->patent_node)) {
			Split(&(new_Mnode_2->patent_node), &new_Mnode_2);
		}
		else {		
			new_Mnode_2->patent_node->ptr_sub_tree.push_back(new_Mnode_2);
		}
	}
}

void PM_Tree::Insert(vector<double> feature_val_, int id_)
{
	M_Node_St* new_node = new M_Node_St(nullptr, 2, -1, id_);
	new_node->feature_val = feature_val_;
	new_node->range = -1;
	Update_pivot_distance(new_node->pivot_distance, feature_val_);

	if (root == nullptr) {	//empty tree
		M_Node_St* new_root = new M_Node_St(nullptr, 1, -1, -1);		//空树第一个根节点设为第一个数据特征值
		new_root->feature_val = feature_val_;
		new_root->ptr_sub_tree.push_back(new_node);
		new_root->range = Cal_Cover_Radius(new_root);
		Update_hyper_rings(new_root->hyper_rings,feature_val_);
		root = new_root;									//update root node
	
		new_node->patent_node = new_root;
		new_node->dist_to_parent = Cal_Dist_To_Parent(new_node);
	}
	else {
		Insert(&root, &new_node);
	}
}

void PM_Tree::Range_Search(vector<double>& q_feature_val_, double search_range_, vector<pair<double, int>>& res_vec_)
{
	this->Via_Node_Num = 0;
	this->Via_subNode_Num = 0;
	this->Pivot_Filter_Num = 0;
	this->Cal_Distance_Num = 0;

	/*produce dist_query_pivot*/
	vector<double> dist_q_pivot(max(PD, HR));
	for (int i = 0; i < pivot_vec.size(); ++i) {
		dist_q_pivot[i] = MyFunc::Cal_Euclidean_distance(q_feature_val_, pivot_vec[i]);
	}
	double dist = MyFunc::Cal_Euclidean_distance(root->feature_val, q_feature_val_);
	this->Via_subNode_Num++;
	this->Via_Node_Num++;
	subRange_Search(&(this->root), q_feature_val_, search_range_, res_vec_,dist, dist_q_pivot);
	sort(res_vec_.begin(), res_vec_.end());

}

//void PM_Tree::subRange_Search(M_Node_St ** cur_node_address_,vector<double>& q_feature_val_, double search_range_, vector<pair<double, int>>& res_vec_,double &dist_parent_q_, vector<double>& dist_q_pivot_)
//{
//	M_Node_St* cur_node_ = *cur_node_address_;
//	if (Is_leaf_Node(cur_node_)) {
//		for (int i = 0; i< cur_node_->ptr_sub_tree.size(); ++i) {			
//			M_Node_St* data_entry = cur_node_->ptr_sub_tree[i];
//			double dist_parent_data = data_entry->dist_to_parent;
//			/*添加范围内的数据*/
//			if (fabs(dist_parent_q_ - dist_parent_data) <= search_range_ && Is_Pivot_Filter(dist_q_pivot_, search_range_, data_entry)) {
//				double dist = MyFunc::Cal_Euclidean_distance(data_entry->feature_val, q_feature_val_);
//				this->Cal_Distance_Num++;
//				if (dist <= search_range_)
//					res_vec_.push_back(make_pair(dist, data_entry->id));
//			}
//		}	
//	}
//	else {	
//		for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
//			M_Node_St* routing_entry = cur_node_->ptr_sub_tree[i];
//			double dist_parent_routing = routing_entry->dist_to_parent;
//			/*搜索下一层*/
//			if (fabs(dist_parent_q_ - dist_parent_routing) <= search_range_ + routing_entry->range) {
//				double dist = MyFunc::Cal_Euclidean_distance(routing_entry->feature_val, q_feature_val_);
//				if (dist <= search_range_ + routing_entry->range&&Is_Pivot_Filter(dist_q_pivot_, search_range_, routing_entry))
//					subRange_Search(&(cur_node_->ptr_sub_tree[i]), q_feature_val_, search_range_, res_vec_,dist, dist_q_pivot_);
//			}			
//		}
//		
//	}
//}

void PM_Tree::subRange_Search(M_Node_St ** cur_node_address_, vector<double>& q_feature_val_, double search_range_, vector<pair<double, int>>& res_vec_, double &dist_parent_q_, vector<double>& dist_q_pivot_)
{
	double dist = 0;
	M_Node_St* cur_node_ = *cur_node_address_;
	this->Via_Node_Num++;
	if (Is_leaf_Node(cur_node_)) {
		for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {			
			/*添加范围内的数据*/
			if (fabs(dist_parent_q_ - cur_node_->ptr_sub_tree[i]->dist_to_parent) <= search_range_ && Is_Pivot_Filter(dist_q_pivot_, search_range_, cur_node_->ptr_sub_tree[i])) {
				this->Via_subNode_Num++;
				dist = MyFunc::Cal_Euclidean_distance(cur_node_->ptr_sub_tree[i]->feature_val, q_feature_val_);
				this->Cal_Distance_Num++;

				if (dist <= search_range_)
					res_vec_.push_back(make_pair(dist, cur_node_->ptr_sub_tree[i]->id));
			}
		}
			
	}
	else {
		for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
			/*搜索下一层*/
			if (fabs(dist_parent_q_ - cur_node_->ptr_sub_tree[i]->dist_to_parent) <= search_range_ + cur_node_->ptr_sub_tree[i]->range) 
			{				
				this->Via_subNode_Num++;
				dist = MyFunc::Cal_Euclidean_distance(cur_node_->ptr_sub_tree[i]->feature_val, q_feature_val_);
				if (dist <= search_range_ + cur_node_->ptr_sub_tree[i]->range&&Is_Pivot_Filter(dist_q_pivot_, search_range_, cur_node_->ptr_sub_tree[i]))
				{		
					subRange_Search(&(cur_node_->ptr_sub_tree[i]), q_feature_val_, search_range_, res_vec_, dist, dist_q_pivot_);
				}
			}
		}

	}
}

void PM_Tree::Set_Pivot(int pivot_num_, int HR_, int PD_, const vector<vector<double>>& pivot_vec_)
{
	this->Piovt_Num = pivot_num_;
	this->HR = HR_;
	this->PD = PD_;
	this->pivot_vec = pivot_vec_;
}

double PM_Tree::Traverse_Get_Volume()
{
	double volume =MyFunc::Cal_Hypersphere_Volume(this->root->range, this->root->feature_val.size())  ;
	Sub_Traverse_Get_Volumem(this->root, volume);
	return volume;
}
void PM_Tree::Update_Level(M_Node_St* cur_node_,int level_)
{
	if (level_ > this->Max_Level)
		this->Max_Level = level_;
	for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
		cur_node_->ptr_sub_tree[i]->level = level_;
		if(!(Is_Data_Node(cur_node_->ptr_sub_tree[i])))
			Update_Level(cur_node_->ptr_sub_tree[i], level_ + 1);
	}
}
void PM_Tree::Traverse_Bread_Tree(string out_file_path_)
{
	ofstream outfile(out_file_path_);
	queue< M_Node_St *> my_queue;
	my_queue.push(root);
	int cur_level = 0;
	if (outfile) {
		while (!my_queue.empty()) {
			M_Node_St * cur_node_ = my_queue.front();
			my_queue.pop();
			if (cur_node_->level - cur_level >= 2) 
				cout << "error in Traverse_Bread_Tree\n";
			if (cur_node_->level != cur_level) {
				cur_level = cur_node_->level;
			}
			Output_Node_Info(outfile,cur_node_);
			for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i)
				my_queue.push(cur_node_->ptr_sub_tree[i]);
		}
	}
	else {
		cout << "error in Traverse_Bread_Tree\n";
	}
}
void PM_Tree::Output_Node_Info(ofstream & out_, const M_Node_St * cur_node_)
{
	/*输出层号*/
	out_ << cur_node_->level<<"  ";

	///*输出特征值*/
	//out_ << cur_node_->feature_val.size() << " ";
	//for (int i = 0; i < cur_node_->feature_val.size(); ++i) {
	//	out_ << cur_node_->feature_val[i] << " ";
	//}

	if (this->Piovt_Num == 0) {
		if(Is_Data_Node(cur_node_))
			out_ << 0 << " " << 1 << "     ";
		else
			out_ << cur_node_->range << " " << cur_node_->ptr_sub_tree.size() << "     ";
	}
		
	else {
		if (Is_Data_Node(cur_node_)) {
			out_ << 0 << " " << 1 << "     ";
			for (int i = 0; i < cur_node_->pivot_distance.size(); ++i)
			{
				out_ << cur_node_->pivot_distance[i] << "  " << cur_node_->pivot_distance[i] << "  ";

			}
		}
		else {
			out_ << cur_node_->range << " " << cur_node_->ptr_sub_tree.size() << "     ";
			for (int i = 0; i < cur_node_->hyper_rings.size(); ++i)
			{
				out_ << cur_node_->hyper_rings[i].first << "  " << cur_node_->hyper_rings[i].second << "  ";

			}
		}	
	}
	out_ << endl;
		
}
void PM_Tree::Sub_Traverse_Get_Volumem(M_Node_St* cur_node_,double& volume_)
{
	if (Is_leaf_Node(cur_node_)) 
		return;
	for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
		double v = MyFunc::Cal_Hypersphere_Volume(cur_node_->ptr_sub_tree[i]->range, cur_node_->ptr_sub_tree[i]->feature_val.size());
		volume_ += v;
		Sub_Traverse_Get_Volumem(cur_node_->ptr_sub_tree[i], volume_);
	}	
}
void PM_Tree::Update_hyper_rings(vector<pair<double, double>>& hyper_rings_, vector<double>& data_feature_val_)
{
	if (hyper_rings_.size() == 0)
		hyper_rings_.resize(HR, make_pair(INT16_MAX, INT16_MIN));
	for (int i = 0; i <(this->HR); ++i) {
		double dist=MyFunc::Cal_Euclidean_distance(data_feature_val_, this->pivot_vec[i]);
		if (dist < hyper_rings_[i].first)hyper_rings_[i].first = dist;
		if (dist > hyper_rings_[i].second)hyper_rings_[i].second = dist;
	}
}

void PM_Tree::Update_pivot_distance(vector<double>& pivot_distance_, vector<double>& data_feature_val_)
{
	if (pivot_distance_.size() == 0)
		pivot_distance_.resize(PD, 0);
	for (int i = 0; i < (this->PD); ++i) {
		double dist = MyFunc::Cal_Euclidean_distance(data_feature_val_, this->pivot_vec[i]);
		pivot_distance_[i] = dist;
	}
}

inline bool PM_Tree::Is_Pivot_Filter(vector<double>& dist_q_pivot_, double range_, M_Node_St * cur_node_)
{	
	if (Is_Data_Node(cur_node_)) {
		double PD_i = 0;
		for (int i = 0; i < PD; ++i) {
			PD_i = cur_node_->pivot_distance[i];
			if (fabs(dist_q_pivot_[i] - PD_i) > range_) {
				Pivot_Filter_Num++;
				return false;
			}
				
		}
	}
	else {
		for (int i = 0; i < HR; ++i) {
			if ((dist_q_pivot_[i] - range_ > cur_node_->hyper_rings[i].second) || (dist_q_pivot_[i] + range_) < cur_node_->hyper_rings[i].first) {
				Pivot_Filter_Num++;
				return false;
			}
		}
	}
	return true;
	
}



void PM_Tree::Insert(M_Node_St ** cur_node_address_, M_Node_St ** insert_node_address_)
{
	M_Node_St *cur_node_ = *cur_node_address_;
	M_Node_St * insert_node_ = *insert_node_address_;
	if (Is_leaf_Node(cur_node_)) {
		if (Is_Full(cur_node_)) {
			Split(&cur_node_, &insert_node_);
		}
		else {
			cur_node_->range = Cal_Cover_Radius(cur_node_, insert_node_);
			cur_node_->ptr_sub_tree.push_back(insert_node_);	

			insert_node_->patent_node = cur_node_;
			insert_node_->dist_to_parent = Cal_Dist_To_Parent(insert_node_);
			Update_hyper_rings(cur_node_->hyper_rings, insert_node_->feature_val);
		}
	}
	else {
		M_Node_St** next_node=Get_Next_Node_And_Update_Range(&cur_node_, &insert_node_);
		Update_hyper_rings((*next_node)->hyper_rings, insert_node_->feature_val);
		Insert(next_node, &insert_node_);
	}
}

/*repalce all attributtion of cur_node_ with new_node_'s*/
void PM_Tree::assign_Node_All_Value(M_Node_St * cur_node_, M_Node_St * new_node_)
{
	cur_node_->dist_to_parent = new_node_->dist_to_parent;
	cur_node_->feature_val = new_node_->feature_val;
	cur_node_->id = new_node_->id;
	cur_node_->node_category = new_node_->node_category;
	cur_node_->patent_node = new_node_->patent_node;
	cur_node_->ptr_sub_tree = new_node_->ptr_sub_tree;
	cur_node_->range = new_node_->range;
	cur_node_->pivot_distance = new_node_->pivot_distance;
	cur_node_->hyper_rings = cur_node_->hyper_rings;
	for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
		cur_node_->ptr_sub_tree[i]->patent_node = cur_node_;
	}
}
/*traverse subnode of cur_node_ and decide the hyper_rings ofcur_node_*/
void PM_Tree::Merge_subNode_HR(M_Node_St * cur_node_)
{
	if (Is_leaf_Node(cur_node_)) {
		for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
			Update_hyper_rings(cur_node_->hyper_rings, cur_node_->ptr_sub_tree[i]->feature_val);
		}
	}
	else {
		if (cur_node_->hyper_rings.size() == 0)
			cur_node_->hyper_rings.resize(HR, make_pair(INT16_MAX, INT16_MIN));
		for (int i = 0; i < HR; ++i) {
			for (int j = 0; j < cur_node_->ptr_sub_tree.size(); ++j) {
				double sub_min;
				double sub_max;
				if (cur_node_->ptr_sub_tree[j]->ptr_sub_tree.size() != 0)
				{
					sub_min = cur_node_->ptr_sub_tree[j]->hyper_rings[i].first;
					sub_max = cur_node_->ptr_sub_tree[j]->hyper_rings[i].second;
				}
				else
					continue;
				double cur_min = cur_node_->hyper_rings[i].first;
				double cur_max = cur_node_->hyper_rings[i].second;
				if (cur_min > sub_min)
					cur_node_->hyper_rings[i].first = cur_node_->ptr_sub_tree[j]->hyper_rings[i].first;
				if (cur_max < sub_max)
					cur_node_->hyper_rings[i].second = cur_node_->ptr_sub_tree[j]->hyper_rings[i].second;
			}
		}
	}
}

M_Node_St ** PM_Tree::Get_Next_Node_And_Update_Range(M_Node_St ** cur_node_address_, M_Node_St ** insert_node_address_)
{
	M_Node_St * cur_node_ = *cur_node_address_;
	M_Node_St * insert_node_ = *insert_node_address_;
	vector<pair<double, M_Node_St**>> data_vec;
	vector<pair<double, M_Node_St**>> data2_vec;
	for (int i = 0; i < cur_node_->ptr_sub_tree.size(); ++i) {
		/*minmum d(O_r,O_ins)<= r(O_r)*/
		double dist = MyFunc::Cal_Euclidean_distance(cur_node_->ptr_sub_tree[i]->feature_val, insert_node_->feature_val);
		if (dist <= cur_node_->ptr_sub_tree[i]->range) {
			data_vec.push_back(make_pair(dist, &(cur_node_->ptr_sub_tree[i])));
		}
		/*minmum d(O_r,O_ins)- r(O_r)*/
		if (data_vec.size() == 0) {
			dist -= cur_node_->ptr_sub_tree[i]->range;
			data2_vec.push_back(make_pair(dist, &(cur_node_->ptr_sub_tree[i])));
		}
	}

	if (data_vec.size() != 0) {
		sort(data_vec.begin(), data_vec.end());
		return data_vec[0].second;
	}
	sort(data2_vec.begin(), data2_vec.end());
	(*(data2_vec[0].second))->range = data2_vec[0].first+ (*(data2_vec[0].second))->range;//update range
	return data2_vec[0].second;
}


void PM_Tree::Promote(vector<M_Node_St*>& entries_, M_Node_St * node_1_, M_Node_St * node_2_)
{
	switch (promotefunc_e) {
		case RANDOM_e: {
			Promote_Random(entries_, node_1_, node_2_);
		}break;
		case m_RAD_2: {
			Promote_M_RAD_2(entries_, node_1_, node_2_);
		}break;
		default: {

		}
	}

}

/*
	randomly choose two feature value from entries,and let them act as the feature value of node_1 and node_2
	only change the feature information of node_1 and node_2
	two feature value are different
*/
void PM_Tree::Promote_Random(vector<M_Node_St*>& entries_, M_Node_St * node_1_, M_Node_St * node_2_)
{
	default_random_engine generator;
	uniform_int_distribution<int> u_random_int(0, entries_.size() - 1);
	int a=u_random_int(generator);
	int b = -1;
	do {
		b= u_random_int(generator);
	} while (a == b);
	node_1_->feature_val = entries_[a]->feature_val;
	node_2_->feature_val = entries_[b]->feature_val;
}

void PM_Tree::Promote_M_RAD_2(vector<M_Node_St*>& entries_, M_Node_St * node_1_, M_Node_St * node_2_)
{
	double minimum = INT32_MAX;
	int a=-1,b=-1;
	for (int i = 0; i < entries_.size(); ++i) {
		for (int j = i+1; j < entries_.size(); ++j) {
			double range = minimum_Rad(entries_, i, j);
			if (range < minimum) {
				a = i;
				b = j;
				minimum = range;
			}
		}
	}
	node_1_->feature_val = entries_[a]->feature_val;
	node_2_->feature_val = entries_[b]->feature_val;
}

double PM_Tree::minimum_Rad(vector<M_Node_St*>& entries_, int pos1_, int pos2_)
{
	M_Node_St* p_1 = new M_Node_St();
	M_Node_St* p_2 = new M_Node_St();
	p_1->node_category = entries_[0]->node_category;
	p_2->node_category = entries_[0]->node_category;
	p_1->feature_val = entries_[pos1_]->feature_val;
	p_2->feature_val = entries_[pos2_]->feature_val;

	for (int i = 0; i < entries_.size(); ++i) {
		if (i == pos1_ || i == pos2_) 
			continue;

		if (MyFunc::Cal_Euclidean_distance(entries_[i]->feature_val, p_1->feature_val)
			<= MyFunc::Cal_Euclidean_distance(entries_[i]->feature_val, p_2->feature_val))
		{
			p_1->ptr_sub_tree.push_back(entries_[i]);
		}
		else {
			p_2->ptr_sub_tree.push_back(entries_[i]);
		}
	}
	double res_range = 0;
	res_range +=Cal_Cover_Radius(p_1);
	res_range += Cal_Cover_Radius(p_2);
	delete(p_1);
	delete(p_2);
	return res_range;
}

/*
	Dividing entries into node_1 and node_2
	changing all relevant information
*/
void PM_Tree::Partition(vector<M_Node_St*>& entries_, M_Node_St * node_1_, M_Node_St * node_2_)
{
	if (entries_[0]->node_category == 1)
		node_1_->node_category = node_2_->node_category = 0;
	else if (entries_[0]->node_category == 2)
		node_1_->node_category = node_2_->node_category = 1;
	else if (entries_[0]->node_category == 0)
		node_1_->node_category = node_2_->node_category = 0;
	else
		cout << "error in Partition\n";

	for (int i = 0; i < entries_.size(); ++i) {
		if (MyFunc::Cal_Euclidean_distance(entries_[i]->feature_val, node_1_->feature_val)
			<= MyFunc::Cal_Euclidean_distance(entries_[i]->feature_val, node_2_->feature_val))
		{
			node_1_->ptr_sub_tree.push_back(entries_[i]);
			entries_[i]->patent_node = node_1_;

		}
		else {
			node_2_->ptr_sub_tree.push_back(entries_[i]);
			entries_[i]->patent_node = node_2_;
		}
		entries_[i]->dist_to_parent = Cal_Dist_To_Parent(entries_[i]);/*update distance*/
	}
	node_1_->range = Cal_Cover_Radius(node_1_);
	node_2_->range = Cal_Cover_Radius(node_2_);


}

double PM_Tree::Cal_Cover_Radius(const M_Node_St * node_)
{
	if (node_->ptr_sub_tree.size() == 0)
		return 0;
	double max_range = INT16_MIN;
	if (Is_leaf_Node(node_)) {
		for (int i = 0; i < node_->ptr_sub_tree.size(); ++i) {
			double dist = MyFunc::Cal_Euclidean_distance(node_->ptr_sub_tree[i]->feature_val, node_->feature_val);
			if (dist > max_range) max_range = dist;
		}	
	}
	else {
		for (int i = 0; i < node_->ptr_sub_tree.size(); ++i) {
			double dist = MyFunc::Cal_Euclidean_distance(node_->ptr_sub_tree[i]->feature_val, node_->feature_val);
			dist += node_->ptr_sub_tree[i]->range;
			if (dist > max_range) max_range = dist;
		}
	}
	return max_range;
}

double PM_Tree::Cal_Cover_Radius(const M_Node_St * node_, const M_Node_St * cmp_node_)
{
	double dist;
	if (Is_leaf_Node(node_)) {	
		dist = MyFunc::Cal_Euclidean_distance(cmp_node_->feature_val, node_->feature_val);
	}
	else {
		dist = MyFunc::Cal_Euclidean_distance(cmp_node_->feature_val, node_->feature_val);
		dist += cmp_node_->range;
	}	
	if (node_->range < dist)
		return dist;
	else
		return node_->range;
}

double PM_Tree::Cal_Dist_To_Parent(const M_Node_St * node_)
{
	double dist;
	if(!(node_==NULL || node_->patent_node==NULL))
		dist=MyFunc::Cal_Euclidean_distance(node_->feature_val, node_->patent_node->feature_val);
	else {
		cout << "error In Cal_Dist_To_Parent \n";
			return -1;
	}
	return dist;
}

