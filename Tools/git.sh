#!/bin/bash

# git 基本命令

#查看所有分支信息
git branch -a 

#删除远程分支和本地分支
git push origin --delete develop

#删除本地分支
git branch -d develop 


#一、开发分支（chensong_develop）上的代码达到上线的标准后，要合并到 master 分支

git checkout chensong_develop
git pull
git checkout master
git merge chensong_develop
git push -u origin master

#二、当master代码改动了，需要更新开发分支（chensong_develop）上的代码

git checkout master 
git pull 
git checkout chensong_develop
git merge master 
git push -u origin chensong_develop