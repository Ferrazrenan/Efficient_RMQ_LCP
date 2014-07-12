#include "stdafx.h"

class suffix{
public:
	int suffix_index;
	int first_half_position;
	int second_half_position;

	bool operator < (const suffix &temp) const{
		return first_half_position < temp.first_half_position || 
			(first_half_position == temp.first_half_position && second_half_position < temp.second_half_position);
	}
};

class SuffixArray{

	int find_position_of_suffix(int suffix_index){
		if (suffix_index == n) return -1;
		return AS[suffix_index];
	}

	void swap_suffix_arrays(int index1, int index2){
		std::swap(SA[index1], SA[index2]);
		//std::swap(AS[index1], AS[index2]);
		AS[SA[index1]] = index1;
		AS[SA[index2]] = index2;
	}

	void orderSuffixArray(){

		std::map<int, int> intervalos;
		intervalos.insert(std::make_pair(n - 1, 0));

		for (int curr_suffix_index = n - 1; curr_suffix_index >= 0; curr_suffix_index--){
			int curr_position_in_SA = find_position_of_suffix(curr_suffix_index);
			std::map< int, int >::iterator iterator_interval = intervalos.lower_bound(curr_position_in_SA);

			if (iterator_interval == intervalos.end()) continue;

			int interval_inicial = iterator_interval->second;
			int interval_end = iterator_interval->first;

			if (curr_position_in_SA < interval_inicial || interval_end < curr_position_in_SA) continue;

			swap_suffix_arrays(curr_position_in_SA, interval_inicial);

			int prev_suffix_final_position = find_position_of_suffix(curr_suffix_index + 1);
			int curr_position = interval_inicial;
			int end_position = interval_end;


			while (curr_position < end_position){
				if (((unsigned char)s[SA[curr_position + 1]]) < ((unsigned char)s[SA[curr_position]]) ||
					(((unsigned char)s[SA[curr_position + 1]]) == ((unsigned char)s[SA[curr_position]]) && AS[SA[curr_position + 1] + 1] < prev_suffix_final_position)){
					swap_suffix_arrays(curr_position + 1, curr_position);
					curr_position++;
				}
				else{
					swap_suffix_arrays(curr_position + 1, end_position);
					end_position--;
				}
			}

			intervalos.erase(iterator_interval);

			if (interval_inicial < curr_position - 1){
				intervalos.insert( std::make_pair(curr_position - 1, interval_inicial) );
			}

			if (end_position + 1 < interval_end){
				intervalos.insert(std::make_pair(interval_end, end_position + 1));
			}

			prev_suffix_final_position = curr_position;
			//print();
		}
	}

	struct suffixPtrComp
	{
		bool operator()(const suffix* lhs, const suffix* rhs) const  { return *lhs < *rhs; }
	};

	void order(suffix** suffixes){
		
		for (int i = 0; i < n; i++){
			list[suffixes[i]->second_half_position+1].push(suffixes[i]);
		}

		int curr_position = 0;
		for (int i = 0; i <= n; i++){
			while(!list[i].empty()){
				suffixes[curr_position++] = list[i].front();
				list[i].pop();
			}
		}

		for (int i = 0; i < n; i++){
			list[suffixes[i]->first_half_position+1].push(suffixes[i]);
		}

		curr_position = 0;
		for (int i = 0; i <= n; i++){
			while (!list[i].empty()){
				suffixes[curr_position++] = list[i].front();
				list[i].pop();
			}
		}

	}

	int num_previous_chars[(1 << 8) + 1];
	void suffixSort(){
		n = s.size();
		const char* c_s = s.c_str();
		suffix *SA_pair = new suffix[n];

		int* suffix_position = new int[n];

		for (int i = 0; i < (1<<8) + 1; i++) num_previous_chars[i] = 0;
		for (int i = 0; i < n; i++){
			unsigned char char_index = s[i];
			num_previous_chars[char_index + 1]++;
		}
		for (int i = 1; i < (1 << 8) + 1; i++) num_previous_chars[i] += num_previous_chars[i - 1];

		for (int i = 0; i < n; i++){
			unsigned char char_index = s[i];
			SA_pair[i].first_half_position = num_previous_chars[char_index];
			SA_pair[i].second_half_position = -1;
			SA_pair[i].suffix_index = i;
		}

		suffix **ptr_suffix = new suffix*[n];
		for (int i = 0; i < n; i++)ptr_suffix[i] = &SA_pair[i];
		
		order(ptr_suffix);

		for (int suffix_size = 1; suffix_size <= n; suffix_size <<= 1){
			
			int smallers_guys = 0;
			int equal_guys = 1;

			for (int j = 0; j < n; j++){

				int first_half_position_temp = smallers_guys;

				if (j + 1 < n && *ptr_suffix[j] < *ptr_suffix[j+1]){
					smallers_guys += equal_guys;
					equal_guys = 0;
				}
				
				ptr_suffix[j]->first_half_position = first_half_position_temp;
				
				equal_guys++;
			}

			for (int j = 0; j < n; j++) 
				suffix_position[ptr_suffix[j]->suffix_index] = j;

			for (int i = 0; i < n; i++){
				int second_half_start_point = ptr_suffix[i]->suffix_index + suffix_size;
				int suffix_index = second_half_start_point <= n - 1 ? suffix_position[second_half_start_point] : -1;
				ptr_suffix[i]->second_half_position = suffix_index >= 0 ? ptr_suffix[suffix_index]->first_half_position : -1;
			}

			order(ptr_suffix);
		}

		for (int i = 0; i < n; i++) SA[i] = ptr_suffix[i]->suffix_index;
		for (int i = 0; i < n; i++) AS[SA[i]] = i;
	}

	public:
		int n;
		int* AS;
		int* SA;
		int* LCP;
		std::string s;
		std::queue<suffix*> *list;

	SuffixArray(std::string* text, bool linear){
		s = *text;
		n = (*text).size();

		this->SA = new int[n];
		this->AS = new int[n];
		this->LCP = new int[n];
		this->list = new std::queue<suffix*>[n + 1];

		for (int i = 0; i < n; i++){
			SA[i] = i;
			AS[i] = i;
		}

		if (linear)
			suffixSort();
		else
			orderSuffixArray();

		fillLCP();
	}

	void fillLCP(){
		int current_matched_size = 0;
		for (int suffix_index = 0; suffix_index < n; suffix_index++){
			int suffix_position_in_SA = AS[suffix_index];			
			int consecutive_suffix = suffix_position_in_SA == n - 1 ? n : SA[suffix_position_in_SA + 1];

			while (std::max(suffix_index, consecutive_suffix) + current_matched_size < n &&
				s[suffix_index + current_matched_size] == s[consecutive_suffix + current_matched_size]) current_matched_size++;
			
			this->LCP[suffix_position_in_SA] = current_matched_size;
			if (current_matched_size) current_matched_size--;
		}

		for (int i = 0; i < n-1; i++){
			for (int j = 0; j < this->LCP[i]; j++) if (s[SA[i] + j] != s[SA[i + 1] + j]) throw std::exception();
			if (std::max(SA[i], SA[i + 1]) + this->LCP[i + 1] <  n  && s[SA[i] + this->LCP[i]] == s[SA[i + 1] + +this->LCP[i]]) throw std::exception();
		}
	}

	void print(){
		for (int i = 0; i < n; i++){
			for (int j = SA[i]; j < n; j++){
				std::cout << s[j];
			}
			std::cout << std::endl;
		}
	}
};