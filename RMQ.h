#include "stdafx.h"
#include "CartesianTree.h"

class bitstring{

public:
	int bit_string;
	int zero_suffix_size;
	int zero_prefix_size;
	int min_value;

	bitstring(int bit_string, int zero_suffix_size, int zero_prefix_size, int min_value){
		this->zero_prefix_size = zero_prefix_size;
		this->zero_suffix_size = zero_suffix_size;
		this->min_value = min_value;
		this->bit_string = bit_string;
	}
};

class cluster_bitstream{
public:
	int min_level;
	int min_level_last_mov_index;
	int level;
	cluster_bitstream(){
	}

	cluster_bitstream(int min_level, int min_level_last_mov_index, int level){
		this->min_level = min_level;
		this->level = level;
		this->min_level_last_mov_index = min_level_last_mov_index;
	}
};

class cluster{
public:
	int bit_stream;
	int min_value; // from nodes that begins before the first move and ends ?? the last
	cluster(){
	}
	cluster(int bit_stream, int min_value){
		this->bit_stream = bit_stream;
		this->min_value = min_value;
	}
};

class RMQ{
public:
	int* values;
	int* rmq;
	
	int n, logn, numOfClusters;
	CartesianTree* cartesianTree;
	cluster_bitstream **stored_min_index;
	cluster* clusters;
	int** clusters_min_values;
	int clusters_size;
	int log_num_of_clusters;
	int stored_min_index_size;
	int size_of_cluster_value_array;

	RMQ(int* values, int n){
		this->values = values;
		this->n = n;

		this->logn = 0;
		while (n > (1 << logn)) logn++;
		int log_div_factor = 4;
		this->clusters_size = (logn + 1 + log_div_factor - 1) / log_div_factor;
		this->numOfClusters = (2*n - 1 + clusters_size - 1) / clusters_size;
		clusters = new cluster[this->numOfClusters];

		this->log_num_of_clusters = 0;
		while (this->numOfClusters > (1 << this->log_num_of_clusters)) this->log_num_of_clusters++;

		build();
	}

	~RMQ(){
		delete cartesianTree;
		for (int i = 0; i < size_of_cluster_value_array; i++){
			delete[] clusters_min_values[i];
		}
		delete[] clusters_min_values;

		delete[] clusters;

		for (int i = 0; i < stored_min_index_size; i++){
			delete[] stored_min_index[i];
		}

		delete[] stored_min_index;
	}

	inline int to_plus_or_minus(bool bit){
		return bit ? -1 : +1;
	}

	void build(){
		this->cartesianTree = new CartesianTree(values, n);
		fill_clusters_bitstream();
		 
		int edge_mov_index = 0;
		for (int cluster_index = 0; cluster_index < this->numOfClusters; cluster_index++){
			int cluster_mask = 0;
			int min_value_in_curr_cluster = 999999999;

			for (int curr_bit_in_cluster = 0; curr_bit_in_cluster < this->clusters_size; curr_bit_in_cluster++){

				int node_index_before_move_in_original_values = edge_mov_index < 2 * n - 1 ?
					this->cartesianTree->node_after_tranversing_edges[edge_mov_index] : this->cartesianTree->node_after_tranversing_edges[2 * n - 1 - 1];
				
				int value_in_original_values = this->values[node_index_before_move_in_original_values];
				int mov_direction_up = edge_mov_index < 2 * n - 1 ? (!!this->cartesianTree->direction_up[edge_mov_index]) : false;
				
				min_value_in_curr_cluster = std::min(min_value_in_curr_cluster, value_in_original_values);
				cluster_mask = (cluster_mask << 1) | mov_direction_up;

				edge_mov_index++;
			}

			clusters[cluster_index].bit_stream = cluster_mask;
			clusters[cluster_index].min_value = min_value_in_curr_cluster;
		}

		size_of_cluster_value_array = this->numOfClusters; // inicialize esta variável

		clusters_min_values = new int*[size_of_cluster_value_array];

		for (int cluster_index = 0; cluster_index < this->numOfClusters; cluster_index++){
			clusters_min_values[cluster_index] = new int[this->log_num_of_clusters];
			clusters_min_values[cluster_index][0] = clusters[cluster_index].min_value;// get_value(0, this->clusters_size - 1, this->clusters[cluster_index]);
		}

		for (int distancia_exponente = 1; (1 << distancia_exponente) <= this->numOfClusters; distancia_exponente++){
			int distance = (1 << distancia_exponente);
			int half_distance = (distance >> 1);

			for (int cluster_index = 0; cluster_index < this->numOfClusters; cluster_index++){

				int neibor_in_the_middle = (cluster_index + half_distance < this->numOfClusters) ?
					cluster_index + half_distance : this->numOfClusters - 1;
				
				clusters_min_values[cluster_index][distancia_exponente] =
					std::min(
					clusters_min_values[cluster_index][distancia_exponente - 1],
					clusters_min_values[neibor_in_the_middle][distancia_exponente - 1]
					);
			}
		}
	}

	void fill_clusters_bitstream(){
		stored_min_index_size = this->clusters_size + 1; // incialize corretamente esta variável
		this->stored_min_index = new cluster_bitstream*[stored_min_index_size];
		
		this->stored_min_index[0] = new cluster_bitstream[1];
		new (&this->stored_min_index[0][0])
			cluster_bitstream(0, 0, 0);

		for (int mask_lenght = 1; mask_lenght <= this->clusters_size; mask_lenght++){
			this->stored_min_index[mask_lenght] = new cluster_bitstream[(1 << mask_lenght)];
			for(int cartesian_mask = 0; cartesian_mask < (1 << mask_lenght); cartesian_mask++){
				bool last_bit = !!(cartesian_mask & 1);
				int last_bit_plus_or_minus = to_plus_or_minus(last_bit);

				if (mask_lenght - 1 == 0){
					new (&this->stored_min_index[mask_lenght][cartesian_mask])
						cluster_bitstream(last_bit_plus_or_minus, 0, last_bit_plus_or_minus);
				}
				else{

					int min_level_from_prefix = this->stored_min_index[mask_lenght - 1][cartesian_mask >> 1].min_level;
					int level_from_prefix = this->stored_min_index[mask_lenght - 1][cartesian_mask >> 1].level;
					int min_last_mov_index_from_prefix = this->stored_min_index[mask_lenght - 1][cartesian_mask >> 1].min_level_last_mov_index;

					if (level_from_prefix + last_bit_plus_or_minus < min_level_from_prefix){
						new (&this->stored_min_index[mask_lenght][cartesian_mask])
							cluster_bitstream(level_from_prefix + last_bit_plus_or_minus, mask_lenght - 1, level_from_prefix + last_bit_plus_or_minus);
					}
					else{
						new (&this->stored_min_index[mask_lenght][cartesian_mask])
							cluster_bitstream(min_level_from_prefix, min_last_mov_index_from_prefix, level_from_prefix + last_bit_plus_or_minus);
					}
				}
			}
		}
	}

	int num_of_movs_to_make(int ini, int fim, int mask){

		int mask_size = fim - ini + 1;
		int finals_bit_mask = (1 << mask_size) - 1;
		int bits_after_final_bit = clusters_size - 1 - fim;
		int trimmed_mask = (mask >> bits_after_final_bit) & finals_bit_mask;

		int mask_first_bit = 1 << (mask_size - 1);
		int inicial_bit_in_mask = !!(mask_first_bit & trimmed_mask);
		
		int last_mov_index = this->stored_min_index[mask_size][trimmed_mask].min_level_last_mov_index;
		
		if (last_mov_index == 0 && (inicial_bit_in_mask == 0)){ //inicial bit in mask é para baixo (aumenta o level), não se deve andar nada
			return 0;
		}
		return 
			last_mov_index + 1;
	}

	int get_min_value_in_movs(int inicial_mov_pos, int final_mov_pos, int cluster_index){
		int moves_to_make = num_of_movs_to_make(inicial_mov_pos, final_mov_pos, this->clusters[cluster_index].bit_stream);
		int node_pos = cartesianTree->node_after_tranversing_edges[cluster_index * this->clusters_size + inicial_mov_pos + moves_to_make];
		return values[node_pos];
	}

	int range_min_query(int inicial_index_in_value, int final_index_in_value){

		//int value_from_inicial_node = values[inicial_index_in_value]; // para considerar não atravessar nenhuma aresta.
		int ini = this->cartesianTree->get_node_first_right_movement[inicial_index_in_value]; // qtd minima de movs que deve ser feita.
		int fim = this->cartesianTree->get_node_first_right_movement[final_index_in_value] - 1; // não percorre o movimento da aresta à direita de final_index_in_value, pois não está no range.

		if (ini > fim) // significa que nenhuma aresta deve ser percorrida, apenas o node é considerado.
			return values[inicial_index_in_value];

		int cluster_of_inicial_query_pos = ini / this->clusters_size;
		int cluster_of_final_query_pos = fim / this->clusters_size;
		int pos_into_inicial_cluster = ini % this->clusters_size;
		int pos_into_final_cluster = fim % this->clusters_size;

		if (cluster_of_inicial_query_pos == cluster_of_final_query_pos){
			return get_min_value_in_movs(pos_into_inicial_cluster, pos_into_final_cluster, cluster_of_final_query_pos);
		}	
		else{
			int first_cluster_in_interval = (cluster_of_inicial_query_pos + 1);
			int last_cluster_in_interval = (cluster_of_final_query_pos - 1);
			int distance = last_cluster_in_interval - first_cluster_in_interval + 1;

			int min_value_in_clusters_in_between = 999999999;
			if (distance){
				int log2_dist = (int) (std::log2(distance) + 0.00001);
				min_value_in_clusters_in_between =
					std::min(
					this->clusters_min_values[first_cluster_in_interval][log2_dist],
					this->clusters_min_values[last_cluster_in_interval - (1 << log2_dist) + 1][log2_dist]
					);
			}

			int min_value_in_inicial_cluster = get_min_value_in_movs(pos_into_inicial_cluster, this->clusters_size - 1, cluster_of_inicial_query_pos);
			int min_value_in_final_cluster = get_min_value_in_movs(0, pos_into_final_cluster, cluster_of_final_query_pos);
			int min_value_in_inicial_or_final_cluster = std::min(min_value_in_inicial_cluster, min_value_in_final_cluster);
			return std::min(min_value_in_inicial_or_final_cluster, min_value_in_clusters_in_between);
		}
	}
	
};