#include "QuadTreeNodeType.h"

int main()
{
	setlocale(0, "russian");
	QuadTreeNodeType Tree;
	Tree.ReadCommandsFromFile("Commands.txt");
	//Tree.~QuadTreeNodeType();
	return 0;
}