#include "QuadTreeNodeType.h"

int main()
{
	QuadTreeNodeType Tree;
	Tree.ReadCommandsFromFile("Commands.txt");
	Tree.~QuadTreeNodeType();
	return 0;
}