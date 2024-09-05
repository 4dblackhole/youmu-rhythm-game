#pragma once

template <typename NodeType>
struct Node
{
public:
	NodeType* pLeft = nullptr, * pRight = nullptr;

	//enable when T is base of Node class
	void LeftRotate()
	{
		if (this == nullptr) return;

		NodeType* const newNode = this->pRight;
		if (newNode == nullptr) return;
		NodeType* const temp = newNode->pLeft;

		//rotate
		newNode->pLeft = this;
		this->pRight = temp;

		this = newNode;
	}

	void RightRotate()
	{
		if (this == nullptr) return;
		
		NodeType* const newNode = this->pLeft;
		if (newNode == nullptr) return;
		NodeType* const temp = newNode->pRight;

		//rotate
		newNode->pRight = this;
		this->pLeft = temp;

		this = newNode;
	}

};

template <typename NodeType>
struct SplayNode : public Node<NodeType>
{
public:
	NodeType* pParent = nullptr;
};