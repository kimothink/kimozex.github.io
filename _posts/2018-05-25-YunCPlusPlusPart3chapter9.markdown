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



**07-02 상속의 문법적인 이해**

* A<-B  : B클래스가 A클래스 를 상속했다.
* B클래스가 A클래스를 상속하면서 A클래스의 데이터 함수를 사용할수 있다.
* B의 초기화 할때 A의 초기화 데이터도 인자값을 전달 해주어야 한다.

```c++
class Person
{
    private :
    int age;
    char name[50];
    public :
    Person(int Myage,char * myname):age(myage)
    {
        strcpy(name,myname);
    }
    
}

class UnivStudent : public Person
{
private:
	char major[50];
public :
		UnivStudent(char * myname,int myage,char* mymajor):Perosn(myage,myname)
        {
        strcpy(myjor,mymajor);
        }

}
```



---------------------

| Person             | UnivStudent         |
| ------------------ | ------------------- |
| 상위 클래스        | 하위클래스          |
| 기초(Base)클래스   | 유도(Derived)클래스 |
| 슈퍼(Super) 클래스 | 서브(Sub)클래스     |
| 부모클래스         | 자식클래스          |

* 하위 클래스는 상위 클래스의 인자값을 받는 책임과 전달하는 책임을 가지므로써 생성자 호출을 한다.
* 이니셜라이저를 통해서 유도 클래스는 기초클래스의 생성자를 명시적으로 호출해야한다.
* 유도 클래스의 생성자는 기초 클래스의 멤버를 초기화 할 의무를 갖는다. 단 ! 기초클래스의 생성자를 명시적으로 호출해서 초기화한다. 때문에 유도 클래스 UnivStudent는 기초 클래스의 생성 호출을 위한 인자까지 함께 전달 받아야한다.
* private 멤버는 유도 클래스에서도 접근이 불가능하므로, 생성장의 호출을 통해서 기초 클래스의 멤버를 초기화 해야한다.
* 기초 클래스의 Private의 멤버 변수는 유도 클래스에서도 접근이 불가능하다.
* 유도 클래스에서 기초클래스의 명시적 생성자를 정의를 안하면 void 기초클래스의 생성자를 호출한다.
* 유도 클래스의 소멸자가 실행된 이후에 기초 클래스의 소멸자가 실행된다.
* 스택에서 생성된 객체의 소멸순서는 생성 순서와 반대가 된다.
* 기초클래스의 멤버 대상의 동적할당은 기초클래스의 생성자를 통해서 소멸역시 기초 클래스의 소멸자를 통해서 소멸시킨다.



**Chapter 07-03 Protected 선언과 세가지 형태의 상속 **

* private < protected < public
* private을 기준으로 보면 protected 는 private과 달리 상속 관계에서의 접근을 허용한다.

```c++
class Base 
{
    private :
    int num1;
    protected :
    int num2;
    public :
    int num3;
    void ShowData()
    {
        std::cout<<num1<<','<<num2<<','<<num3<<std::endl;
    }
}

class Derived : public Base
{
public :
void ShowBaseMemeber()
{
    std::cout << num1; //compile error
    std::cout <<num2; // compile OK
    std::cout << num3 ;//compile Error
}
}
```



------------------------------------

| 접근 권한자 |                                                              |
| ----------- | ------------------------------------------------------------ |
| public      | 접근 제어 권한을 그대로 상속 단 private은 접근 불가로 상속   |
| procted     | protected 보다 접근 범위가 넓은 멤버는 protecte로 상속한다. private는 접근 불가로 상속한다. |
| private     | private보다 접근의 범위가 넓은 멤버로 protected로 상속한다. private은 접근 불가로 상속한다. |



|  

**Chapter 07-04 상속을 위한 최소한의 조건 **

* 같은 범주안에 관계를 IS-A 관계이다. (전화기 -> 핸드폰  / 컴퓨터 -> 노트북 컴퓨터) 고유한 기능에서 기능을 추가한것을 IS -A 관계이다.
* 기본 기능에 새로운 특성이 추가된것을 IS -A 관계이다.
* 기존의 클래스는 범주가 높으며 상속을 할수록  기능이 추가 될 수록 구체화 되어간다.
* HAS -a 관계는 상속하지 말자
* has -a 관계는 포함으로 표현한다. 클래스의 멤버로 표현한다.