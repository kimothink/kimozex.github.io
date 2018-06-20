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

### Chapter 06 Friend와 static 그리고 Const 

**06-1 Const와 관련해서 아직 못다한 이야기**

* 객체에도 Const선언이 가능하다. 객체의 멤버 변수의 변경을 허용하지 않는다.

* 함수 의 const 선언 유무로 함수 오버로딩의 조건이 된다.

* const 객체 또는 참조자를 대상으로 멤버함수 호출시 const 선언된 멤버 함수가 호출된다.

  ```c++
  class Sosimple
  {
      
      void SimpleFunc()
      {
          
      }
      
      
      void SimpleFunc() const
      {
          
      }
  };
  
  void Yourfunc(const Sosimple &obj)
  {
      obj.Simplefunc(); //const SimpleFunc 호출한다.
  }
  
  int main()
  {
     Sosimple obj(2);
     const Sosimple obj2(7);
     Yourfunc(obj1);
     Yourfunc(obj2);
      return 0;
  }
  
  ```

  

```c++
// ConsoleApplication12.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include<iostream>
class SoSimple
{
private:
	int num;
public :
	SoSimple(int n) :num(n)
	{

	}

	SoSimple& AddNum(int n)
	{
		num += n;
		return *this;
	}

	void ShowData()const
	{
		std::cout <<"num:"<< num << std::endl;
	}

};


int main()
{
	const SoSimple  obj(7);
	//obj.AddNum(5);
	obj.ShowData();

    return 0;
}


```



**Chapter 06-02 클래스와 함수에 대한 friend**

* 나의 private 영역에 접근을 허용한다.
* 정보은닉에 위배된다.

**Chapter -06-03 C++에서의 Static란?**

*  전역변수 / 지역변수에 Static 선언이 가능하고 할당이 되면 프로그램 종료 될때까지 메모리에 

  할당 된다.

* 지역변수에 static의 의미.

  *  한번만 초기화 되고 지역변수와 달리 함수를 빠져나가도 소멸되지 않는다.

* 전역변수에 static의 의미  

  * 선언된 파일 내에서만 참조를 허용하겠다는 의미

* 클래스의 멤버 변수와 멤버 함수에도 static 키워드가 가능하다.

* 클래스의 static 멤버변수 선언시

  * 프로그램이 종료되어야 메모리에 종료된다.
  * 딱 하나만 존재한다.
  * 접근 방법 클래스명::멤버변수 로 접근한다.(public으로 선언되어야한다.)
  * 클래스간의 같은 메모리 공간을 공유한다.

  ```c++
  class SoSimple
  {
    private:
      static int simObjcnt //static 멤버 변수 , 클래스 변수
    
  };
  
  int SoSimple::simObjcnt=0;
  //초기화 방법
  
  int main()
  {
      SoSimple sim1;  
      SoSimple sim2;
      SoSimple sim3;
      //simObjcnt는 sim1,sim2,sim3에 공유된다.
  }
  ```

```c++


```



static 멤버함수

* 선언된 클래스의 모든 객체가 공유한다.

* public 으로 선언이 되면 클래스의 이름을 이용해서 호출이 가능하다.

* 객체의 멤버로 존재하는  것이 아니다.

* static 함수는 객체 내에 존재하는 함수가 아니기 때문에 멤버 변수나 멤버 함수 접근이 불가능하다. static 함수는 static 변수에만 접근이 가능하고 static 함수만 호출 가능하다.

  ```c++
  class SoSimple
  {
    private 
       int num1;
      static int num2;
     public :
      static void adder(int n)
      {
          num1+=n;//컴파일 에러
          num2+=n;//
      }
  };
  ```

  

const static 멤버와 mutable

* const static의 멤버 변수는 클래스가 정의 될때 지정된 값이 유지되는 상수이기 때문에 위 예제에서 보이는 바와 같이 초기화가 가능하도록 문법을 정의하고 있다.

  ```c++
  class CountryArea
  {
    private :
      mutable int num2;
    public :
      const static int russia=170540
          void copynum2() const
      {
          num2=num1;
          //mutable로 선언되 멤버 변수는 const함수내에서 값의 변경이 가능하다.
      }
  };
  
  int main(void)
  {
      std::cout<<"러시아 면적 "<<CountryArea::russia<<std::endl;
      return 0;
  }
  ```

  