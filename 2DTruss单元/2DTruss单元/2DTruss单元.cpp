// 2DTruss单元.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;


double E = 200000;								//弹性模量(MPa)
double A = 4532;									//截面积（mm^2）
//double scalefactor = 0.2;
MatrixXd NodeCoord(7, 2);
MatrixXd EleNode(11, 2);
int numNode ;		//节点数量
int numEle ;		//单元数量
//MatrixXf restrainedDof(1,3);
RowVector3d restrainedDof;
VectorXd xx(7);		//节点的x坐标[numNode * 1]
VectorXd yy(7);		//节点的y坐标[numNode * 1]
int numDOF;			//自由度总数 numNode*2
VectorXd displacement(14);	//节点的整体位移 [numDof*1]
VectorXd force(14);		//节点力[numDOF * 1]
MatrixXd stiffness(14, 14);	//刚度矩阵 [numDOF*numDOF]
VectorXd activeDof( 11);		//激活的自由度
VectorXd sigma(numEle);	//单元的轴向应力 [numEl*1]
Vector3d reaction;

//MatrixXf displacement(7, 1);



int main()
{
	NodeCoord << -4500, 0, -1500, 0, 1500, 0, 4500, 0, -3000, 3000, 0, 3000, 3000, 3000;
	//cout << NodeCoord<<endl;
	EleNode << 0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 0, 4, 1, 5, 2, 6, 1, 4, 2, 5, 3, 6;

	numNode = NodeCoord.rows();
	numEle = EleNode.rows();
	restrainedDof << 0, 1, 7;
	//cout << restrainedDof<<endl;
	for (int i = 0; i < NodeCoord.rows(); i++)
	{
		xx(i) = NodeCoord(i, 0);
		yy(i) = NodeCoord(i, 1);
	}
	//cout << xx<<endl;
	//cout << yy<<endl;
	numDOF = (numNode) * 2;

	displacement.setZero();
	force.setZero();

	force(9) = -100000;
	force(11) = -100000;
	force(13) = -100000;
	//cout << force.inverse() << endl;
	//displacement.resize(numDOF);
	//force.resize(numDOF);

#pragma region 刚度矩阵
	//stiffness.resize(numDOF);
	stiffness.setZero();
	//cout << stiffness << endl;

	for (int i = 0; i < numEle; i++)
	{
		Vector2d noindex;
		noindex(0) = EleNode(i, 0);
		noindex(1) = EleNode(i, 1);
		double deltax = xx(noindex(1)) - xx(noindex(0));
		double deltay = yy(noindex(1)) - yy(noindex(0));
		double L = sqrt(deltax*deltax + deltay * deltay);
		double C = deltax / L;
		double S = deltay / L;
		MatrixXd  eleK(4, 4);
		eleK << C * C, C*S, -C * C, -C * S,
			C*S, S*S, -C * S, -S * S,
			-C * C, -C * S, C*C, C*S,
			-C * S, -S * S, C*S, S*S;
		
		eleK = eleK * E*A / L;
		//cout << eleK << endl;
		//MatrixXd eleDof(4,4);
		Vector4d  eleDof(4);
		eleDof << noindex(0) * 2, noindex(0) * 2 + 1, noindex(1) * 2,  noindex(1) * 2 + 1;
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				stiffness(eleDof(j), eleDof(k)) = stiffness(eleDof(j), eleDof(k))+eleK(j, k);

			}
		}
	}
	//cout << stiffness;
#pragma endregion
#pragma region 集合的差集
	vector<int> v_1{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13 };
	vector<int> v_2{ 0,1,7 };
	sort(v_1.begin(), v_1.end());//排序
	sort(v_2.begin(), v_2.end());
	vector<int> v_intersection;
	vector<int> v_difference;
	set_difference(v_1.begin(), v_1.end(), v_2.begin(), v_2.end(), back_inserter(v_difference)	);
	for (int i = 0; i < 11; i++)
	{
		activeDof(i) = v_difference[i];
	}
	//cout << activeDof<<endl;
	
#pragma endregion	
#pragma region  节点位移(划行划列)
	MatrixXd stiffness_1(numDOF, numDOF);
	stiffness_1 = stiffness;
	for (int i = restrainedDof.size()-1; i >=0; i--)
	{
		stiffness_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), numDOF) = stiffness_1.block(restrainedDof(i) + 1, 0, numDOF-1 - restrainedDof(i), numDOF);
	}
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		stiffness_1.block(0,restrainedDof(i), numDOF- restrainedDof.size(),  numDOF - 1 - restrainedDof(i)) = stiffness_1.block( 0,restrainedDof(i) + 1, numDOF-restrainedDof.size(), numDOF - 1 - restrainedDof(i));
	}
	stiffness_1.conservativeResize(numDOF - restrainedDof.size(), numDOF - restrainedDof.size());
	cout << stiffness_1<<endl;

	VectorXd force_1(numDOF);
	force_1 = force;
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		force_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), 1) = force_1.block(restrainedDof(i) + 1, 0, numDOF - 1 - restrainedDof(i), 1);
	}
	force_1.conservativeResize(numDOF - restrainedDof.size());
	cout << force_1 << endl;

	VectorXd displacement_1(numDOF);
	displacement_1 = displacement;
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		displacement_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), 1) = displacement_1.block(restrainedDof(i) + 1, 0, numDOF - 1 - restrainedDof(i), 1);
	}
	 /*displacement_1.block(7, 0, 6, 1) =  displacement_1.block(8, 0, 6, 1);
	 displacement_1.block(1, 0, 11, 1) =  displacement_1.block(2, 0, 11, 1);
	 displacement_1.block(0, 0, 11, 1) =  displacement_1.block(1, 0, 11, 1);*/
	 displacement_1.conservativeResize(numDOF - restrainedDof.size());

	displacement_1 = stiffness_1.inverse()*force_1;
	//cout << displacement_1 << endl;
	 for (int i = 0; i < 11; i++)
	 {
		 displacement(activeDof(i)) = displacement_1(i);
	 }
	 cout << "displacement:" << endl;
	cout << displacement << endl;
#pragma endregion
#pragma region 单元的轴向应力
	 sigma.conservativeResize(numEle);
	 sigma.setZero();
	 for (int i = 0; i < numEle; i++)
	 {
		 Vector2d noindex;
		 noindex(0) = EleNode(i, 0);
		 noindex(1) = EleNode(i, 1);
		 double deltax = xx(noindex(1)) - xx(noindex(0));
		 double deltay = yy(noindex(1)) - yy(noindex(0));
		 double L = sqrt(deltax*deltax + deltay * deltay);
		 double C = deltax / L;
		 double S = deltay / L;
		 Vector4d  eleDof(4);
		 eleDof << noindex(0) * 2, noindex(0) * 2 + 1, noindex(1) * 2, noindex(1) * 2 + 1;
		 Vector4d vector4d_1;
		 vector4d_1 << -C, -S, C, S;
		 for (int k = 0; k < 4; k++)
		 {
			 sigma(i) = sigma(i) + (E / L)*vector4d_1(k)*displacement(eleDof(k));
		 }
	 }
	 cout << "sigma: "<< endl;
	 cout << sigma << endl;
#pragma endregion
#pragma region  支座反力
	 reaction.setZero();
	 for (int i=0 ;i<3;i++)
	 {
		 int rownum = restrainedDof(i);
		 reaction(i)= stiffness.row(rownum)*displacement;
	 }
	 cout <<" reaction:" << endl;
	 cout << reaction << endl;
#pragma endregion

	 

	cout << "Hello World!\n";
	   
}


// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
