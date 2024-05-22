#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
struct NodesList;
struct Quadrant
{
	float XLB;
	float XUB;
	float YLB;
	float YUB;
	Quadrant();
	Quadrant(float xlb, float xub, float ylb, float yub);
	void QuadSet(float xlb = -INFINITY, float xub = INFINITY, float ylb = -INFINITY, float yub = INFINITY);
};
class QuadTreeNodeType
{
	bool exist;
	int depth;
	int xval;
	int yval;
	QuadTreeNodeType* father;
	QuadTreeNodeType* NW;
	QuadTreeNodeType* NE;
	QuadTreeNodeType* SW;
	QuadTreeNodeType* SE;
	Quadrant quadrant;
public:
	QuadTreeNodeType();
	QuadTreeNodeType(int x, int y);
	QuadTreeNodeType(const QuadTreeNodeType& Object);
	~QuadTreeNodeType();
	// �������
	bool Place(const QuadTreeNodeType& Node);
	// ��������� �������
	bool RePlace(const QuadTreeNodeType& Node);
	// �����
	QuadTreeNodeType* Search(const QuadTreeNodeType& Node);
	// ��������
	void DeleteNode(const QuadTreeNodeType& Node);
	//����� �� ������ �������
	void AreaSearch(QuadTreeNodeType* Node, int X, int Y, int R, NodesList* Result);
	// �������������� ������
	// ����� ��������� ��� ������
	QuadTreeNodeType* Candidate(int quadrant);
	// ������������� ����������
	float Manhattan(const QuadTreeNodeType* Node1, const QuadTreeNodeType* Node2);
	// ADJQUAD
	void ADJQUAD(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking);
	// NEWROOT
	void NEWROOT(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking);
	// �������� �� ����
	bool IsLeaf();
	// ������ ���������, � ������� ��������� ������
	int Quad(const QuadTreeNodeType& Node);
	// ������ ���������������� ���������
	int OpQuad(int quad);
	// ����� ������������� ����
	QuadTreeNodeType* SearchSon(int number);
	// ������� ������������� ����
	void SetSon(int quad, QuadTreeNodeType* Node);
	// �������� ������� ��������� ��� ���������
	void UpdateQuadrant();
	// ������ ������������� ����
	void PrintSon(int number);
	// ������ ������
	friend std::ostream& operator<<(std::ostream&, QuadTreeNodeType* Object);
	// ��������� ���� �� �������� ���������
	bool operator==(const QuadTreeNodeType& Object);
	// ��������� ����� �� �������� ��������� ������������ �����-�� �����
	bool Less(const QuadTreeNodeType& Object, int X, int Y);
	// ���������� ������ �� �����
	void ReadCommandsFromFile(const char* filename);
};
// ���������, � �������� �� ���� �����
bool IsInRange(int x, int first_border, int second_border);
// ��������, ������ �� ����� � ����
bool PointInCircle(int x, int y, int X0, int Y0, int R);
// ��������, ���������� �� ���� �������� ��������
bool CircleIntersectsQuadrant(int X0, int Y0, int R, Quadrant Quad);
struct NodesList
{
	QuadTreeNodeType* current_node;
	NodesList* father;
	NodesList* son;
	void Push(QuadTreeNodeType* Node);
	bool operator+=(const NodesList& Object);
	NodesList();
	NodesList(const NodesList& Object);
	~NodesList();
};