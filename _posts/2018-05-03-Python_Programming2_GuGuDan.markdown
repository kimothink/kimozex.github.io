---
title: "[Python_Coding]한줄로 구구단 코딩하기"
layout: post
date: 2018-05-03 01:44
image: /assets/images/markdown.jpg
headerImage: false
tag:
- Python Programming
- Coding
- Python
- 생활코딩
star: true
category: blog
author: john Ko
description: Coding
---

## Title : [Python]한줄로 구구단 코딩하기



### 한줄로 구구단 코딩하기 


![구구단](https://youtu.be/CuOwAqEk348)


예전에  본 드라마의 내용이다. 

"1줄로 짤 코딩을 누가 10줄로 만들어 놓았어요?!  에러 찾기 힘들게!!!"

"코드가 길어서 프로그램만 무거워졌지 않습니까?!"

"이러니깐 서비스가 버벅 거리지.."

그렇다.!!!! 코드가 길면 ?! 서비스가 버벅 거린덴다. 그래서 상욱형님의 구구단 코드를 한줄로 짜 보았다.

 ```python
print([(lambda x,y : '{}x{}={}'.format(x, y, x*y))(x, y) for x in range(2,10) for y in range(1,10)])
 ```

![결과값](/imgs/GuGuDan.jpg)







