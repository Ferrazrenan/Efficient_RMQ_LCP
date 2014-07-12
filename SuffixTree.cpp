#include "stdafx.h"

using namespace std;

class node{
	unordered_map<char, node*> childs;
public:
	int inicial_index;
	int s_common_size;
	

	node(int inicial_index, int s_common_size){
		this->inicial_index = inicial_index;
		this->s_common_size = s_common_size;
	}

	void add_child(char _give, node* child){
		if (childs.find(_give) != childs.end()) throw new exception();
		else childs[_give] = child;
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

};

class SuffixTree{
public:

	string s;
	node* root;

	node& add_suffix(node& current_node, int suffix_inicial_position){
		int common_size = 0;
		while (common_size < current_node.s_common_size && suffix_inicial_position + common_size < s.size() &&
			s[current_node.inicial_index + common_size] == s[suffix_inicial_position + common_size]) common_size++;

		if (common_size == current_node.s_common_size && suffix_inicial_position + common_size == s.size()){
			return current_node;
		}
		else if (common_size == current_node.s_common_size){ //node is over berfore missmatch
			char next_suffix_char = s[suffix_inicial_position + common_size];
			if (current_node.has_child(next_suffix_char)){
				node& node_child = add_suffix(*current_node.get_child(next_suffix_char), suffix_inicial_position + common_size);
				current_node.replace_child(next_suffix_char, &node_child);
			}
			else current_node.add_child(next_suffix_char, new node(suffix_inicial_position + common_size, s.size() - (suffix_inicial_position + common_size)));
			return current_node;
		}
		else if (suffix_inicial_position + common_size == s.size()){ // suffix is over berfore missmatch
			char next_node_char = s[current_node.inicial_index + common_size];
			node& new_node_in_between = *new node(current_node.inicial_index, common_size);
			new_node_in_between.add_child(next_node_char, &current_node);
			return new_node_in_between;
		}
		else //missmatch
		{
			char next_suffix_char = s[suffix_inicial_position + common_size];
			char next_node_char = s[current_node.inicial_index + common_size];
			node& new_node_in_between = *new node(current_node.inicial_index, common_size);
			new_node_in_between.add_child(next_node_char, &current_node);
			new_node_in_between.add_child(next_suffix_char, new node(suffix_inicial_position + common_size, s.size() - (suffix_inicial_position + common_size)));
			return new_node_in_between;
		}
	}
	
	SuffixTree(string s){
		this->s = s;

		root = new node(0, s.size());		
			
		for (int i = 1; i < s.size(); i++){
			root = &add_suffix(*root, i);
		}
	}
};

