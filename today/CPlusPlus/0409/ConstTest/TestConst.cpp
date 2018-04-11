#include<iostream>

class test
{
public:
	test();
	~test();
	test(int _x,int _y):x(_x),y(_y)
	{}

private:

	int x, y;

};

test::test()
{
}

test::~test()
{
}