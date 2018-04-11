---
title: "리터럴(Literal) 과 Expression(표현식) 및 Statements(문)"
layout: post
date: 2018-04-24 22:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- Fundamental Programming
star: true
category: blog
author: john Ko
description: Fundamental Programming
---

## Title : 리터럴(Literal) 과 Expression(표현식) 및 Statements(문)

### 리터럴(Literal)

리터럴은 명칭이 아니라,  **값** 그 자체이다. 변수에 넣는 **변하지 않는 데이터**를 의미한다.

리터럴은 정수, 부동소수정 숫자, 문자열 , 불린(boolean) 등을 리터럴이라고 한다.

```
int data = 1; // 값 1은 리터럴(Literal)이라고 부릅니다.
```



###표현식(Expression)

표현식은 하나의 값을 **도출 되는 식**입니다. 프로그래밍 언어가 해석하는 하나 이상의 명시적 값, 상수 , 변수 , 연산자 조합이고 함수도 함수도 실행되면 하나의 값이 되어 이를 **조합**해서 **표현식**으로 인식합니다.

``` 1+2 // 1 더하기 2 는 값으로 도출 되기 때문에 식이 됩니다. ```

``` x // x는 변수의 값이 도출된다.```

``` x = 3  좌변에 우변이 대입되면서 우변의 값이 대입식의 도출 값이 된다. ``` 

```ADD(1,2)는 식이다. 보통 반환을 하는 함수 호출은 반환 값이 도출 되는 것이 므로 식이라고 할수 있다.```



### 문(Statements) 

문장은 한 라인으로 처리되는 단순 문장으로부터 여러라인을 처리하는 블록 문장 등이 있다. 특히 제어문, 순환문 등은 여러 라인을 하나의 블록으로 묶어서 처리하므로 블록 문장이다. 

``` x = 3 ; //할당문  ```

```ADD(1,2); //호출문 ```

 ```c++
if ( i > 0 )  //조건문
   y = x / i;  
else   
{  
    x = i;  
    y = f( x );  
}
 ```









