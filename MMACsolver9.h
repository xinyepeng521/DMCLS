#pragma once
#include <chrono>
#include<iostream>
#include<set>
#include<unordered_set>
#include<sstream>
#include <fstream>  
#include<random>
#include <chrono>
#include <iostream>
using namespace std;
#define LAMBDA 10000

int Short_k;
pair<int,int> Long_k;
int s_restore;
int min_restore = 100;
int res_iterator = 0;
int restor_strength = 1;
int num_1 = 0;
int rd_MOVE_2 = 2;

mt19937 gen(0);
class Edge;
class Node {
public:
	int ID;

	int layPos;//the layer-pos
	int layerID;//the pos in layer

	int maxCross = 0;
	vector<Edge*>outEdge;
	vector<Edge*>inEdge;

	Node() {};
	Node(int i) { ID = i; }
};
class Edge {
public:
	Node* source;
	Node* sink;
	int cross = 0;
	int tmpCross;
	Edge(Node* src = nullptr, Node* snk = nullptr) : source(src), sink(snk) {
	}
};
class Move {
public:
	Node* node;
	int newPos;
	int delta;
	Move(Node* n, int p, int d) {
		node = n;
		newPos = p;
		delta = d;
	}
};
class MMACsolver {
private:
	struct SnapShot {
		vector<vector<int>> layerOrder;
		vector<int> edgeCrossValues;
		vector<int> nodeMaxCrossValues;
		int currentMaxCross;
		set<int> maxCrossNodes;
		vector<int> nodeAges;
	} snapshot;
public:
	vector<vector<Node*>>layer;//[ , );
	vector<pair<int, int>>layerBound;
	set<Node*>maxCrossNode;
	vector<Edge>allEdge;
	vector<Node>allNode;
	vector<int>age;
	vector<vector<int>>Tabu;
	int best_solution = INT32_MAX;
	int iterator;
	int maxCross = 0;
	MMACsolver(string instance, int seed)
	{
		gen.seed(seed);
		ifstream infile(instance);
		if (!infile) {
			cerr << "Open instance file failed." << endl;
		}

		int nodeNum, edgeNum, layerNum;
		infile >> nodeNum >> edgeNum >> layerNum;
		min_restore = nodeNum * s_restore;
		age.assign(nodeNum, 0);
		layer.assign(layerNum, vector<Node*>());
		allEdge.assign(edgeNum, Edge());
		for (int i = 0; i < nodeNum; i++)
			allNode.push_back(Node(i));
		{
			int begin = 0;
			for (int i = 0; i < layerNum; i++)//Allocate the number of vertices per layer, determine the layer number corresponding to each vertex.
			{
				int temp;
				infile >> temp;
				layerBound.push_back({ begin,begin + temp });
				for (int j = 0; j < temp; j++)
				{
					allNode[begin].layerID = i;
					layer[i].push_back(&allNode[begin]);
					allNode[begin++].layPos = j;
				}
			}
		}

		int lower, higer, temp1, temp2, ind = 0;
		for (int i = 0; i < edgeNum; i++)
		{
			infile >> temp1 >> temp2;
			lower = min(temp1, temp2) - 1;
			higer = max(temp1, temp2) - 1;

			allEdge[ind] = (Edge(&allNode[lower], &allNode[higer]));
			allNode[lower].outEdge.push_back(&allEdge[ind]);
			allNode[higer].inEdge.push_back(&allEdge[ind++]);
		}
		for (int i = 0; i < allNode.size(); i++)
		{
			Tabu.push_back(vector<int>(layer[allNode[i].layerID].size(), 0));
		}
	}
	auto randomConstruction() -> void;
	auto solve() -> int;
	auto init() -> void;
	auto calcCross() -> void;
	auto isCross(int edge1_out, int edge1_in, int edge2_out, int edge2_in) -> int;
	auto calcMaxCross() -> int;
	auto makeMove(Move move) -> void;
	auto findMove() -> Move;
	auto calcSwapDelta(Node* node, int newPos,
		Node* newNode, int newNodePos) -> int;
	auto countTmpDelta(Node* newNode) -> int;
	auto calcDelta(int cross, int newCross) -> int;

	auto tryMovePos(Node* n, int ubIndex, int& count, int delt, Move& bestMv) -> Move;
	auto tryMoveNeg(Node* n, int lbIndex, int& count, int delt, Move& bestMv) -> Move;

	auto tryMovePos2(Node* n, int ubIndex, int& count, int delt, Move& bestMv) -> Move;
	auto tryMoveNeg2(Node* n, int lbIndex, int& count, int delt, Move& bestMv) -> Move;

	auto setTmpCross(int lbIndex, int ubIndex, int layerId) -> void;
	auto setNewMaxCross(Node* n) -> void;

	auto getMaxCrossNode() -> void;
	auto updateCrossAfterSwap(Node* n1, Node* n2) -> void;

	auto check() -> bool;
	int calcMaxCrossEdgeNum()
	{
		int num = 0;
		for (auto i : allEdge)
		{
			if (i.cross == maxCross)
			{
				num++;
				/*cout << i.source->ID << " " << i.sink->ID << endl;*/
			}
		}
		return num;

	}
	void recordState() {
		snapshot.layerOrder.clear();
		for (auto& l : layer) {
			vector<int> layerIDs;
			for (auto node : l) {
				layerIDs.push_back(node->ID);
			}
			snapshot.layerOrder.push_back(layerIDs);
		}

		snapshot.edgeCrossValues.resize(allEdge.size());
		for (size_t i = 0; i < allEdge.size(); ++i) {
			snapshot.edgeCrossValues[i] = allEdge[i].cross;
		}

		snapshot.nodeMaxCrossValues.resize(allNode.size());
		snapshot.nodeAges.resize(allNode.size());
		for (size_t i = 0; i < allNode.size(); ++i) {
			snapshot.nodeMaxCrossValues[i] = allNode[i].maxCross;
			snapshot.nodeAges[i] = age[i];
		}

		snapshot.currentMaxCross = maxCross;
		snapshot.maxCrossNodes.clear();
		for (auto node : maxCrossNode) {
			snapshot.maxCrossNodes.insert(node->ID);
		}
	}

	void restoreState() {
		for (int layerIdx = 0; layerIdx < layer.size(); ++layerIdx) {
			auto& currentLayer = layer[layerIdx];
			auto& savedOrder = snapshot.layerOrder[layerIdx];

			for (int pos = 0; pos < currentLayer.size(); ++pos) {
				Node* node = &allNode[savedOrder[pos]];
				currentLayer[pos] = node;
				node->layPos = pos;
				node->layerID = layerIdx;
			}
		}

		for (size_t i = 0; i < allEdge.size(); ++i) {
			allEdge[i].cross = snapshot.edgeCrossValues[i];
		}

		for (size_t i = 0; i < allNode.size(); ++i) {
			allNode[i].maxCross = snapshot.nodeMaxCrossValues[i];
			age[i] = snapshot.nodeAges[i];
		}

		maxCross = snapshot.currentMaxCross;
		maxCrossNode.clear();
		for (auto id : snapshot.maxCrossNodes) {
			maxCrossNode.insert(&allNode[id]);
		}
	}
};
auto MMACsolver::randomConstruction()->void {

	for (int i = 0; i < layerBound.size(); i++)
	{
		int length = layerBound[i].second - layerBound[i].first;
		for (int j = layerBound[i].first; j < layerBound[i].second; j++)
		{
			int rand = gen() % length + layerBound[i].first;
			swap(allNode[j].layPos, allNode[rand].layPos);
			swap(layer[i][allNode[j].layPos], layer[i][allNode[rand].layPos]);
		}
	}
}
auto MMACsolver::init()->void {
	iterator = 1;
	randomConstruction();
	calcCross();
	calcMaxCross();
}
auto MMACsolver::calcCross()->void {
	for (auto& i : layer)//for i:every layer
	{
		for (int j = 0; j < i.size(); j++)//for j:every Node in layer
		{
			for (auto& k : i[j]->outEdge)//for k:every edge in Pos
			{
				for (int l = j + 1; l < i.size(); l++)
				{
					for (auto& m : i[l]->outEdge)
					{
						int temp = isCross(j, k->sink->layPos, l, m->sink->layPos);
						k->cross += temp;
						m->cross += temp;
					}
				}
			}
		}
	}
}
auto MMACsolver::calcMaxCross()->int {
	maxCross = 0;
	for (int i = 0; i < layer.size(); i++)
	{
		for (int j = 0; j < layer[i].size(); j++)
		{
			for (auto k : layer[i][j]->outEdge)
			{
				layer[i][j]->maxCross = max(layer[i][j]->maxCross, k->cross);
			}
			for (auto k : layer[i][j]->inEdge)
			{
				layer[i][j]->maxCross = max(layer[i][j]->maxCross, k->cross);
			}
			if (layer[i][j]->maxCross > maxCross)
			{
				maxCrossNode.clear();
				maxCross = max(layer[i][j]->maxCross, maxCross);
				maxCrossNode.insert(layer[i][j]);
			}
			else if (maxCross == layer[i][j]->maxCross && maxCross != 0)
			{
				maxCrossNode.insert(layer[i][j]);
			}
		}
	}
	return maxCross;
}
auto MMACsolver::isCross(int edge1_out, int edge1_in, int edge2_out, int edge2_in)->int {
	if (edge2_out > edge1_out && edge2_in < edge1_in)
		return 1;
	if (edge1_out > edge2_out && edge1_in < edge2_in)
		return 1;
	return 0;
}
auto MMACsolver::calcSwapDelta(Node* node, int newPos, Node* newNode, int newNodePos)->int
{
	int delta = 0;
	for (auto& i : node->outEdge)
	{
		int posToISink = i->sink->layPos;
		for (auto& j : newNode->outEdge)
		{
			int posToJSink = j->sink->layPos;
			if (posToISink == posToJSink)
				continue;
			if (isCross(newPos, posToISink, newNodePos, posToJSink))//Cross--
			{
				i->tmpCross++;
				j->tmpCross++;
			}
			else
			{
				i->tmpCross--;
				j->tmpCross--;
			}
		}
	}
	for (auto& i : node->inEdge)
	{
		int posToISource = i->source->layPos;
		for (auto& j : newNode->inEdge)
		{
			int posToJSource = j->source->layPos;
			if (posToISource == posToJSource)
				continue;
			if (isCross(posToISource, newPos, posToJSource, newNodePos))//Cross--
			{
				i->tmpCross++;
				j->tmpCross++;
			}
			else
			{
				i->tmpCross--;
				j->tmpCross--;
			}
		}
	}
	return countTmpDelta(newNode) /*+ countTmpDelta(node)*/;
}
auto MMACsolver::tryMoveNeg(Node* n, int lbIndex, int& count, int delta, Move& bestMv)->Move
{
	int pos = -1;
	int temp2 = 0;
	for (int i = n->layPos - 1; i >= lbIndex; i--)//i is current index of n
	{

		/*temp2 += calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos + 1);
		int temp = temp2 + countTmpDelta(n);*/
		int temp = calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos + 1) + countTmpDelta(n);
		/*	if (Tabu[n->ID][i] > iterator)
				continue;*/
		if (temp < delta)
		{
			delta = temp;
			pos = i;
			count = 2;
			bestMv = Move(n, pos, delta);
		}
		else if (temp == delta)
		{
			if (age[n->ID] < age[bestMv.node->ID])
			{
				count = 2;
				pos = i;
				bestMv = Move(n, pos, delta);
			}
			else if (age[n->ID] == age[bestMv.node->ID] && !(gen() % (count++)))
			{
				pos = i;
				bestMv = Move(n, pos, delta);
			}
		}
	}
	return Move(n, pos, delta);
}
auto MMACsolver::tryMovePos(Node* n, int ubIndex, int& count, int delta, Move& bestMv)->Move {//扰动模式
	int pos = -1;
	int temp2 = 0;
	for (int i = n->layPos + 1; i <= ubIndex; i++)//i is current index of n
	{

		/*temp2 += calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos - 1);
		int temp = temp2 + countTmpDelta(n);*/
		int temp = calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos - 1) + countTmpDelta(n);
		/*if (Tabu[n->ID][i] > iterator)
			continue;*/
		if (temp < delta)
		{
			delta = temp;
			pos = i;
			count = 2;
			bestMv = Move(n, pos, delta);
		}
		else if (temp == delta)
		{
			if (age[n->ID] < age[bestMv.node->ID])
			{
				count = 2;
				pos = i;
				bestMv = Move(n, pos, delta);
			}
			else if (age[n->ID] == age[bestMv.node->ID] && !(gen() % (count++)))
			{
				pos = i;
				bestMv = Move(n, pos, delta);
			}
		}
	}
	return Move(n, pos, delta);
}
auto MMACsolver::tryMoveNeg2(Node* n, int lbIndex, int& count, int delta, Move& bestMv)->Move
{
	int pos = -1;
	int temp2 = 0;
	for (int i = n->layPos - 1; i >= lbIndex; i--)//i is current index of n
	{

		/*temp2 += calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos + 1);
		int temp = temp2 + countTmpDelta(n);*/
		int sc_mid = calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos + 1);
		int sc_new = countTmpDelta(n);
		temp2 += sc_mid;
		int temp = sc_mid + sc_new;
		int temp3 = temp2 + sc_new;
		/*if (Tabu[n->ID][i] > iterator)
			continue;*/
		if (temp3 < delta)
		{
			delta = temp3;
			pos = i;
			count = 2;
			bestMv = Move(n, pos, delta);
		}
		else if (temp3 == delta)
		{
			if (age[n->ID] < age[bestMv.node->ID])
			{
				count = 2;
				pos = i;
				bestMv = Move(n, pos, delta);
			}
			else if (age[n->ID] == age[bestMv.node->ID] && !(gen() % (count++)))
			{
				pos = i;
				bestMv = Move(n, pos, delta);
			}
		}
	}
	return Move(n, pos, delta);
}
auto MMACsolver::tryMovePos2(Node* n, int ubIndex, int& count, int delta, Move& bestMv)->Move//关键模式
{
	int pos = -1;
	int temp2 = 0;
	for (int i = n->layPos + 1; i <= ubIndex; i++)//i is current index of n
	{

		/*temp2 += calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos - 1);
		int temp = temp2 + countTmpDelta(n);*/
		int sc_mid = calcSwapDelta(n, i, layer[n->layerID][i], layer[n->layerID][i]->layPos - 1);//末尾分数
		int sc_new = countTmpDelta(n);//n移动到i的分数
		temp2 += sc_mid;//中间累计分数
		int temp = sc_mid + sc_new;//首位分数
		int temp3 = temp2 + sc_new;//总分数
		/*if (Tabu[n->ID][i] > iterator)
			continue;*/
		if (temp3 < delta)
		{
			delta = temp3;
			pos = i;
			count = 2;
			bestMv = Move(n, pos, delta);
		}
		else if (temp3 == delta)
		{
			if (age[n->ID] < age[bestMv.node->ID])
			{
				count = 2;
				pos = i;
				bestMv = Move(n, pos, delta);
			}
			else if (age[n->ID] == age[bestMv.node->ID] && !(gen() % (count++)))
			{
				pos = i;
				bestMv = Move(n, pos, delta);
			}
		}
	}
	return Move(n, pos, delta);
}
auto MMACsolver::setTmpCross(int lbIndex, int ubIndex, int layerId)->void {
	for (int i = lbIndex; i <= ubIndex; i++)
	{
		for (auto j : layer[layerId][i]->inEdge)
			j->tmpCross = j->cross;
		for (auto j : layer[layerId][i]->outEdge)
			j->tmpCross = j->cross;
	}
}
auto MMACsolver::calcDelta(int cross, int newCross)->int
{
	if (newCross < maxCross)
	{
		if (cross > maxCross)
			return LAMBDA * (maxCross - cross) - 1;
		if (cross < maxCross)
			return 0;
		if (cross == maxCross)
			return -1;
	}
	if (newCross > maxCross)
	{
		if (cross > maxCross)
			return LAMBDA * (newCross - cross);
		if (cross < maxCross)
			return LAMBDA * (newCross - maxCross) + 1;
		if (cross == maxCross)
			return LAMBDA * (newCross - maxCross);
	}
	if (newCross == maxCross)
	{
		if (cross > maxCross)
			return LAMBDA * (maxCross - cross);
		if (cross < maxCross)
			return 1;
		if (cross == maxCross)
			return 0;
	}


}
auto MMACsolver::setNewMaxCross(Node* n)->void {
	n->maxCross = 0;
	for (auto i : n->inEdge)
	{
		n->maxCross = max(i->cross, n->maxCross);
	}
	for (auto i : n->outEdge)
	{
		n->maxCross = max(i->cross, n->maxCross);
	}
}
auto MMACsolver::countTmpDelta(Node* newNode)->int
{
	int delta = 0;
	for (auto i : newNode->outEdge)
	{
		delta += calcDelta(i->cross, i->tmpCross);
	}
	for (auto i : newNode->inEdge)
		delta += calcDelta(i->cross, i->tmpCross);
	return delta;
}
auto MMACsolver::updateCrossAfterSwap(Node* n1, Node* n2)->void {
	for (auto& i : n1->inEdge)
	{
		for (auto& j : n2->inEdge)
		{
			if (i->source->layPos == j->source->layPos)
				continue;
			if (isCross(i->source->layPos, i->sink->layPos, j->source->layPos, j->sink->layPos))
			{
				i->cross--;
				if (i->cross + 1 == i->sink->maxCross)
					setNewMaxCross(i->sink);
				if (i->cross + 1 == maxCross && i->sink->maxCross != maxCross)
					maxCrossNode.erase(i->sink);

				if (i->cross + 1 == i->source->maxCross)
					setNewMaxCross(i->source);
				if (i->cross + 1 == maxCross && i->source->maxCross != maxCross)
					maxCrossNode.erase(i->source);

				j->cross--;
				if (j->cross + 1 == j->sink->maxCross)
					setNewMaxCross(j->sink);
				if (j->cross + 1 == maxCross && j->sink->maxCross != maxCross)
					maxCrossNode.erase(j->sink);

				if (j->cross + 1 == j->source->maxCross)
					setNewMaxCross(j->source);
				if (j->cross + 1 == maxCross && j->source->maxCross != maxCross)
					maxCrossNode.erase(j->source);
			}
			else {
				i->cross++;
				if (i->cross > i->sink->maxCross)
					i->sink->maxCross++;
				if (i->cross > maxCross)
				{
					maxCross = i->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(i->sink);
				}
				if (i->cross == maxCross)
					maxCrossNode.insert(i->sink);

				if (i->cross > i->source->maxCross)
					i->source->maxCross++;
				if (i->cross > maxCross)
				{
					maxCross = i->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(i->source);
				}
				if (i->cross == maxCross)
					maxCrossNode.insert(i->source);

				j->cross++;
				if (j->cross > j->sink->maxCross)
					j->sink->maxCross++;
				if (j->cross > maxCross)
				{
					maxCross = j->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(j->sink);
				}
				if (j->cross == maxCross)
					maxCrossNode.insert(j->sink);

				if (j->cross > j->source->maxCross)
					j->source->maxCross++;
				if (j->cross > maxCross)
				{
					maxCross = j->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(j->source);
				}
				if (j->cross == maxCross)
					maxCrossNode.insert(j->source);
			}
		}
	}
	for (auto& i : n1->outEdge)
	{
		for (auto& j : n2->outEdge)
		{
			if (i->sink->layPos == j->sink->layPos)
				continue;
			if (isCross(i->source->layPos, i->sink->layPos, j->source->layPos, j->sink->layPos))
			{
				i->cross--;
				if (i->cross + 1 == i->sink->maxCross)
					setNewMaxCross(i->sink);
				if (i->cross + 1 == maxCross && i->sink->maxCross != maxCross)
					maxCrossNode.erase(i->sink);

				if (i->cross + 1 == i->source->maxCross)
					setNewMaxCross(i->source);
				if (i->cross + 1 == maxCross && i->source->maxCross != maxCross)
					maxCrossNode.erase(i->source);

				j->cross--;
				if (j->cross + 1 == j->sink->maxCross)
					setNewMaxCross(j->sink);
				if (j->cross + 1 == maxCross && j->sink->maxCross != maxCross)
					maxCrossNode.erase(j->sink);

				if (j->cross + 1 == j->source->maxCross)
					setNewMaxCross(j->source);
				if (j->cross + 1 == maxCross && j->source->maxCross != maxCross)
					maxCrossNode.erase(j->source);
			}
			else {
				i->cross++;
				if (i->cross > i->sink->maxCross)
					i->sink->maxCross++;
				if (i->cross > maxCross)
				{
					maxCross = i->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(i->sink);
				}
				if (i->cross == maxCross)
					maxCrossNode.insert(i->sink);

				if (i->cross > i->source->maxCross)
					i->source->maxCross++;
				if (i->cross > maxCross)
				{
					maxCross = i->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(i->source);
				}
				if (i->cross == maxCross)
					maxCrossNode.insert(i->source);

				j->cross++;
				if (j->cross > j->sink->maxCross)
					j->sink->maxCross++;
				if (j->cross > maxCross)
				{
					maxCross = j->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(j->sink);
				}
				if (j->cross == maxCross)
					maxCrossNode.insert(j->sink);

				if (j->cross > j->source->maxCross)
					j->source->maxCross++;
				if (j->cross > maxCross)
				{
					maxCross = j->cross;
					maxCrossNode.clear();
					maxCrossNode.insert(j->source);
				}
				if (j->cross == maxCross)
					maxCrossNode.insert(j->source);
			}
		}
	}
}
auto MMACsolver::makeMove(Move move)->void {
	age[move.node->ID] = iterator;
	int layerId = move.node->layerID;
	int old_pos = move.node->layPos;
	if (move.newPos > move.node->layPos)
	{
		auto temp = move.node;
		for (int i = move.node->layPos; i <= move.newPos - 1; i++)
		{
			updateCrossAfterSwap(move.node, layer[layerId][i + 1]);
			layer[layerId][i] = layer[layerId][i + 1];
			layer[layerId][i]->layPos--;
		}
		layer[layerId][move.newPos] = temp;
		temp->layPos = move.newPos;
	}

	else if (move.newPos < move.node->layPos)
	{
		auto temp = move.node;
		for (int i = move.node->layPos; i >= move.newPos + 1; i--)
		{
			updateCrossAfterSwap(move.node, layer[layerId][i - 1]);
			layer[layerId][i] = layer[layerId][i - 1];
			layer[layerId][i]->layPos++;
		}
		layer[layerId][move.newPos] = temp;
		temp->layPos = move.newPos;
	}
}
auto MMACsolver::getMaxCrossNode()->void {
	maxCross = 0;
	for (auto& i : allNode)
	{
		if (i.maxCross > maxCross)
		{
			maxCrossNode.clear();
			maxCross = i.maxCross;
			maxCrossNode.insert(&i);
		}
		else if (i.maxCross == maxCross)
		{
			maxCrossNode.insert(&i);
		}
	}
}
auto MMACsolver::findMove()->Move {
	Move bestMv = Move(nullptr, -1, INT32_MAX);
	rd_MOVE_2 = 2;
	int count = 2;
	/*if (gen() % 20)*/
	for (auto p : maxCrossNode)
	{
		Node* n = p;
		int moveMaxDistance = layer[n->layerID].size() / Short_k;
		int lbIndex = max(0, n->layPos - moveMaxDistance);
		int ubIndex = min(p->layPos + moveMaxDistance, int(layer[p->layerID].size()) - 1);

		setTmpCross(lbIndex, ubIndex, p->layerID);
		tryMoveNeg2(n, lbIndex, count, bestMv.delta, bestMv);

		setTmpCross(p->layPos, p->layPos, p->layerID);
		tryMovePos2(n, ubIndex, count, bestMv.delta, bestMv);
	}

	if (bestMv.delta >= 0)
	{
		bestMv = Move(nullptr, -1, INT32_MAX);
		for (int i = 0; i < maxCrossNode.size(); i++)
		{

			int rd = gen() % allNode.size();
			pair<int, int>p = { allNode[rd].layerID,allNode[rd].layPos };

			Node* n = layer[p.first][p.second];

			int moveMaxDistance = layer[n->layerID].size()/Long_k.second*Long_k.first;

			int lbIndex = max(0, p.second - moveMaxDistance);
			int ubIndex = min(p.second + moveMaxDistance, int(layer[p.first].size()) - 1);
			setTmpCross(lbIndex, ubIndex, p.first);

			tryMoveNeg(n, lbIndex, count, bestMv.delta, bestMv);
			setTmpCross(p.second, p.second, p.first);

			tryMovePos(n, ubIndex, count, bestMv.delta, bestMv);
		}
		for (auto i : maxCrossNode)
		{

			Node* n = i;

			/*int moveMaxDistance = log(layer[n->layerID].size() + 1);*/

			int moveMaxDistance = layer[n->layerID].size() / Short_k;
			/*moveMaxDistance = max(2, moveMaxDistance);*/
			pair<int, int>p = { n->layerID,n->layPos };
			int lbIndex = max(0, p.second - moveMaxDistance);
			int ubIndex = min(p.second + moveMaxDistance, int(layer[p.first].size()) - 1);
			setTmpCross(lbIndex, ubIndex, p.first);

			tryMoveNeg(n, lbIndex, count, bestMv.delta, bestMv);
			setTmpCross(p.second, p.second, p.first);

			tryMovePos(n, ubIndex, count, bestMv.delta, bestMv);
		}
	}
	/*if (bestMv.newPos != n2 || bestMv.node != n1)*/
	//if (MOVE_2.delta < bestMv.delta)
	//	bestMv = bestMv3;
	/*if (bestMv.newPos != -1)
		Tabu[bestMv.node->ID][bestMv.newPos] = iterator + gen()%10 ;*/

	return bestMv;
}
auto MMACsolver::solve()->int {
	init();
	best_solution = min(maxCross, best_solution);
	clock_t start_time = clock();
	clock_t curr_time;
	int time = 1;
	bool fl = false;
	iterator = 1;
	while (iterator++)
	{
		int x, y;
		/*check();*/
		/*int b=calcMaxCrossEdgeNum();*/
		Move a = findMove();
		int gg = maxCross;
		if (a.newPos != -1)
			makeMove(a);
		/*int c= calcMaxCrossEdgeNum();
		if (maxCrossNode.size() != 0 && a.delta < 0 && b + a.delta != c)
			cout << "wrong";
		if (maxCrossNode.size() != 0 && a.delta >1000 && gg==maxCross)
			cout << "wrong";*/
			/*check();*/
		if (maxCrossNode.size() == 0)
		{

			getMaxCrossNode();
			if (maxCross < best_solution || maxCross == best_solution)
			{
				recordState();
				restor_strength = 1;
				fl = true;
				/*min_restore++;*/
				/* res_iterator = iterator-min_restore-50;*/
				res_iterator = iterator;
				best_solution = min(maxCross, best_solution);
				if (best_solution == 0)
				{
					cout << best_solution << endl;
					return 0;
				}
			}



		}
		if (iterator - res_iterator > min_restore /** pow(5, restor_strength)*/ && fl)
		{
			res_iterator = iterator;
			/*restor_strength = restor_strength %4 +1;*/
			restoreState();
		}
		if (iterator % 10000 == 0)
		{
			curr_time = clock();
			int now_time = (curr_time - start_time) / CLOCKS_PER_SEC;
			if (now_time > time)
			{

				/*	cout << "#iterator: " << iterator << " #maxCross: " << maxCross << " #maxCrossNode: " << maxCrossNode.size() << " #best_solution " << best_solution <<
						" #time: " << time << " #res_strengrh " << restor_strength << " num_1 " << num_1 << endl;*/

				time = now_time;
				if (time >= 60)
				{
					cout << best_solution << endl;
					return 0;
				}
			}
		}
	}
	return 0;
}
auto MMACsolver::check() -> bool {
	for (const Node& node : allNode) {
		int layer_id = node.layerID;
		int pos = node.layPos;
		if (layer_id < 0 || layer_id >= layer.size() ||
			pos < 0 || pos >= layer[layer_id].size() ||
			layer[layer_id][pos] != &node) {
			cerr << "Node " << node.ID << " position mismatch in layer." << endl;
			return false;
		}
	}

	for (int i = 0; i < layer.size(); ++i) {
		int expected_size = layerBound[i].second - layerBound[i].first;
		if (layer[i].size() != expected_size) {
			cerr << "Layer " << i << " size mismatch." << endl;
			return false;
		}
	}

	for (int i = 0; i < layer.size(); ++i) {
		int start = layerBound[i].first;
		int end = layerBound[i].second;
		for (Node* node : layer[i]) {
			if (node->ID < start || node->ID >= end) {
				cerr << "Node " << node->ID << " out of layer bound." << endl;
				return false;
			}
		}
	}

	for (const Node& node : allNode) {
		for (Edge* edge : node.outEdge) {
			if (edge->source != &node) {
				cerr << "Out edge source error at node " << node.ID << endl;
				return false;
			}
		}
		for (Edge* edge : node.inEdge) {
			if (edge->sink != &node) {
				cerr << "In edge sink error at node " << node.ID << endl;
				return false;
			}
		}
	}

	int current_max = maxCross;
	set<pair<int, int>> computed_max_nodes;
	for (int i = 0; i < layer.size(); ++i) {
		for (int j = 0; j < layer[i].size(); ++j) {
			Node* node = layer[i][j];
			current_max = max(current_max, node->maxCross);
			if (node->maxCross == current_max) {
				computed_max_nodes.insert({ i, j });
			}
		}
	}
	if (current_max != maxCross) {
		cerr << "Max cross value mismatch." << endl;
		return false;
	}
	for (auto i : maxCrossNode)
	{
		if (!computed_max_nodes.count({ i->layerID,i->layPos })) {
			cerr << "Max cross nodes mismatch." << endl;
			return false;
		}

	}

	vector<int> temp_cross(allEdge.size(), 0);
	for (int i = 0; i < layer.size(); ++i) {
		const auto& curr_layer = layer[i];
		for (int j = 0; j < curr_layer.size(); ++j) {
			Node* node_j = curr_layer[j];
			for (Edge* edge_j : node_j->outEdge) {
				for (int l = j + 1; l < curr_layer.size(); ++l) {
					Node* node_l = curr_layer[l];
					for (Edge* edge_l : node_l->outEdge) {
						int cross_val = isCross(j, edge_j->sink->layPos, l, edge_l->sink->layPos);
						temp_cross[edge_j - &allEdge[0]] += cross_val;
						temp_cross[edge_l - &allEdge[0]] += cross_val;
					}
				}
			}
		}
	}
	for (size_t i = 0; i < allEdge.size(); ++i) {
		if (allEdge[i].cross != temp_cross[i]) {
			cerr << "Edge cross count mismatch at edge " << i << endl;
			return false;
		}
	}

	for (int i = 0; i < layer.size(); ++i) {
		for (int j = 0; j < layer[i].size(); ++j) {
			if (layer[i][j]->layPos != j) {
				cerr << "Layer position mismatch at node " << layer[i][j]->ID << endl;
				return false;
			}
		}
	}

	return true;
}