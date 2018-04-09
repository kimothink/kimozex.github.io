#include<iostream>

class A
{
public:
	A();
	~A();

public : 
	void Test(void) {}
	void Test(void) const {}
	void Test2(void) &{}
	void Test2(void) const&{}
	void Test2(void) &&{}
	void Test2(void)const&&{
		std::cout << "Test2(void) const&&" << std::endl;
	}

private:
};



A::A()
{
}

A::~A()
{
}



int main()
{

	const A().Test2();
	const A b;
	std::move(b).Test2();
	system("PAUSE");
	return 0;

}