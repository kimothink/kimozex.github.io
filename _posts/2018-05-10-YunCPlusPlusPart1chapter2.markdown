---
title: "[열혈강의]Part01_Chapter 02 C++로의 전환"
layout: post
date: 2018-05-12 01:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- CPlusPlus
star: true
category: blog
author: john Ko
description: Fundamental Programming
---


## Title : Part01-02 C++로의 전환

### Chapter 02 C언어 기반의 C++ 2 

**Chapter 02의 시작에 앞서**

* const의 의미

  ```c++
  const int num=10
  //변수 num을 상수화!
  
  const int * ptr = &val1;
  //포인터 ptr1을 이용해서 val1의 값을 변경할 수 없음
  
  int * const ptr2=&val2;
  //포인터 ptr2가 상수화 됨
  
  const int* const ptr3=&val3;
  //포인터 ptr3가 상수화 되었으며,ptr3를 이용해서 val3의 값을 변경할 수 없음
  ```

  

* 실행중인 프로그램 메모리 공간

  

  |     영역      |                                                              |
  | :-----------: | :----------------------------------------------------------- |
  |    데이터     | 전역 변수가 저장되는 영역                                    |
  |     스택      | 지역변수 및 매개 변수가 저장되는 영역 / 함수호출을 위한 공간 |
  |      힙       | malloc 함수 호출에 의해 프로그램이 실행되는 과정에서 동적으로 할당이 이뤄지는 영역 |
  | malloc & free | malloc 함수 호출에 의해 할당된 메모리 공간은 free 함수 호출을 통해서 소멸하지 않으면 해제 되지 않는영역 |
  |      bss      | bss나 bss는 초기에 오직 제로 값으로 표시된 정적으로 할당된 변수가 포함된 데이터 세그먼트의 일부로 컴파일러나 링커에 의해  사용된다. |

  **새로운 자료 형 bool**

  * 참과 거짓을 데이터로 표현하기 위해 bool 타입이 생겼다.

  * 0 은 거짓을 말하면 0을 제외한 모든 숫자는 참으로 표현한다.

  * true는 '참'을 의미하는 1바이트 데이터이고 , false는 거짓을 의미하는 1바이트 데이터 이다. 이둘을 은 각각 1과 0이 아니다. 그러나 정수가 와야할 위치에 오게 되면 각각 1과 0으로 변환이 된다.

  * true와 false 정보를 저장할 수 있는 변수는 bool 형 변수이다.

    

  **참조자의 이해**

  * 기본 변수의 이름에 닉네임 혹은 새로운 이름(별칭)을 추가한다.

    ```c++
    int Peter=17;
    int &Spiderman=Peter;
    //변수가 생성시 & 참조자가 됩니다.
    ```

    참조자의 선언 가능 범위

    ```c++
    int &ref=20;
    //상수 대상으로 참조자 선언 불가능
    int &ref;
    //참조자는 생성과 동시에 누군가를 참조해야한다.
    int &ref=NULL
    //포인터처럼 NULL로 초기화하는 것도 불가능하다.
    
    int arr[3]={1,3,5};
    int &ref=arr[0];
    //변수의 성향을 지니는 대상이라면 참조자의 선언이 가능하다.
    
    int num=12;
    int *ptr=&num;
    int **dptr=&ptr;
    
    int &ref=num;
    int *(&pref)=ptr;
    int **(&dpref)=dptr;
    
    //포인터를 이용하여 참조자의 선언이 가능하다.
    ```

    **참조자의 이해**

    - call-by-value & call-by-reference 

      * call-by-value : 값을 전달하면서 호출 하게 되는 함수 함수외에 선언된 변수에는 접근 불가능 하다.
      * call-by-reference : 값은 값이되 주소 값을 전달 하면서 호출하게 되는 함수 인자로 전달 된 주소의 메모리 공간에 접근이 가능하다.

    - 외부에 선언된 변수에 접근을 가능한것은 Call-by-Reference라고 한다.

    - 매개 변수는 함수가 호출될때 선언이 되는 변수 이므로 함수 호출의 과정에서 선언과 동시에 전달되는 대상으로 초기화 된다.

    - 매개 변수는 선언과 동시에 초기화가 된다.

    - 즉 매개변수에 선언된 참조자는 여전히 선언과 동시에 초기화가 된다.

      ```c++
      #include<iostream>
      
      void SwapByRef(int&ref1, int &ref2)
      {
      	int temp = ref1;
      	ref1 = ref2;
      	ref2 = temp;
          //함수가 종료 되면 ref1과 ref2는 사라진다.
      }
      void SwapByRef2(const int&ref1,const int &ref2)
      {
      	int temp = ref1;
      	ref1 = ref2;
      	ref2 = temp;
       //값의 변경을 허용하지 않는다.
      }
      
      int main()
      {
      	int val1 = 10;
      	int val2 = 20;
      	SwapByRef(val1, val2); //상수 혹은 리터럴 값을 전달할 수 없다. 
      	std::cout << "val1 : " << val1 << std::endl;
      	std::cout << "val2 : " << val2 << std::endl;
      
      	return 0;
      }
      
      ```

      **반환형이 참조이고 반환도 참조로 받는 경우**

      ```c++
      #include<iostream>
      
      int& ref(int&ref)
      {
      	ref++;
      	return ref;
      }
      int main()
      {
      	int number1 = 1;
      	int &number2 = ref(number1);
      	std::cout << number2 << std::endl;
      
      
      	return 0;
      }
      ```

      **반환형은 참조이되 반환은 변수로 받는 경우**

      ```c++
      #include<iostream>
      
      int& ref(int&ref)
      {
      	ref++;
      	return ref;
      }
      
      int main()
      {
      	int number1 = 1;
      	int number2 = ref(number1);
      	std::cout << number2 << std::endl;
          
      	return 0;
      }
      ```

      **반환형이 값이라면 참조자로 그 값을 받을수 없다.**

      

**malloc & free를 대신하는 new&delete**

* int형 변수의 할당            int * ptr1=new int;
* double형 변수의 할당          double * ptr2=new double
* 길이가 3인 int형 배열의 할당 . int* arr1=new int[3]
* 길이가 7인 double형 배열의 할당  double * arr2=new double[7];

new는 malloc을 대한하는 메모리의 동적할당 방법 크기를 바이트 단위로 계산하는 일을 거치지 않아도 된다.

* 앞서 할당한 int 형 변수의 소멸 delete prt1;
* 앞서 할당한 double형 변수의 소멸  delete prt2;
* 앞서 할당한 int형 배열의 소멸  delete []arr1;
* 앞서 할당한 double형 배열의 소멸  delete []arr2;

포인터를 사용하지 않고 힙에 접근하기

```c++
int *ptr=new int;
int &ref=*ptr;
ref=20;
cout<<*ptr<<endl;
```

변수의 성향을 지니는 (값의 변경이 가능한) 대상에 대해서는 참조자의 선언이 가능하다.

c언어의 경우 힙영역으로 접근을 위해서는 반드시 포인터를 사용했지만 C++에서는 참조자의 접근을 허용한다.

**C++에서 C언어의 표준 함수 호출하기**

```c++
#include<stdio.h> -> #include<cstdio>
#include<stdlib.h> -> #include<stdlib>
#include<math.h> -> #include<math>
#include<string.h> -> #include<string>

```

* C++ 표준함수는 네임스페이스가 정의가 되어있으며 함수 오버로딩이 되어있습니다.
```



**01-3 매개변수의 디폴트 값**

- 함수를 정의할때 인자 값을 정의 할 수 있습니다.

- 매개 변수는 왼쪽 파라미터 부터 정의 된다.

- 함수의 선언을 별도로 둘때에는 디폴트 값의 선언을 함수의 선언부에 위치 시켜야한다.

  (컴파일러는 함수의 디폴트 값의 지정여부를 알아야 함수의 호출 문장을 적절히 컴파일 할수 있다)

- 전달되는 인자가 왼쪽에서부터 채워지므로 , 함수의 디폴트 인자 값은 오른쪽에서부터 채워져야 한다. 왼쪽부터 채워지면 컴파일 에러가 생긴다.

​```c++
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









