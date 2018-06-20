---
title: "[열혈강의]Part01_Chapter 01 C++로의 전환"
layout: post
date: 2018-05-10 00:44
image: /assets/images/markdown.jpg
headerImage: true
tag:
- CPlusPlus
star: flase
category: blog
author: john Ko
description: Fundamental Programming
---
## Title : Part03 객체지향의 전개

### Chapter 07 상속(Inheritance)이해 

**07-01 상속에 들어가기에 앞서**

* 상속은 클래스를 재활용하기 위해 사용하는것은 아닙니다.
* 좋은 코드는 요구사항의 변경 및 기능의 추가에 따른 변경이 최소화되어야 한다. 그리고 이를 위해 해결책을 상속이 사용된다.



**08-02가상함수 **

```c++
#include<iostream>
class A
{
public: 
	void fct()
	{
		std::cout << "A Class Hello World" << std::endl;
	}
};

class B : public A
{
public : 
	void fct()
	{
		std::cout << "B Class Hello World" << std::endl;
	}
};

int main()
{
	B *b = new B();//OK
	A *a = new B; //OK
	B *c = new A();//Error

	b->fct();
	a->fct();
	return 0;
}
```

c++ 컴파일러는 포인터 연산의 기능성 여부를 판단할때 포인터의 자료형을 기준으로 판단하지 실제 가리키는 객체의 자료 형을 기준으로 판단하지 않는다.

**함수의 오버라이딩과 포인터 형**

* 함수를 호출할때 사용이 된 포인터의 형에 따라서 호출되는 함수가 결정된다.
* 오버라이딩 된 함수가 virtual 이면 오버랑이딩 한 함수도 자동 virtual
* 
* 버츄얼키워드를 사용하면 마지막으로 오버라이딩 된 함수가 호출된다.