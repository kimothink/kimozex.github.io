---
title: "Part01 C++로의 전환"
layout: post
date: 2018-05-10 0:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- CPlusPlus
- Coding
star: false
category: blog
author: john Ko
description: Coding
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





