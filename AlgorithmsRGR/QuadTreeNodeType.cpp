#include "QuadTreeNodeType.h"
QuadTreeNodeType::QuadTreeNodeType()
{
	xval = 0;
	yval = 0;
	depth = 0;
	exist = false;
	father = nullptr;
	NW = nullptr;
	NE = nullptr;
	SW = nullptr;
	SE = nullptr;
}
QuadTreeNodeType::QuadTreeNodeType(int x, int y)
{
	xval = x;
	yval = y;
	depth = 0;
	exist = true;
	father = nullptr;
	NW = nullptr;
	NE = nullptr;
	SW = nullptr;
	SE = nullptr;
	quadrant.QuadSet();
}
QuadTreeNodeType::QuadTreeNodeType(const QuadTreeNodeType& Object)
{
	xval = Object.xval;
	yval = Object.yval;
	exist = Object.exist;
	depth = Object.depth;
	father = Object.father;
	NW = Object.NW;
	NE = Object.NE;
	SW = Object.SW;
	SE = Object.SE;
	quadrant = Object.quadrant;
}
QuadTreeNodeType::~QuadTreeNodeType()
{
	xval = 0;
	yval = 0;
	exist = 0;
	quadrant.QuadSet();
	father = nullptr;
	if (NW != nullptr)
		NW->~QuadTreeNodeType();
	if (NE != nullptr)
		NE->~QuadTreeNodeType();
	if (SW != nullptr)
		SW->~QuadTreeNodeType();
	if (SE != nullptr)
		SE->~QuadTreeNodeType();
}
// Вставка
bool QuadTreeNodeType::Place(const QuadTreeNodeType& Node)
{
	QuadTreeNodeType* place = Search(Node);
	if (!place->exist)
	{
		place->exist = true;
		place->xval = Node.xval;
		place->yval = Node.yval;
		return true;
	}
	else
	{
		return false;
	}
}
// Повторная вставка целой ветви
bool QuadTreeNodeType::RePlace(const QuadTreeNodeType& Node)
{
	Place(Node);
	if (Node.NW != nullptr)
		RePlace(*Node.NW);
	if (Node.NE != nullptr)
		RePlace(*Node.NE);
	if (Node.SW != nullptr)
		RePlace(*Node.SW);
	if (Node.SE != nullptr)
		RePlace(*Node.SE);
	return true;
}
// Поиск
QuadTreeNodeType* QuadTreeNodeType::Search(const QuadTreeNodeType& Node)
{
	QuadTreeNodeType* currentNode = this;
	int x = Node.xval;
	int y = Node.yval;
	while(true)
	{
		if (*currentNode == Node || !currentNode->exist)
			return currentNode;
		if (x < currentNode->xval)
		{
			if (y < currentNode->yval)
			{
				currentNode = currentNode->SearchSon(2);
			}
			else
			{
				currentNode = currentNode->SearchSon(0);
			}
		}
		else
		{
			if (y < currentNode->yval)
			{
				currentNode = currentNode->SearchSon(3);
			}
			else
			{
				currentNode = currentNode->SearchSon(1);
			}
		}
	}
}
// Удаление
void QuadTreeNodeType::DeleteNode(const QuadTreeNodeType& Node)
{
	QuadTreeNodeType* node = Search(Node);
	if (node->IsLeaf())
	{
		if (node->father != nullptr)
		{
			if (*node->father == *this)
				this->SetSon(Quad(*node), nullptr);
			else
				node->father->SetSon(node->father->Quad(*node), nullptr);
			delete node;
		}
		else this->~QuadTreeNodeType();
		return;
	}
	QuadTreeNodeType* new_root = nullptr;
	QuadTreeNodeType* temp = nullptr;
	QuadTreeNodeType* candidate[4] = { node->Candidate(0),node->Candidate(1),node->Candidate(2),node->Candidate(3) };
	float wins[4] = { 0 ,0 ,0 ,0 };
	float min_manhattan = INFINITY;
	int quad = 0;
	int opquad = 0;
	for (int i = 0; i < 4; i++)
	{
		if (candidate[i] == nullptr) 
			wins[i] = -1;
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			if (j != OpQuad(i))
			{
				if (candidate[j] != nullptr && candidate[i]->Less(*candidate[j], node->xval, node->yval))
					wins[i]++;
				else
					wins[j]++;
			}
		}
	}
	for (int i = 0; i < 2; i++)
	{
		if (wins[i] == 2)
		{
			if (wins[OpQuad(i)] == 2 && Manhattan(node, candidate[i]) > Manhattan(node, candidate[OpQuad(i)]))
			{
				new_root = candidate[OpQuad(i)];
			}
			else new_root = candidate[i];
		}
	}
	if (new_root == nullptr)
	{
		for (int i = 0; i < 4; i++)
		{
			if (wins[i] != -1)
			{
				wins[i] = Manhattan(node, candidate[i]);
				if (wins[i] < min_manhattan)
				{
					min_manhattan = wins[i];
					new_root = candidate[i];
				}
			}
		}
	}
	new_root->father = node->father;
	new_root->quadrant = node->quadrant;
	new_root->depth = node->depth;
	quad = node->Quad(*new_root);
	opquad = OpQuad(quad);
	// Привязываем противоположный квадрант так как он не меняется
	for (int i = 0; i < 4; i++)
	{
		if (i == opquad)
		{
			new_root->SetSon(i, node->SearchSon(i));
			break;
		}
	}
	NEWROOT(node, new_root, node->SearchSon(quad));
	for (int i = 0; i < 4; i++)
	{
		if (i != quad && i != opquad)
		{
			temp = node->SearchSon(i);
			if (temp->exist == 0)
			{
				node->SetSon(i, nullptr);
				delete temp;
			}
			else
			{
				new_root->RePlace(*temp);
			}
		}
	}
	*node = *new_root;
}
// Ответ на запрос области
void QuadTreeNodeType::AreaSearch(QuadTreeNodeType* Node, int X, int Y, int R, NodesList* Result)
{
	if (PointInCircle(Node->xval, Node->yval, X, Y, R))
		Result->Push(Node);
	if (Node->NW != nullptr && CircleIntersectsQuadrant(X, Y, R, Node->NW->quadrant))
		AreaSearch(Node->NW, X, Y, R, Result);
	if (Node->NE != nullptr && CircleIntersectsQuadrant(X, Y, R, Node->NE->quadrant))
		AreaSearch(Node->NE, X, Y, R, Result);
	if (Node->SW != nullptr && CircleIntersectsQuadrant(X, Y, R, Node->SW->quadrant))
		AreaSearch(Node->SW, X, Y, R, Result);
	if (Node->SE != nullptr && CircleIntersectsQuadrant(X, Y, R, Node->SE->quadrant))
		AreaSearch(Node->SE, X, Y, R, Result);
}
// Проверка, входил ли точка в круг
bool PointInCircle(int x, int y, int X0, int Y0, int R)
{
	int dx = x - X0;
	int dy = y - Y0;
	return dx * dx + dy * dy <= R * R;
}
// Проверка, пересекает ли круг заданный квадрант
bool CircleIntersectsQuadrant(int X0, int Y0, int R, Quadrant Quad) 
{
	if (X0 >= Quad.XLB && X0 <= Quad.XUB && Y0 >= Quad.YLB && Y0 <= Quad.YUB) return true;
	int closestX = std::max((int)Quad.XLB, std::min(X0, (int)Quad.XUB));
	int closestY = std::max((int)Quad.YLB, std::min(Y0, (int)Quad.YUB));
	int dx = closestX - X0;
	int dy = closestY - Y0;
	return dx * dx + dy * dy <= R * R;
}
// Поиск кандидата для замены
QuadTreeNodeType* QuadTreeNodeType::Candidate(int quadrant)
{
	QuadTreeNodeType* result = this;
	int new_quad = 0;
	switch (quadrant)
	{
	case 0:
	{
		result = NW;
		new_quad = 3;
		break;
	}
	case 1:
	{
		result = NE;
		new_quad = 2;
		break;
	}
	case 2:
	{
		result = SW;
		new_quad = 1;
		break;
	}
	case 3:
	{
		result = SE;
		new_quad = 0;
		break;
	}
	default:
		exit(-1);
	}
	while (result != nullptr)
	{
		result = result->SearchSon(new_quad);
		if (result->exist == 0)
		{
			result->father->SetSon(new_quad, nullptr);
			result = result->father;
			break;
		}
	}
	return result;
}
// Манхеттенское расстояние
float QuadTreeNodeType::Manhattan(const QuadTreeNodeType* Node1, const QuadTreeNodeType* Node2)
{
	if (Node1 == nullptr || Node2 == nullptr) 
	{
		return INFINITY;
	}
	return (float)(abs(Node1->xval - Node2->xval) + abs(Node1->yval - Node2->yval));
}
// ADJQUAD
void QuadTreeNodeType::ADJQUAD(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking)
{
	int quad = Deleting_Node->Quad(*New_Root);
	int opquad = OpQuad(quad);
	QuadTreeNodeType* temp = nullptr;
	// Если корень четверти не лежит в штрафной зоне
	if (!IsInRange(Checking->xval, Deleting_Node->xval, New_Root->xval) && !IsInRange(Checking->yval, Deleting_Node->yval, New_Root->yval))
	{
		for (int i = 0; i < 4; i++)
		{
			if (i != quad && i != OpQuad(Checking->father->Quad(*Checking)))
			{
				temp = Checking->SearchSon(i);
				if (temp->exist == 0)
				{
					Checking->SetSon(i, nullptr);
					delete temp;
				}
				else
				{
					ADJQUAD(Deleting_Node, New_Root, temp);
				}
			}
		}
		Checking->UpdateQuadrant();
	}
	else
	{
		Checking->father->SetSon(Checking->father->Quad(*Checking), nullptr);
		New_Root->RePlace(*Checking);
	}
}
// NEWROOT
void QuadTreeNodeType::NEWROOT(QuadTreeNodeType* Deleting_Node, QuadTreeNodeType* New_Root, QuadTreeNodeType* Checking)
{
	int quad = Deleting_Node->Quad(*New_Root);
	int opquad = OpQuad(quad);
	QuadTreeNodeType* checking_node = Checking->SearchSon(opquad);
	QuadTreeNodeType* temp = nullptr;
	if (checking_node->exist == 0)
	{
		Checking->SetSon(opquad, nullptr);
	}
	else if (Checking != New_Root)
	{
		checking_node->SetSon(quad, Checking);
		Checking->SetSon(opquad, nullptr);
		for (int i = 0; i < 4; i++)
		{
			if (i != quad && i != opquad)
			{
				temp = Checking->SearchSon(i);
				if (temp->exist == 0)
				{
					Checking->SetSon(i, nullptr);
					delete temp;
				}
				else
				{
					ADJQUAD(Deleting_Node, New_Root, temp);
				}
			}
		}
		NEWROOT(Deleting_Node, New_Root, checking_node);
	}
}
// Проверка на лист
bool QuadTreeNodeType::IsLeaf()
{
	if (NW != nullptr) return false;
	if (NE != nullptr) return false;
	if (SW != nullptr) return false;
	if (SE != nullptr) return false;
	return true;
}
// Выдача квадранта, в котором находится корень
int QuadTreeNodeType::Quad(const QuadTreeNodeType& Node)
{
	if (Node.xval < xval)
	{
		if (Node.yval < yval) return 2;
		else return 0;
	}
	else
	{
		if (Node.yval < yval) return 3;
		else return 1;
	}
	return -1;
}
// Выдача противоположного квадранта
int QuadTreeNodeType::OpQuad(int quad)
{
	switch (quad)
	{
	case 0:
		return 3;
	case 1:
		return 2;
	case 2:
		return 1;
	case 3:
		return 0;
	default:
		exit(-1);
	}
}
// Поиск определенного сына
QuadTreeNodeType* QuadTreeNodeType::SearchSon(int number)
{
	switch (number)
	{
	case 0:
	{
		if (NW == nullptr)
		{
			NW = new QuadTreeNodeType();
			NW->depth = depth + 1;
			NW->father = this;
			NW->quadrant.QuadSet((float)this->quadrant.XLB, (float)this->xval, (float)this->yval, (float)this->quadrant.YUB);
		}
		return NW;
	}
	case 1:
	{
		if (NE == nullptr)
		{
			NE = new QuadTreeNodeType();
			NE->depth = depth + 1;
			NE->father = this;
			NE->quadrant.QuadSet((float)this->xval, (float)this->quadrant.XUB, (float)this->yval, (float)this->quadrant.YUB);
		}
		return NE;
	}
	case 2:
	{
		if (SW == nullptr)
		{
			SW = new QuadTreeNodeType();
			SW->depth = depth + 1;
			SW->father = this;
			SW->quadrant.QuadSet((float)this->quadrant.XLB, (float)this->xval, (float)this->quadrant.YLB, (float)this->yval);
		}
		return SW;
	}
	case 3:
	{
		if (SE == nullptr)
		{
			SE = new QuadTreeNodeType();
			SE->depth = depth + 1;
			SE->father = this;
			SE->quadrant.QuadSet((float)this->xval, (float)this->quadrant.XUB, (float)this->quadrant.YLB, (float)this->yval);
		}
		return SE;
	}
	default:
	{
		std::cout << "Illegal son!";
		exit(-1);
	}
	}
}
// Вставка определенного сына
void QuadTreeNodeType::SetSon(int quad, QuadTreeNodeType* Node)
{
	if (Node != nullptr)
	{
		Node->father = this;
	}
	switch (quad)
	{
	case 0:
	{
		NW = Node;
		if (Node != nullptr)
			NW->quadrant.QuadSet((float)this->quadrant.XLB, (float)this->xval, (float)this->yval, (float)this->quadrant.YUB);
		break;
	}
	case 1:
	{
		NE = Node;
		if (Node != nullptr)
			NE->quadrant.QuadSet((float)this->xval, (float)this->quadrant.XUB, (float)this->yval, (float)this->quadrant.YUB);
		break;
	}
	case 2:
	{
		SW = Node;
		if (Node != nullptr)
			SW->quadrant.QuadSet((float)this->quadrant.XLB, (float)this->xval, (float)this->quadrant.YLB, (float)this->yval);
		break;
	}
	case 3:
	{
		SE = Node;
		if (Node != nullptr)
			SE->quadrant.QuadSet((float)this->xval, (float)this->quadrant.XUB, (float)this->quadrant.YLB, (float)this->yval);
		break;
	}
	default:
		break;
	}
}
// Печать определенного сына
void QuadTreeNodeType::PrintSon(int number)
{
	switch (number)
	{
	case 0:
	{
		if (NW != nullptr)
		{
			for (int i = 0; i < depth; i++)
			{
				std::cout << "--";
			}
			std::cout << "NW--" << NW;
		}
		break;
	}
	case 1:
	{
		if (NE != nullptr)
		{
			for (int i = 0; i < depth; i++)
			{
				std::cout << "--";
			}
			std::cout << "NE--" << NE;
		}
		break;
	}
	case 2:
	{
		if (SW != nullptr)
		{
			for (int i = 0; i < depth; i++)
			{
				std::cout << "--";
			}
			std::cout << "SW--" << SW;
		}
		break;
	}
	case 3:
	{
		if (SE != nullptr)
		{
			for (int i = 0; i < depth; i++)
			{
				std::cout << "--";
			}
			std::cout << "SE--" << SE;
		}
		break;
	}
	default:
		break;
	}
}
// Обновить границы квадранта при удалениях
void QuadTreeNodeType::UpdateQuadrant()
{
	if (father == nullptr)
		quadrant.QuadSet();
	else
	{
		switch (father->Quad(*this))
		{
		case 0:
		{
			quadrant.QuadSet((float)father->quadrant.XLB, (float)father->xval, (float)father->yval, (float)father->quadrant.YUB);
			break;
		}
		case 1:
		{
			quadrant.QuadSet((float)father->xval, (float)father->quadrant.XUB, (float)father->yval, (float)father->quadrant.YUB);
			break;
		}
		case 2:
		{
			quadrant.QuadSet((float)father->quadrant.XLB, (float)father->xval, (float)father->quadrant.YLB, (float)father->yval);
			break;
		}
		case 3:
		{
			quadrant.QuadSet((float)father->xval, (float)father->quadrant.XUB, (float)father->quadrant.YLB, (float)father->yval);
			break;
		}
		default:
			break;
		}
	}
}
//Печать дерева
std::ostream& operator<<(std::ostream& ostream, QuadTreeNodeType* Object)
{
	if (Object->exist == 0)
	{
		std::cout << "NONE";
		return ostream;
	}
	std::cout << "(" << Object->xval << "," << Object->yval << ")";
	//std::cout << ", border: " << Object->quadrant.XLB << ", " << Object->quadrant.XUB << ", " << Object->quadrant.YLB << ", " << Object->quadrant.YUB;
	std::cout << std::endl;
	for (int i = 0; i < 4; i++)
		Object->PrintSon(i);
	return ostream;
}
// Проверить, в штрафной ли зоне число
bool IsInRange(int x, int first_border, int second_border)
{
	if (first_border >= second_border)
	{
		if (x >= second_border && x <= first_border) return true;
	}
	else
	{
		if (x >= first_border && x <= second_border) return true;
	}
	return false;
}
// Сравнение узла по значению координат
bool QuadTreeNodeType::operator==(const QuadTreeNodeType& Object)
{
	if (xval != Object.xval) return false;
	if (yval != Object.yval) return false;
	return true;
}
// Сравнение узлов по значению координат относительно какой-то точки
bool QuadTreeNodeType::Less(const QuadTreeNodeType& Object, int X, int Y)
{
	if (!this) return false;
	double distance_1 = std::sqrt((xval - X) * (xval - X) + (yval - Y) * (yval - Y));
	double distance_2 = std::sqrt((Object.xval - X) * (Object.xval - X) + (Object.yval - Y) * (Object.yval - Y));
	if (distance_1 > distance_2) return false;
	return true;
}
// Обработчик команд из файла
void  QuadTreeNodeType::ReadCommandsFromFile(const char* filename)
{
	std::ifstream file(filename);
	std::string current_string;
	QuadTreeNodeType* temp_node;
	NodesList temp_list;
	int x;
	int y;
	int R;
	if (file.is_open())
	{
		while (file >> current_string)
		{
			if (current_string == "place")
			{
				file >> x >> y;
				if (Place(QuadTreeNodeType(x, y)))
					std::cout << "placed " << x << " " << y << std::endl;
				else
					std::cout << "tree already have a node " << x << " " << y << std::endl;
			}
			else if (current_string == "search")
			{
				file >> x >> y;
				temp_node = Search(QuadTreeNodeType(x, y));
				std::cout << "Searching" << std::endl;
				if (temp_node->exist == 1)
					std::cout << "found (" << temp_node->xval << ", " << temp_node->yval << ")" << std::endl << std::endl;
				else
				{
					std::cout << "(" << x << ", " << y << ") - no such node!" << std::endl << std::endl;
					if (temp_node->father != nullptr)
						temp_node->father->SetSon(temp_node->father->Quad(*temp_node), nullptr);
				}
			}
			else if (current_string == "delete")
			{
				file >> x >> y;
				DeleteNode(QuadTreeNodeType(x, y));
				std::cout << "deleted (" << x << ", " << y << ")" << std::endl;
			}
			else if (current_string == "area")
			{
				file >> x >> y >> R;
				AreaSearch(this, x, y, R, &temp_list);
				std::cout << "Area with center in (" << x << ", " << y << "), R = " << R << " includes nodes:" << std::endl;
				if (temp_list.current_node == nullptr)
					std::cout << "NONE" << std::endl;
				else
				{
					std::cout << "(" << temp_list.current_node->xval << ", " << temp_list.current_node->yval << ") ; ";
					NodesList* temp = &temp_list;
					while (temp->son != nullptr)
					{
						temp = temp->son;
						std::cout << "(" << temp->current_node->xval << ", " << temp->current_node->yval << ") ; ";
					}
					std::cout << std::endl;
				}
				temp_list.~NodesList();
			}
			else if (current_string == "print")
			{
				std::cout << "Tree: " << std::endl << this << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Не получилось открыть файл!";
		exit(-1);
	}
	file.close();
}
Quadrant::Quadrant()
{
	XLB = -INFINITY;
	XUB = INFINITY;
	YLB = -INFINITY;
	YUB = INFINITY;
}
Quadrant::Quadrant(float xlb, float xub, float ylb, float yub)
{
	XLB = xlb;
	XUB = xub;
	YLB = ylb;
	YUB = yub;
}
void Quadrant::QuadSet(float xlb, float xub, float ylb, float yub)
{
	XLB = xlb;
	XUB = xub;
	YLB = ylb;
	YUB = yub;
}
void NodesList::Push(QuadTreeNodeType* Node)
{
	if (current_node == nullptr)
	{
		current_node = Node;
	}
	else
	{
		NodesList* temp = son;
		NodesList* last = this;
		while (temp != nullptr)
		{
			last = temp;
			temp = temp->son;
		}
		last->son = new NodesList;
		last->son->current_node = Node;
		last->son->father = last;
	}
}
bool NodesList::operator+=(const NodesList& Object)
{
	if (Object.current_node == nullptr) return false;
	if (current_node == nullptr)
	{
		current_node = Object.current_node;
		father = Object.father;
		son = Object.son;
		return true;
	}
	NodesList* temp = son;
	NodesList* last = this;
	NodesList* temp_obj = Object.son;
	while (temp != nullptr)
	{
		last = temp;
		temp = temp->son;
	}
	temp = last;
	last->Push(Object.current_node);
	while (temp_obj != nullptr)
	{
		last = last->son;
		last->Push(temp_obj->current_node);
		temp_obj = temp_obj->son;
	}
	/*temp->son = new NodesList;
	last = temp;
	temp = temp->son;
	temp->current_node = Object.current_node;
	temp->father = last;
	temp->son = temp_obj;
	while (temp_obj != nullptr)
	{
		temp->son = new NodesList;
		last = temp;
		temp = temp->son;
		temp->current_node = Object.current_node;
		temp->father = last;
		temp->son = temp_obj;
		temp_obj = temp_obj->son;
	}*/
	return true;
}
NodesList::NodesList()
{
	current_node = nullptr;
	father = nullptr;
	son = nullptr;
}
NodesList::NodesList(const NodesList& Object)
{
	current_node = nullptr;
	father = nullptr;
	son = nullptr;
	if (Object.current_node == nullptr) return;
	current_node = Object.current_node;
	if (Object.father != nullptr)
		father = new NodesList(*Object.father);
	if (Object.son != nullptr)
		son = new NodesList(*Object.son);
}
NodesList::~NodesList()
{
	current_node = nullptr;
	NodesList* temp = son;
	NodesList* last = this;
	while (temp != nullptr)
	{
		last = temp;
		temp = temp->son;
	}
	temp = last;
	while (temp != this)
	{
		last = temp;
		temp = temp->father;
		temp->son = nullptr;
		delete last;
	}
}