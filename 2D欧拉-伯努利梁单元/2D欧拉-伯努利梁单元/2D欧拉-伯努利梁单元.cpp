// 2D欧拉-伯努利梁单元.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<Eigen\Core>
#include<Eigen\Dense>
#include<algorithm>
#include<vector>
using namespace std;
using namespace Eigen;

Vector2d EList;		//弹性模量
Vector2d AList;		//截面积
Vector2d IList;			//截面惯性矩
MatrixXd nodeCoord; //节点[numNode*2]
MatrixXd EleNode;	//单元[numEle*2]
VectorXd EleSect;	//单元截面
VectorXd restrainedDof;		//约束自由度
VectorXd xx, yy;		//节点x，y坐标[numNode]
VectorXd displacement;	//位移[numDOF]
VectorXd force;		//节点 力
MatrixXd stiffness;	//刚度矩阵
VectorXd activeDof;
VectorXd reaction;	//支座反力

int numEle;
int numNode;
int numDOF;
int main()
{
	EList << 3.0e7, 3.0e7;
	AList << 0.08, 0.16;
	IList << 0.0128 / 12, 0.0256 / 12;
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
	EleNode << 0,1, 
						1,2, 
						2,3,  
						4,5,  
						5,6, 
						6,7,  
						1, 5,
						2, 6, 
						3, 7;
	EleSect.resize(9);
	EleSect << 1, 1, 1, 1, 1, 1, 0, 0, 0;						
	numEle = EleNode.rows();
	numNode = nodeCoord.rows();
	restrainedDof.resize(6);
	restrainedDof<< 0, 1, 2, 12, 13, 14;
	xx.resize(8); yy.resize(8);
	xx = nodeCoord.col(0);
	yy = nodeCoord.col(1);
	numDOF = numNode * 3;
	displacement.resize(numDOF);
	displacement.setZero();
	force.resize(numDOF);
	force.setZero();
	force(9) = 200;

#pragma region 刚度矩阵
	stiffness.resize(numDOF, numDOF);
	stiffness.setZero();
	for (int i = 0; i < numEle; i++)
	{
		Vector2d noindex=EleNode.row(i);
		double deltax = xx(noindex(1)) - xx(noindex(0));
		double deltay = yy(noindex(1)) - yy(noindex(0));
		double L = sqrt(deltax*deltax + deltay * deltay);
		double C = deltax / L;
		double S = deltay / L;
		MatrixXd T(6, 6);
		T << C, S, 0, 0, 0, 0,
				-S, C, 0, 0, 0, 0,
				0, 0, 1, 0, 0, 0,
				0, 0, 0, C, S, 0,
				0, 0, 0, -S, C, 0,
				0, 0, 0, 0, 0, 1;
		double E = EList(EleSect(i));
		double A = AList(EleSect(i));
		double I = IList(EleSect(i));
		MatrixXd eleK(6, 6);
		double EAL = E * A / L;
		double EIL1 = E * I / L;
		double EIL2 = 6.0*E*I / L / L;
		double EIL3 = 12.0*E*I / L/L / L;
		eleK << EAL, 0, 0, -EAL, 0, 0,
						0, EIL3, EIL2, 0, -EIL3, EIL2,
						0, EIL2, 4 * EIL1, 0, -EIL2, 2 * EIL1,
						-EAL, 0, 0, EAL, 0, 0,
						0, -EIL3, -EIL2, 0, EIL3, -EIL2,
						0, EIL2, 2 * EIL1, 0, -EIL2, 4 * EIL1;
		VectorXd eleDof(6);
		eleDof << 3*noindex(0), 3*noindex(0) + 1, 3*noindex(0) + 2, 3*noindex(1), 3*noindex(1) + 1, 3*noindex(1) + 2;
		MatrixXd TeleKT(6, 6);
		TeleKT = T.inverse()*eleK*T;
		for (int j = 0; j < 6; j++) 
		{
			for (int k = 0; k < 6; k++)
			{
				stiffness(eleDof(j), eleDof(k)) = stiffness(eleDof(j), eleDof(k))+ TeleKT(j,k);
				//cout << stiffness(eleDof(j), eleDof(k)) << endl;
			}
		}
	}
	//cout << stiffness << endl;

	#pragma endregion
#pragma region 集合的差集
	activeDof.resize(numDOF - 6);
	vector<int> v_1{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
	vector<int> v_2{ 0,1,2,12,13,14 };
	sort(v_1.begin(), v_1.end());//排序
	sort(v_2.begin(), v_2.end());
	vector<int> v_intersection;
	vector<int> v_difference;
	set_difference(v_1.begin(), v_1.end(), v_2.begin(), v_2.end(), back_inserter(v_difference));
	for (int i = 0; i < numDOF - 6; i++)
	{
		activeDof(i) = v_difference[i];
	}
	//cout << activeDof<<endl;
#pragma endregion
#pragma region  节点位移(划行划列)
	MatrixXd stiffness_1(numDOF, numDOF);
	stiffness_1 = stiffness;
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		stiffness_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), numDOF) = stiffness_1.block(restrainedDof(i) + 1, 0, numDOF - 1 - restrainedDof(i), numDOF);
	}
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		stiffness_1.block(0, restrainedDof(i), numDOF - restrainedDof.size(), numDOF - 1 - restrainedDof(i)) = stiffness_1.block(0, restrainedDof(i) + 1, numDOF - restrainedDof.size(), numDOF - 1 - restrainedDof(i));
	}
	stiffness_1.conservativeResize(numDOF - restrainedDof.size(), numDOF - restrainedDof.size());
	//cout << stiffness_1 << endl;

	VectorXd force_1(numDOF);
	force_1 = force;
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		force_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), 1) = force_1.block(restrainedDof(i) + 1, 0, numDOF - 1 - restrainedDof(i), 1);
	}
	force_1.conservativeResize(numDOF - restrainedDof.size());
	//cout << force_1 << endl;

	VectorXd displacement_1(numDOF);
	displacement_1 = displacement;
	for (int i = restrainedDof.size() - 1; i >= 0; i--)
	{
		displacement_1.block(restrainedDof(i), 0, numDOF - 1 - restrainedDof(i), 1) = displacement_1.block(restrainedDof(i) + 1, 0, numDOF - 1 - restrainedDof(i), 1);
	}
	displacement_1.conservativeResize(numDOF - restrainedDof.size());

	displacement_1 = stiffness_1.inverse()*force_1;
	//cout << displacement_1 << endl;
	for (int i = 0; i < numDOF - restrainedDof.size(); i++)
	{
		displacement(activeDof(i)) = displacement_1(i);
	}
	//cout << "displacement:" << endl;
	for (int i = 0; i < numNode; i++)
	{
		//cout <<i<<"\t"<< displacement(3*i) << "\t" << displacement(3 * i+1) << "\t" << displacement(3 * i+2) << "\t" << endl;
	}

	//cout << displacement << endl;
#pragma endregion
#pragma region 支座反力
	reaction.resize(restrainedDof.size());
	for (int i = 0; i < restrainedDof.size(); i++)
	{
		int rownum = restrainedDof(i);
		reaction(i) = stiffness.row(rownum)*displacement;
	}
	//cout << reaction << endl;

#pragma endregion
#pragma region 单元杆端内力
	for (int i = 0; i < numEle; i++)
	{
		Vector2d noindex = EleNode.row(i);
		double deltax = xx(noindex(1)) - xx(noindex(0));
		double deltay = yy(noindex(1)) - yy(noindex(0));
		double L = sqrt(deltax*deltax + deltay * deltay);
		double C = deltax / L;
		double S = deltay / L;
		MatrixXd T(6, 6);
		T << C, S, 0, 0, 0, 0,
			-S, C, 0, 0, 0, 0,
			0, 0, 1, 0, 0, 0,
			0, 0, 0, C, S, 0,
			0, 0, 0, -S, C, 0,
			0, 0, 0, 0, 0, 1;
		double E = EList(EleSect(i));
		double A = AList(EleSect(i));
		double I = IList(EleSect(i));
		MatrixXd eleK(6, 6);
		double EAL = E * A / L;
		double EIL1 = E * I / L;
		double EIL2 = 6.0*E*I / L / L;
		double EIL3 = 12.0*E*I / L / L / L;
		eleK << EAL, 0, 0, -EAL, 0, 0,
			0, EIL3, EIL2, 0, -EIL3, EIL2,
			0, EIL2, 4 * EIL1, 0, -EIL2, 2 * EIL1,
			-EAL, 0, 0, EAL, 0, 0,
			0, -EIL3, -EIL2, 0, EIL3, -EIL2,
			0, EIL2, 2 * EIL1, 0, -EIL2, 4 * EIL1;
		VectorXd eleDof(6);
		eleDof << 3 * noindex(0), 3 * noindex(0) + 1, 3 * noindex(0) + 2, 3 * noindex(1), 3 * noindex(1) + 1, 3 * noindex(1) + 2;

		VectorXd ue(6);
		for (int j = 0; j < eleDof.size(); j++)
		{
			ue(j) = displacement(eleDof(j));
		}
		VectorXd uep(eleDof.size());
		uep = T * ue;
		VectorXd fp(eleDof.size());
		fp = eleK * uep;
		cout <<" fp "<< i<< endl;
		cout << fp << endl;
	}
#pragma endregion


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
