#include "stdafx.h"

class TreeNode{
public:
	TreeNode* left;
	TreeNode* right;
	TreeNode* parent;

	int value;
	int index;

	TreeNode(TreeNode* parent, TreeNode* left, TreeNode* right, int value, int index){
		this->parent = parent;
		this->left = left;
		this->right = right;
		this->value = value;
		this->index = index;
	}
	
};

class CartesianTree{

	void buildCartesianTree(){
		root = NULL;

		TreeNode* curr_node = NULL;
		for (int current_index = 0; current_index < n; current_index++){
			
			int new_value = values[current_index];
			while (curr_node != NULL && new_value < curr_node->value) curr_node = curr_node->parent;
			
			if (curr_node != NULL && new_value == curr_node->value)
				throw std::exception();
			
			TreeNode* new_node;
			if (curr_node == NULL){
				new_node = new TreeNode(NULL, root, NULL, new_value, current_index);
				root = new_node;
			}
			else {
				new_node = new TreeNode(curr_node, curr_node->right, NULL, new_value, current_index);
				curr_node->right = new_node;
			}

			if (new_node->left != NULL) new_node->left->parent = new_node;
			curr_node = new_node;
		}
		
	}

	void tranverseAndFillDirections(bool delete_nodes){
		TreeNode* curr_node = root;
		TreeNode* prev_node = NULL;
		const bool DIRECTION_UP = true;
		const bool DIRECTION_DOWN = false;
		bool CURRENT_DIRECTION;
		int root_visit = 0;
		int next_transversing_edge_movement = 0;
		
		if (root == NULL) return;
		
		while (root_visit < 3){		

			this->node_after_tranversing_edges[next_transversing_edge_movement] = curr_node->index;

			if (root == curr_node){
				if (root_visit == 0){
					
					CURRENT_DIRECTION = DIRECTION_DOWN;

					prev_node = curr_node;
					curr_node = root->left;
					root_visit = 1;
				}
				else if(root_visit == 1){
					this->get_node_first_right_movement[curr_node->index] = next_transversing_edge_movement;
					CURRENT_DIRECTION = DIRECTION_DOWN;

					prev_node = curr_node;
					curr_node = root->right;
					root_visit = 2;
				}
				else{
					CURRENT_DIRECTION = DIRECTION_UP;

					prev_node = curr_node;
					curr_node = root->parent;
					root_visit = 3;

					if (delete_nodes)
						delete (prev_node + 0);
				}
			}
			else if (curr_node->left == NULL && curr_node->right == NULL){

				this->get_node_first_right_movement[curr_node->index] = next_transversing_edge_movement;

				CURRENT_DIRECTION = DIRECTION_UP;
				prev_node = curr_node;
				curr_node = curr_node->parent;

				if (delete_nodes) 
					delete (prev_node+0);
			}
			else if (curr_node->parent == prev_node){
				CURRENT_DIRECTION = DIRECTION_DOWN;
				prev_node = curr_node;
				curr_node = curr_node->left;
			}
			else if (curr_node->left == prev_node){
				this->get_node_first_right_movement[curr_node->index] = next_transversing_edge_movement;
				
				CURRENT_DIRECTION = DIRECTION_DOWN; //
				prev_node = curr_node;
				curr_node = curr_node->right;

			}
			else if (curr_node->right == prev_node){
				CURRENT_DIRECTION = DIRECTION_UP;
				prev_node = curr_node;
				curr_node = curr_node->parent;
				if (delete_nodes) 
					delete (prev_node + 0);
			}

			if (curr_node != NULL)
				direction_up[next_transversing_edge_movement++] = CURRENT_DIRECTION;
			else
				std::swap(curr_node, prev_node);

		}
		
	}

	void fillUpOrDownDirections(){
		int num_of_edges_tranversals = 2 * (n - 1);
		get_node_first_right_movement = new int[n];
		direction_up = new bool[num_of_edges_tranversals];
		node_after_tranversing_edges = new int[num_of_edges_tranversals];
		
		int next_transversing_edge_movement = 0;
		int current_index = 0;
		tranverseAndFillDirections(false);
	}    

public:
	int* values;
	int n;
	TreeNode* root;
	bool* direction_up;
	//int* first_occurence_of_index; // in first_occurence_of_index[x] = y1 , y1 is not a position in direction_up, but the separation (which is a vertex) between positions (which indicates a direction in a edge): y1 - 1 | y1
	//int* last_occurence_of_index; // in last_occurence_of_index[x] = y2 , y2 is not a position in direction_up, but the separation (which is a vertex) between positions (which indicates a direction in a edge): y2 | y2 + 1
	//this means that the edges tranverse from the subtree of x are in postitions z such that => first_occurence_of_index[x] <= z < last_occurence_of_index[x]
	int* get_node_first_right_movement; //  visiting_right_edges_movement[x] = y, means that after following y moviments in direction_up you get to node x and no left subtree edge mov will be found after those mov.
	int* node_after_tranversing_edges; // in node_after_tranversing_edges[x] indicates which node you are after making the x firsts edges tranversing moviments

	CartesianTree(int* values, int n){
		this->values = values;
		this->n = n;
		buildCartesianTree();
		fillUpOrDownDirections();
	}

	~CartesianTree(){
		tranverseAndFillDirections(true);
		delete[] node_after_tranversing_edges;
		delete[] get_node_first_right_movement;
	}
};