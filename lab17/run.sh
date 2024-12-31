#!/bin/bash

# 使用 set -e 使脚本在任何命令发生错误时退出
set -e

# 编译程序
gcc -o additional additional.c

# 创建数据库
./additional db.dat c 100 10

# 设置记录
./additional db.dat s 1 "Zed" "zed@zedshaw.com"
./additional db.dat s 2 "Frank" "frank@zedshaw.com"
./additional db.dat s 3 "Joe" "joe@zedshaw.com"

# 获取记录
./additional db.dat g 1
./additional db.dat g 2
./additional db.dat g 3

# 列出所有记录
./additional db.dat l

# 删除记录
./additional db.dat d 2

# 再次列出所有记录
./additional db.dat l

# 获取已删除的记录（应失败）
./additional db.dat g 2 || echo "Record 2 not found, as expected."

echo "All tests passed."