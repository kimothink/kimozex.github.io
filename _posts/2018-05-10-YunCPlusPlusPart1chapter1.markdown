---
title: "[Yun_CPlusPlus_Study]C++로의 전환"
layout: post
date: 2018-05-10 00:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- CPlusPlus
star: true
category: blog
author: john Ko
description: Fundamental Programming
---
## Title : Part01 C++로의 전환

### Chapter 01 C언어 기반의 C++ 1 

**01-1 printf와 scanf를 대신하는 입출력 방식**

* C의 Printf는 C++에서는 cout(consol output)으로 변경되었으며 scnaf는 cin(consol input)으로 변경 되었습니다.
* C에서의 입력 출력시 타입에 맞는 별도의 서식 문자(%d,%f,%c)가 필요하였지만 C++에서는 별도의 서식 지정이 불필요합니다. 
* C++에서는 변수의 선언 위치에 제한을 두지 않습니다.

 ```c++
#include<iostream> //헤더 파일의 선언

int main()
{
	int num = 20;
	//cout = consol output의 약자
	//출력의 기본구성            //개행의 진행
	std::cout << "Hello World" << std::endl;
	std::cout << "Hello " << "World" << std::endl;
	std::cout << num << ' ' << 'A';
	std::cout << ' ' << 3.14 << std::endl;
	//cin = consol input의 약자
	int val1 = 0;
	std::cout << "첫번째 숫자를 입력";
	std::cin >> val1;
	int val2 = 0;
	std::cout << "두번째 숫자를 입력";
	std::cin >> val2;
	int result = val1 + val2;
	std::cout << "덧셈 결과 : " << result << std::endl;
    
	return 0;
}
 ```

**01-2 함수 오버로딩(Function Overloading)**

- C언어의 경우 함수를 호출할 경우 함수의 이름에만 의존하여  호출할 함수를 결정을 함으로써 오류를 발생시키지만 C++은 함수 호출시 함수 이름과 전달되는 인자의 정보(Type)를 참조하여 호출함수를 결정하는것을 함수 오버로딩이라고합니다. 

- 오버로딩이 성립되는 예)

  |              경우               | 오버로딩 성립 |
  | :-----------------------------: | ------------- |
  | 매개 변수의 자료 형이 다를 경우 | True          |
  |   매개 변수의 수가 다를 경우    | True          |
  |    반환형의 타입이 다를 경우    | False         |
  |         레퍼런스의 타입         | True          |
  |           const 유무            | False         |

```c++
#include<iostream>
void Myfun(int num);  //function signature
void Myfun(int a, int b); //function signature

int main()
{
	Myfun(20);
	Myfun(30, 40);
    return 0;
}

void Myfun(int num) //Myfun(20)호출했을 경우
{
	std::cout << num << std::endl;
}


void Myfun(int num1 ,int num2) //Myfun(30, 40) 호출 했을 경우 
{
	std::cout << num1 << ' ' <<num2<< std::endl;
}

```



**01-3 매개변수의 디폴트 값**

- 함수를 정의할때 인자 값을 정의 할 수 있습니다.

- 매개 변수는 왼쪽 파라미터 부터 정의 된다.

- 함수의 선언을 별도로 둘때에는 디폴트 값의 선언을 함수의 선언부에 위치 시켜야한다.

  (컴파일러는 함수의 디폴트 값의 지정여부를 알아야 함수의 호출 문장을 적절히 컴파일 할수 있다)

- 전달되는 인자가 왼쪽에서부터 채워지므로 , 함수의 디폴트 인자 값은 오른쪽에서부터 채워져야 한다. 왼쪽부터 채워지면 컴파일 에러가 생긴다.

```c++
#include<iostream>

void Myfun(int a, int b,int c=20); //함수2 선언 (디폴트 값은 오른쪽에서부터 채워져야한다)

void Myfun(int num=10) //함수1
{
	std::cout << num << std::endl;
}

int main()
{
	Myfun(); //함수1 호출
	Myfun(20);//함수1 호출
	Myfun(30, 40); //함수2 호출
    return 0;
}

void Myfun(int num1 ,int num2 ,int num3) //함수2
{
	std::cout << num1 << ' ' <<num2<< std::endl;
}
```



**01-4 인라인(inline)함수**

**매크로 함수**  

컴파일이 되기전 선행처리기에 통하여 작성되는 함수 

장점 : 함수가 인라인화 되어 성능의 향상으로 이어질수 있다.(스택에 할당을 받지 않는다.)

단점 : 함수의 정의 방식이 일반함수에 비해서 복잡하다. 복잡한 함수는 정의할수 없다. 

​         



**인라인 함수**

매크로 함수의 장점을 갖으며 단점을 보완한 것이 C++의 인라인 함수 이다.

인라인 함수 선언은 컴파일러에 의해서 처리되며 컴파일러가 인라인화를 결정합니다. 그러므로

인라인(inline)선언을 하여도 인라인 처리 되지 않을 수 있고, 인라인(inline) 선언이 없어도 인라인 처리

될 수 있습니다.

```c++
#include <iostream>

inline int SQUARE(int x)
{
	return x * x;
}

int main()
{
	std::cout << SQUARE(6) << std::endl;

    return 0;
}

```



인라인에 비해 매크로는 자료형이 독립점입니다. 하지만 매크로는 디버깅을 하기에는 힘듭니다.



**01-5 이름공간(namespace)에 대한 소개**

* 이름 같음으로 인해서 이름충돌이 생기는 것을 막기위해서 namespace를 작성한다.

* 존재하는 이름공간이 다르면 동일한 이름의 함수 및 변수를 선언하는 것이 가능하다.

* 이름공간(namespace)을 사용할때에는 ::(범위 지정 연산자) 을 사용한다.

* 선언된 이름공간의 이름이 동일 하다면 이둘은 동일한 이름 공간으로 간주한다.

* 이름 공간은 중첩이 가능하다

  ```c++
  #include <iostream>
  
  namespace BestComImpl
  {
  	void SimpleFunc()
  	{
  		std::cout << "BestComImpl이 정의한 함수" << std::endl;
  	}
  }
  
  namespace ProgComImpl
  {
  	void SimpleFunc()
  	{
  		std::cout << "ProgComImpl이 정의한 함수" << std::endl;
  	}
  }
  
  int main()
  {
  	BestComImpl::SimpleFunc();
  	ProgComImpl::SimpleFunc();
  
  	return 0;
  }
  ```

* using을 이용한 이름공간을 명시 할 수 이쓰며 using namespace std 는 std에 선언된 것을 std라는 이름공간의 선언없이 접근하겠다라는 선언이다.

* 이름공간을 사용하여 별칭을 작성할수 있다. ex)namespace abc=aaa::bb::cc

**ADL**









