# 项目说明
## 运行环境
 - Linux环境
## 库依赖
 - 额外的nlohmann/json库
## 使用说明
 - cd build/ (若无buidl文件夹请先make)
 - ./mytool 目标文件地址 \[下界, 上界] \[指定0个或者多个标签(空格分隔)]
 -（若想使用mytool而非./mytool可自行添加临时环境变量或创建符号链接到系统目录）
 - 生成目录位于build/result/
## 项目特色
 - 模块化设计：graph_extractor.cpp为子图提取主逻辑，utils.cpp中实现部分相关工具函数，main.cpp为程序入口，通过CMakeList构建
 - bitset优化：使用bitset对寻找单源闭包以及幂集运算进行优化
 - 可视化拓展：使用QT对节点图进行了可视化，详情见qt_codes文件夹