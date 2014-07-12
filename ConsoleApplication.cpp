// ConsleApplication.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "RMQ.h"

const int num_of_tests = 1 << 10;
const int value_max_size = 1<<15;
int num_of_queries = 1 << 10;
int v[value_max_size];
char already_sorted[value_max_size];

int _tmain(int argc, _TCHAR* argv[])
{
	
	int distinct_tests = 0;
	while (distinct_tests < num_of_tests){
		int c = 0;
		int test_value_size = 1 + ((unsigned int)std::rand())%value_max_size;

		std::memset(already_sorted, 0, sizeof(already_sorted));
		while (c < test_value_size){
			int sorted_number = ((unsigned int)std::rand()) % test_value_size;
			while (already_sorted[sorted_number]){
				sorted_number = (sorted_number + 1) % test_value_size;
			}
			already_sorted[sorted_number] = true;
			v[c++] = sorted_number;
		}

		RMQ* rmq = new RMQ(v, c);

		int q = 0;
		while (q++ < num_of_queries){
			int maxi = ((unsigned int)std::rand()) % c;
			int mini = ((unsigned int)std::rand()) % (maxi + 1);
			int result = rmq->range_min_query(mini, maxi);
			int result2 = 999999;
			for (int i = mini; i <= maxi; i++) result2 = std::min(result2, v[i]);
			if (result2 != result)
				throw std::exception();
		}

		delete rmq;
		distinct_tests++;
	}
	

	return 0;
}
//#include "SuffixArray.h"
//#include "SuffixTree.h"
//#include "RMQ.h"
//#include <windows.h>
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	//FILE* entrada;
//	//fopen_s(&entrada, "entrada.txt", "r");
//	std::fstream my_stream;
//	my_stream.open("entrada.txt");
//	//while (true){
//		std::string exp;
//		std::string tolken;
//		//std::cin >> exp;
//		
//		while (my_stream >> tolken){
//			for (int j = 0; j < tolken.size(); j++){
//				for (int i = 0; i < 4; i++)	exp += " " + (tolken[j] | ((((int)tolken[j]) + (1 << (i)))));
//			}
//		}
//		//std::getline(std::cin, exp);
//		SYSTEMTIME st;
//		GetSystemTime(&st);
//		int inicial = st.wSecond * 1000 + st.wMilliseconds;
//		SuffixArray *SA1 = new SuffixArray(&exp, false);
//		GetSystemTime(&st);
//		long long mid = st.wSecond * 1000 + st.wMilliseconds;
//		SuffixArray *SA2 = new SuffixArray(&exp, false);
//		GetSystemTime(&st);
//		long long final = st.wSecond * 1000 + st.wMilliseconds;
//		std::cout << mid - inicial << " " << final - mid << std::endl;
//		
//		inicial = st.wSecond * 1000 + st.wMilliseconds;
//		SuffixTree* ST1 = new SuffixTree(exp, SA1->SA, SA1->LCP);
//		mid = st.wSecond * 1000 + st.wMilliseconds;
//		SuffixTree* ST2 = new SuffixTree(exp, SA2->SA, SA2->LCP);
//		final = st.wSecond * 1000 + st.wMilliseconds;
//		std::cout << mid - inicial << " " << final - mid << std::endl;
//
//
//		ST1->preprocessMostFrequentlyString();
//		ST2->preprocessMostFrequentlyString();
//
//		for (int i = 0; i < exp.size(); i++) if (SA1->SA[i] != SA2->SA[i]) 
//			throw std::exception();
//		//std::cout <<  << std::endl;
//	//}
//	my_stream.close();
//	system("Pause");
//	
//	for (int size = 1; size < exp.size(); size++){
//		int start = ST1->MostCommonSubstringSize(size);
//		for (int ii = start; ii < start + size; ii++)
//			cout << exp[ii];
//		cout <<  " " << size << endl;
//		system("Pause");
//	}
//	
//	//my_stream.open("entrada.txt");
//	//while (my_stream >> tolken){
//	//	//inicial = st.wSecond * 1000 + st.wMilliseconds;
//	//	int first_index_ST1 = ST1->first_occurence(tolken);		
//	//	//mid = st.wSecond * 1000 + st.wMilliseconds;
//	//	int first_index_ST2 = ST2->first_occurence(tolken);
//	//	//final = st.wSecond * 1000 + st.wMilliseconds;
//	//	
//	//	if (first_index_ST1 != first_index_ST2) throw exception();
//	//	
//	//	//ST2->MostCommonSubstringSize(tolken.size());
//	//	//if (first_index_ST1 >= 0) for (int i = 0; i < tolken.size() + 50 && i < exp.size(); i++) cout << exp[first_index_ST1 + i];
//	//}
//	//my_stream.close();
//	system("Pause");
//
//	return 0;
//}
//
