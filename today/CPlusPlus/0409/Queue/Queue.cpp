#include<iostream>

template <typename T>
class Queue
{
public:
	Queue();
	~Queue();

private:
	struct Node
	{
		T data;
		Node* next;

		Node(T data, Node* next)
		{
			this->data = data;
			this->next = next;
		}

	};

};

Queue::Queue()
{
}

Queue::~Queue()
{
}

int main()
{
	return 0;
}