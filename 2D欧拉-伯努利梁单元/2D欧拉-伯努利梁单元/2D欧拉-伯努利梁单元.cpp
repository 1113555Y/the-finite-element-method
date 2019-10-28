// 2D欧拉-伯努利梁单元.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<Eigen\Core>
#include<Eigen\Dense>
using namespace std;
using namespace Eigen;

Vector2d ELise;		//弹性模量
Vector2d ALise;		//截面积
Vector2d ILise;			//截面惯性矩
MatrixXd nodeCoord; //节点[numNode*2]
MatrixXd EleNode;	//单元[numEle*2]
VectorXd EleSect;	//单元截面
int numEle;
int numNode;
int main()
{
	ELise << 3.0e7, 3.0e7;
	ALise << 0.08, 0.16;
	ILise << 0.0128 / 12, 0.0256 / 12;
	nodeCoord.resize(8, 2);
	nodeCoord << 0, 0,
							0, 3, 
							0, 6, 
							0, 9, 
							5, 0, 
							5, 3,
							5, 6, 
							5, 9;
	EleNode.resize(9,2);
	EleNode << 1, 2,
						2, 3,
						3, 4, 
						5, 6, 
						6, 7,
						7, 8, 
						2, 6,
						3, 7, 
						4, 8;
	EleSect.resize(9);
	EleSect << 2, 2, 2, 2, 2, 2, 1, 1, 1;
	numEle = EleNode.rows();
	numNode = nodeCoord.rows();


    cout << "Hello World!\n"; 

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
