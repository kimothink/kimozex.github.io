---
title: "[Python][Coding]확장명을 이용한 파일삭제"
layout: post
date: 2018-05-01 11:44
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

## Title : [Python]확장명을 이용한 파일삭제



### 확장자명으로 파일삭제 

파일명은 다르지만 확장자명이 같은 필요없는 파일들이 한 경로에 쌓여져 있어  한꺼번에 혹은 프로그램 시작시에 자동으로 파일을 삭제 하게 만든 코드이다. 폴더안의 파일도 확인 하므로 잘 확인해서 쓰셔야 됩니다.

 ```python
import os

for(path,dir,files)in os.walk("경로") :#경로 Ex)C:\\Users\\kimo\\Downloads
    for filename in files :
        ext=os.path.splitext(filename)[-1]
        if ext=='.확장자':#확장자명 Ex).torrent
            os.remove("%s\\%s"%(path,filename))
 ```









