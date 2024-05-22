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
	// Вставка
	bool Place(const QuadTreeNodeType& Node);
	// Повторная вставка
	bool RePlace(const QuadTreeNodeType& Node);
	// Поиск
	QuadTreeNodeType* Search(const QuadTreeNodeType& Node);
	// Удаление
	void DeleteNode(const QuadTreeNodeType& Node);
	//Ответ на запрос области
	void AreaSearch(QuadTreeNodeType* Node, int X, int Y, int R, NodesList* Result);
	// Дополнительные методы
	// Поиск кандидата для замены
	QuadTreeNodeType* Candidate(int quadrant);
	// Манхеттенское расстояние
	float Manhattan(const QuadTreeNodeType* Node1, const QuadTreeNodeType* Node2);
	// ADJQUAD
	void ADJQUAD(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking);
	// NEWROOT
	void NEWROOT(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking);
	// Проверка на лист
	bool IsLeaf();
	// Выдача квадранта, в котором находится корень
	int Quad(const QuadTreeNodeType& Node);
	// Выдача противоположного квадранта
	int OpQuad(int quad);
	// Поиск определенного сына
	QuadTreeNodeType* SearchSon(int number);
	// Вставка определенного сына
	void SetSon(int quad, QuadTreeNodeType* Node);
	// Обновить границы квадранта при удалениях
	void UpdateQuadrant();
	// Печать определенного сына
	void PrintSon(int number);
	// Печать дерева
	friend std::ostream& operator<<(std::ostream&, QuadTreeNodeType* Object);
	// Сравнение узла по значению координат
	bool operator==(const QuadTreeNodeType& Object);
	// Сравнение узлов по значению координат относительно какой-то точки
	bool Less(const QuadTreeNodeType& Object, int X, int Y);
	// Обработчик команд из файла
	void ReadCommandsFromFile(const char* filename);
};
// Проверить, в штрафной ли зоне число
bool IsInRange(int x, int first_border, int second_border);
// Проверка, входил ли точка в круг
bool PointInCircle(int x, int y, int X0, int Y0, int R);
// Проверка, пересекает ли круг заданный квадрант
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