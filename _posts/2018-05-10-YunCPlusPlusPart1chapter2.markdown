---
title: "[CPlusPlus_Study]C++로의 전환"
layout: post
date: 2018-05-10 00:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- CPlusPlus Programming
- Coding
- CPlusPlus
  star: false
  category: blog
  author: john Ko
  description: Coding
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

      

**malloc & frre를 대신하는 new&delete**

* int형 변수의 할당            int * ptr1=new int;
* double형 변수의 할당          double * ptr2=new double
* 길이가 3인 int형 배열의 할당 . int* arr1=new int[3]
* 길이가 7인 double형 배열의 할당  double * arr2=new double[7];

new는 malloc을 대한하는 메로리의 동적할당 방법 크기를 바이트 단위로 계산하는 일을 거치지 않아도 된다.

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