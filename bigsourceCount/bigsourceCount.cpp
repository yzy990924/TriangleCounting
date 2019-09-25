﻿#include <iostream>
#include <fstream>
#include <memory.h>
#include <vector>
#include <set>
#include <algorithm>
#include <omp.h>
//大数据集
//#define MAX 4900000
#define MAX 16780000  //pos1
using namespace std;

int deg[MAX];  //记录每个结点的度数信息
vector<int> edge[MAX];  //记录每个结点的有效邻接点

//两个vector求交集
vector<int> vec_intersection(vector<int> v1, vector<int> v2) {
	vector<int> v;
	set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));
	return v;
}

int main() {
	//一些必要的初始化或清空操作
	memset(deg, 0, sizeof(deg));
	for (int i = 0; i < MAX; i++)
		edge[i].clear();

	int sum = 0;  //用于统计三角形的个数
	int x[2];  //用于读取每两个的源节点，目的结点
	int maxn = 0, totalEdge = 0, maxd = 0, emaxd = 0;
	//总结点数     总边数     最大度数  最大有效度数

	//第一遍遍历源文件得到node，edge，degree等信息
	ifstream inF;
	//inF.open("D:/BDCI/dateSet/./soc-LiveJournal1.bin", std::ifstream::binary);
	inF.open("D:/BDCI/dateSet/./s24.kron.edgelist.bin", std::ifstream::binary);  //pos2
	while (!inF.eof()) {
		inF.read(reinterpret_cast<char*>(x), sizeof(int) * 2);
		if (inF.eof())
			break;
		maxn = max(maxn, x[0]);
		maxn = max(maxn, x[1]);
		deg[x[0]]++;
		deg[x[1]]++;
		totalEdge += 1;
	}
	inF.close();

	//    //统计最大度数
	//    for(int i = 1; i <= maxn; i++)
	//        maxd = max(maxd, deg[i]);

		//第二遍遍历得到图数据的有效邻接表存储
		//inF.open("D:/BDCI/dateSet/./soc-LiveJournal1.bin", std::ifstream::binary);
	inF.open("D:/BDCI/dateSet/./s24.kron.edgelist.bin", std::ifstream::binary);  //pos3
	while (!inF.eof()) {
		inF.read(reinterpret_cast<char*>(x), sizeof(int) * 2);
		if (inF.eof())
			break;

		if (deg[x[0]] > deg[x[1]])
			edge[x[1]].push_back(x[0]);
		else if ((deg[x[0]] == deg[x[1]]) && (x[0] > x[1]))
			edge[x[1]].push_back(x[0]);
		else
			edge[x[0]].push_back(x[1]);
	}
	inF.close();

	omp_set_num_threads(8);
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i <= maxn; i++) {
			//去除有效邻接表中的重复元素
			sort(edge[i].begin(), edge[i].end());  //对每个结点的邻接链表排序
			vector<int>::iterator iter = unique(edge[i].begin(), edge[i].end());
			edge[i].erase(iter, edge[i].end());
			//统计最大有效度数
			int s = edge[i].size();
#pragma omp critical
			emaxd = max(emaxd, s);
		}
	}

#pragma omp parallel
	{
#pragma omp for
		//计算三角形个数
		for (int i = 0; i <= maxn; i++) {
			vector<int> Vtmp;
			int sz = edge[i].size();
			for (int j = 0; j < sz; j++) {
				int tmp = edge[i][j];
				Vtmp = vec_intersection(edge[i], edge[tmp]);
				int ssum = Vtmp.size();
#pragma omp critical
				sum += ssum;
			}
		}
	}

	cout << "结点数：" << maxn << endl;
	cout << "边数(含重边)：" << totalEdge << endl;
	//    cout << "最大度数：" << maxd << endl;
	cout << "最大有效度数：" << emaxd << endl << endl;
	cout << "三角形个数为：" << sum << endl;

	return 0;
}
