#!/bin/bash

# git 基本命令

#查看所有分支信息
git branch -a 

#删除远程分支和本地分支
git push origin --delete develop

#删除本地分支
git branch -d develop 