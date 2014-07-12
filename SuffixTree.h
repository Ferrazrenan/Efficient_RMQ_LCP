#include "stdafx.h"
#include "IntervalTree.h"
using namespace std;

class node{
	unordered_map<char, node*> childs;

public:
	int inicial_index;
	int s_common_size;
	int num_suffix_passes_through;

	node(int inicial_index, int s_common_size){
		this->inicial_index = inicial_index;
		this->s_common_size = s_common_size;
		this->num_suffix_passes_through = 1;
	}

	void add_child(char _give, node* child){
		childs.insert(make_pair(_give, child));
	}

	bool has_child(char _give){
		return childs.find(_give) != childs.end();
	}

	node* get_child(char character){
		return childs[character];
	}

	void replace_child(char character, node* child){
		childs[character] = child;
	}

	void add_or_replace_child(char character, node* child){
		if (has_child(character)){
			replace_child(character, child);
		}
		else
			add_child(character, child);
	}

	unordered_map<char, node*>::iterator get_iterator() {
		return childs.begin();
	}

	unordered_map<char, node*>::iterator get_end(){
		return childs.end();
	}

};

class SuffixTree{
public:
	int n;
	string s;
	node* root;

	node& add_suffix(node& current_node, int suffix_inicial_position){
		int common_size = 0;
		while (common_size < current_node.s_common_size && suffix_inicial_position + common_size < s.size() &&
			s[current_node.inicial_index + common_size] == s[suffix_inicial_position + common_size]) common_size++;

		if (common_size == current_node.s_common_size && suffix_inicial_position + common_size == s.size()){
			current_node.num_suffix_passes_through++;
			return current_node;
		}
		else if (common_size == current_node.s_common_size){ //node is over berfore missmatch
			char next_suffix_char = s[suffix_inicial_position + common_size];
			if (current_node.has_child(next_suffix_char)){
				node& node_child = add_suffix(*current_node.get_child(next_suffix_char), suffix_inicial_position + common_size);
				current_node.replace_child(next_suffix_char, &node_child);
			}
			else current_node.add_child(next_suffix_char, new node(suffix_inicial_position + common_size, s.size() - (suffix_inicial_position + common_size)));

			current_node.num_suffix_passes_through++;

			return current_node;
		}
		else if (suffix_inicial_position + common_size == s.size()){ // suffix is over berfore missmatch
			char next_node_char = s[current_node.inicial_index + common_size];
			node& new_node_in_between = *new node(current_node.inicial_index, common_size);
			new_node_in_between.add_child(next_node_char, &current_node);
			new_node_in_between.num_suffix_passes_through += current_node.num_suffix_passes_through;
			current_node.s_common_size -= common_size;
			current_node.inicial_index += common_size;
			return new_node_in_between;
		}
		else //missmatch
		{
			char next_suffix_char = s[suffix_inicial_position + common_size];
			char next_node_char = s[current_node.inicial_index + common_size];
			node& new_node_in_between = *new node(current_node.inicial_index, common_size);
			new_node_in_between.add_child(next_node_char, &current_node);
			new_node_in_between.add_child(next_suffix_char, new node(suffix_inicial_position + common_size, s.size() - (suffix_inicial_position + common_size)));
			new_node_in_between.num_suffix_passes_through += current_node.num_suffix_passes_through;
			current_node.s_common_size -= common_size;
			current_node.inicial_index += common_size;
			return new_node_in_between;
		}
	}

	void tranverse(node* curr){
		cout << "/ index: " << curr->inicial_index << "/ size: " << curr->s_common_size << " / num_suffix: " << curr->num_suffix_passes_through << " childs : ";
		for (unordered_map<char, node*>::iterator it = curr->get_iterator(); it != curr->get_end(); it++)
			cout << it->first << " ";
		cout << endl;

		for (unordered_map<char, node*>::iterator it = curr->get_iterator(); it != curr->get_end(); it++)
			tranverse(it->second);

		cout << "up" << endl;
	}

	SuffixTree(string &s){
		this->s = s;

		root = new node(0, s.size());

		for (int i = 1; i < s.size(); i++){
			root = &add_suffix(*root, i);
		}

		//tranverse(root);
	}

	SuffixTree(string &s, int* SA, int* LCP){
		this->s = s;
		this->SA = SA;
		this->LCP = LCP;
		this->n = s.size();

		buildTreeFromSAandLCP();
	}

	int first_occurence(string &q){
		if (q.size() == 0) return -1;
		return query(root, q, 0);
	}
	
	void preprocessMostFrequentlyString(){
		intervalTree = new IntervalTree(n);
		fillNumberOfChilds(root,0);
		//fillEventPoints(root, 0);
		//fillMostFrequentSubstrings();
	}

	int MostCommonSubstringSize(int k){
		if (k < 0) return -1;
		else if (k>n) return n;
		else return intervalTree->get_max_value_key(k);// most_common_string_with_size.at(k);
	}
private: 
	int* SA, *LCP;
	unordered_map<long long, int> number_of_subtree_nodes;

	void add_or_replace_in_parent_a_child_node(node* parent, node* child){
		char divegence_char = (child->inicial_index == n) ? '$' : s[child->inicial_index];
		parent->add_or_replace_child(divegence_char, child);
	}

	void buildTreeFromSAandLCP(){

		int sum_of_ancestors_size = 0;
		this->root = new node(-1, 0);
		node* curr_node = this->root;
		std::stack<node*> parents;
		//char temp[80000];

		for (int sa_index = 0; sa_index < n; sa_index++){
			int size_of_char_that_match_previous_inserted_suffix = sa_index ? this->LCP[sa_index - 1] : 0; // como já inserir o anterior, então tenho de ir a algum pai do anterior cuja meu lcp \in [depth, depth+size
			int suffix_index = this->SA[sa_index];
			int suffix_size = n - suffix_index;

			while (curr_node != root && size_of_char_that_match_previous_inserted_suffix <= sum_of_ancestors_size){
				curr_node = parents.top();
				parents.pop();
				sum_of_ancestors_size -= curr_node->s_common_size;
			}
			
			//for (int ii = 0; ii < size_of_char_that_match_previous_inserted_suffix; ii++) if (temp[ii] != s[suffix_index + ii]) 
			//	throw exception();

			if (sum_of_ancestors_size < size_of_char_that_match_previous_inserted_suffix
				&& size_of_char_that_match_previous_inserted_suffix < sum_of_ancestors_size + curr_node->s_common_size){
				int size_of_the_node = size_of_char_that_match_previous_inserted_suffix - sum_of_ancestors_size;

				node* new_node_in_between = new node(curr_node->inicial_index, size_of_the_node);

				curr_node->inicial_index += new_node_in_between->s_common_size;
				curr_node->s_common_size -= new_node_in_between->s_common_size;

				add_or_replace_in_parent_a_child_node(new_node_in_between, curr_node);
				add_or_replace_in_parent_a_child_node(parents.top(), new_node_in_between);

				int new_branch_node_ancestors = sum_of_ancestors_size + new_node_in_between->s_common_size;
				node* new_branch = new node(suffix_index + new_branch_node_ancestors, suffix_size - new_branch_node_ancestors);
				add_or_replace_in_parent_a_child_node(new_node_in_between, new_branch);

				sum_of_ancestors_size += new_node_in_between->s_common_size;
				parents.push(new_node_in_between);
				curr_node = new_branch;
				/*for (int ii = 0; ii < curr_node->s_common_size; ii++) temp[sum_of_ancestors_size + ii] = s[curr_node->inicial_index + ii];*/
			}
			else if (size_of_char_that_match_previous_inserted_suffix == sum_of_ancestors_size + curr_node->s_common_size){
				
				int new_node_incial_string_index = suffix_index + size_of_char_that_match_previous_inserted_suffix;
				
				node* new_child_node = new node(new_node_incial_string_index, suffix_size - size_of_char_that_match_previous_inserted_suffix);
				add_or_replace_in_parent_a_child_node(curr_node, new_child_node);

				sum_of_ancestors_size += curr_node->s_common_size;
				parents.push(curr_node);
				curr_node = new_child_node;
	/*			for (int ii = 0; ii < curr_node->s_common_size; ii++) temp[sum_of_ancestors_size + ii] = s[curr_node->inicial_index + ii];*/
			}
			else throw exception();
		}
 	}
	
	int query(node* curr_node, string &q, int q_curr_index){
		int common_size = 0;
		while (common_size < curr_node->s_common_size && common_size < q.size()
			&& s[curr_node->inicial_index + common_size] == q[q_curr_index + common_size]) common_size++;

		if (q_curr_index + common_size == q.size()) return curr_node->inicial_index + common_size;
		else if (common_size < curr_node->s_common_size ||
			!curr_node->has_child(q[q_curr_index + common_size])) return -(q_curr_index + common_size);
		else{
			return query(curr_node->get_child(q[q_curr_index + common_size]), q, q_curr_index + common_size);
		}
	}

	multimap<int, node*> nodes_start_size;
	map<int, int> most_common_string_with_size;
	IntervalTree* intervalTree;


	int fillNumberOfChilds(node* curr, int char_depth){
		int num_of_childs = 0;
		for (unordered_map<char, node*>::iterator it = curr->get_iterator(); it != curr->get_end(); it++) 
			num_of_childs += fillNumberOfChilds((*it).second, char_depth + curr->s_common_size);

		intervalTree->add_max_in_interval(char_depth + 1, char_depth + curr->s_common_size, curr->inicial_index - char_depth, num_of_childs + 1);
		//number_of_subtree_nodes.insert(make_pair((long long) (curr), num_of_childs + 1));
		return num_of_childs + 1;
	}

	void fillEventPoints(node* curr, int number_of_previous_chars){
		for (unordered_map<char, node*>::iterator it = curr->get_iterator(); it != curr->get_end(); it++) fillEventPoints((*it).second, number_of_previous_chars + curr->s_common_size);
		nodes_start_size.insert(make_pair(number_of_previous_chars + 1, curr));
	}

	void fillMostFrequentSubstrings(){
		priority_queue< pair<int, multimap<int, node*>::iterator*> > current_alive_nodes;
		int curr_word_size = 0;
		most_common_string_with_size.insert(make_pair(curr_word_size, -1));

		for (multimap<int, node*>::iterator it = nodes_start_size.begin(); it != nodes_start_size.end(); it++){
			curr_word_size++;

			while (it != nodes_start_size.end() && (*it).first == curr_word_size){
				current_alive_nodes.push(make_pair(number_of_subtree_nodes[(long long)(*it).second], &it));
				it++;
			}

			node* most_frequent_end_node = (*current_alive_nodes.top().second)->second;
			int num_of_char_to_get_inside_node = (*current_alive_nodes.top().second)->first;

			while (num_of_char_to_get_inside_node + most_frequent_end_node->s_common_size < curr_word_size){
				current_alive_nodes.pop();
				if (current_alive_nodes.empty()) 
					break;

				most_frequent_end_node = (*current_alive_nodes.top().second)->second;
				num_of_char_to_get_inside_node = (*current_alive_nodes.top().second)->first;
			}

			int string_inicial_index = most_frequent_end_node->inicial_index - (num_of_char_to_get_inside_node-1);
			most_common_string_with_size.insert(make_pair(curr_word_size, string_inicial_index));
		}
	}

};


class MatchAndCatch{

	string s1, s2;

	int lowestCommonUniqueSubstring(node* const current_node_st1, const int index_in_node1, node* const current_node_st2, int const index_in_node2, int const depth){
		int common_substring = 0;
		//cout << "in: " << current_node_st1->inicial_index + index_in_node1 << " " << current_node_st2->inicial_index + index_in_node2 << endl;
		if ((s1[current_node_st1->inicial_index + index_in_node1] == s2[current_node_st2->inicial_index + index_in_node2]) &&
			current_node_st1->num_suffix_passes_through == 1 && current_node_st2->num_suffix_passes_through == 1){
			//cout << "up " << depth + 1 << endl;
			return depth + 1;
		}

		while (common_substring + index_in_node1 < current_node_st1->s_common_size && common_substring + index_in_node2 < current_node_st2->s_common_size &&
			s1[current_node_st1->inicial_index + index_in_node1 + common_substring] == s2[current_node_st2->inicial_index + index_in_node2 + common_substring]){
			common_substring++;
		}

		if (common_substring + index_in_node1 == current_node_st1->s_common_size &&
			common_substring + index_in_node2 == current_node_st2->s_common_size){
			int resp = LONG_MAX;
			for (unordered_map<char, node*>::iterator it = current_node_st1->get_iterator(); it != current_node_st1->get_end(); it++){
				if (current_node_st2->has_child(it->first)){
					node* child_node1 = it->second;
					node* child_node2 = current_node_st2->get_child(it->first);

					resp = min(resp, lowestCommonUniqueSubstring(child_node1, 0, child_node2, 0, depth + common_substring));
				}
			}
			//cout << "up " << resp << endl;
			return resp;
		}
		else if (common_substring + index_in_node2 == current_node_st2->s_common_size){
			if (current_node_st2->has_child(s1[current_node_st1->inicial_index + index_in_node1 + common_substring])){
				return lowestCommonUniqueSubstring(current_node_st1, index_in_node1 + common_substring,
					current_node_st2->get_child(s1[current_node_st1->inicial_index + index_in_node1 + common_substring]), 0, depth + common_substring);
			}
			else{
				//cout << "up" << endl;
				return LONG_MAX;
			}
		}
		else if (common_substring + index_in_node1 == current_node_st1->s_common_size){
			if (current_node_st1->has_child(s2[current_node_st2->inicial_index + index_in_node2 + common_substring])){
				return lowestCommonUniqueSubstring(current_node_st1->get_child(s2[current_node_st2->inicial_index + index_in_node2 + common_substring]), 0,
					current_node_st2, index_in_node2 + common_substring, depth + common_substring);
			}
			else{
				//cout << "up" << endl;
				return LONG_MAX;
			}
		}
		else{
			//cout << "up" << endl;
			return LONG_MAX;
		}
	}

	int smallestCommonUniqueSubstring;

	MatchAndCatch(string s1, string s2){
		SuffixTree st1 = *new SuffixTree(s1);
		SuffixTree st2 = *new SuffixTree(s2);
		smallestCommonUniqueSubstring = lowestCommonUniqueSubstring(st1.root, 0, st2.root, 0, 0);
		delete &st1;
		delete &st2;
	}
};
