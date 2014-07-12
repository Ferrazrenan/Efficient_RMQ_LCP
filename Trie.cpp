class Trie{

	node& add_string(node& current_node, int string_ini, int string_end, string& s){
		int common_size = 0;
		while (common_size < current_node.s_common_size && string_ini + common_size <= string_end &&
			s[current_node.inicial_index + common_size] == s[string_ini + common_size]) common_size++;

		if (common_size == current_node.s_common_size && string_end - string_ini + 1 == common_size){
			current_node.num_suffix_passes_through++;
			return current_node;
		}
		else if (common_size == current_node.s_common_size){ //node is over berfore missmatch
			char next_suffix_char = s[string_ini + common_size];
			if (current_node.has_child(next_suffix_char)){
				node& node_child = add_string(*current_node.get_child(next_suffix_char), string_ini + common_size, string_end, s);
				current_node.replace_child(next_suffix_char, &node_child);
			}
			else current_node.add_child(next_suffix_char, new node(string_ini + common_size, string_end + 1 - (string_ini + common_size)));

			current_node.num_suffix_passes_through++;

			return current_node;
		}
		else if (string_ini + common_size == string_end + 1){ // suffix is over berfore missmatch
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
			char next_suffix_char = s[string_ini + common_size];
			char next_node_char = s[current_node.inicial_index + common_size];
			node& new_node_in_between = *new node(current_node.inicial_index, common_size);
			new_node_in_between.add_child(next_node_char, &current_node);
			new_node_in_between.add_child(next_suffix_char, new node(string_ini + common_size, string_end + 1 - (string_ini + common_size)));
			new_node_in_between.num_suffix_passes_through += current_node.num_suffix_passes_through;
			current_node.s_common_size -= common_size;
			current_node.inicial_index += common_size;
			return new_node_in_between;
		}
	}

public:
	node* root;

	Trie(){
		root = NULL; //new node(0, s.size());
	}

	void add_string(string& s, int inicial_index, int final_index){
		if(root == NULL)
			root = new node(0, final_index - inicial_index + 1);
		else
			root = &add_string(*root, inicial_index, final_index, s);		
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
};
