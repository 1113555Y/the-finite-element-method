// 3D八节点六面体单元.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<vector>
#include<Eigen/Core>
#include<Eigen/Dense>

using namespace std;
using namespace Eigen;
double E=2.0e8;		//弹性模量
double v=0.3;		//泊松比
MatrixXd nodeCoord;
MatrixXd EleNode;
VectorXd restrainedNode;
VectorXd restrainedDof;
VectorXd xx, yy, zz;
VectorXd displacement;
VectorXd force;
MatrixXd kesi_yita;
VectorXd w;
VectorXd ww;
Vector3d fv;

int numEle, numNode,numDOF;

Matrix3d HEX_J(int kesi, int yita, int zeta, Matrix3d xyz);
MatrixXd HEX_N(int kesi, int yita, int zeta);
int main()
{
	nodeCoord.resize(54, 3);
	nodeCoord.setZero();
	EleNode.resize(16,8);
	EleNode.setZero();
	nodeCoord <<
		-1			, 0		, 0		,
		1			, 0		, 0		,
		1			, 0		, 0.5	,
		-1			, 0		, 0.5	,
		-0.75	, 0		, 0		,
		-0.75	, 0		, 0.25,
		-1			, 0		, 0.25,
		-0.75	, 0		, 0.5	,
		-0.5		, 0		, 0		,
		-0.5		, 0		, 0.25,
		-0.5		, 0		, 0.5	,
		-0.25	, 0		, 0		,
		-0.25	, 0		, 0.25,
		-0.25	, 0		, 0.5	,
		0			, 0		, 0		,
		0			, 0		, 0.25,
		0			, 0		, 0.5	,
		0.25		, 0		, 0		,
		0.25		, 0		, 0.25,
		0.25		, 0		, 0.5	,
		0.5		, 0		, 0		,
		0.5		, 0		, 0.25,
		0.5		, 0		, 0.5	,
		0.75		, 0		, 0		,
		0.75		, 0		, 0.25,
		0.75		, 0		, 0.5	,
		1			, 0		, 0.25,
		-1			, -0.2	, 0		,
		- 0.75	, -0.2	, 0		,
		-1			, -0.2, 0.25,
		-0.75	, -0.2, 0.25,
		-1			, -0.2, 0.5	,
		-0.75	, -0.2, 0.5	,
		-0.5		, -0.2, 0		,
		-0.5		, -0.2, 0.25,
		-0.5		, -0.2, 0.5	,
		-0.25	, -0.2, 0		,
		-0.25	, -0.2, 0.25,
		-0.25	, -0.2, 0.5	,
		0			, -0.2, 0		,
		0			, -0.2, 0.25,
		0			, -0.2, 0.5	,
		0.25		, -0.2, 0		,
		0.25		, -0.2, 0.25,
		0.25		, -0.2, 0.5	,
		0.5		, -0.2, 0		,
		0.5		, -0.2, 0.25,
		0.5		, -0.2, 0.5	,
		0.75		, -0.2, 0		,
		0.75		, -0.2, 0.25,
		0.75		, -0.2, 0.5	,
		1			, -0.2, 0		,
		1			, -0.2, 0.25,
		1			, -0.2, 0.5;
	EleNode <<
		1, 4, 5, 6, 27, 28, 30, 29,
		6, 5, 7, 3, 29, 30, 32, 31,
		4, 8, 9, 5, 28, 33, 34, 30,
		5, 9, 10, 7, 30, 34, 35, 32,
		8, 11, 12, 9, 33, 36, 37, 34,
		9, 12, 13, 10, 34, 37, 38, 35,
		11, 14, 15, 12, 36, 39, 40, 37,
		12, 15, 16, 13, 37, 40, 41, 38,
		14, 17, 18, 15, 39, 42, 43, 40,
		15, 18, 19, 16, 40, 43, 44, 41,
		17, 20, 21, 18, 42, 45, 46, 43,
		18, 21, 22, 19, 43, 46, 47, 44,
		20, 23, 24, 21, 45, 48, 49, 46,
		21, 24, 25, 22, 46, 49, 50, 47,
		23, 1, 26, 24, 48, 51, 52, 49,
		24, 26, 2, 25, 49, 52, 53, 50;
	numEle = EleNode.size();
	numNode = nodeCoord.size();
	restrainedNode.resize(6);
	restrainedNode << 0, 3, 6, 27, 29, 31;
	restrainedDof.resize(3* restrainedNode.size());
	for (int i = 0; i < restrainedNode.size(); i++)
	{
		restrainedDof(3*i) = 3 * restrainedNode(i);
		restrainedDof(3 * i+1) = 3 * restrainedNode(i)+1;
		restrainedDof(3 * i+2) = 3 * restrainedNode(i)+2;
	}
	//cout << restrainedDof << endl;
	xx.resize(nodeCoord.rows());
	yy.resize(nodeCoord.rows());
	zz.resize(nodeCoord.rows());
	xx = nodeCoord.col(0);
	yy= nodeCoord.col(1);
	zz = nodeCoord.col(2);
	numDOF = 3 * numNode;
	displacement.resize(numDOF);
	force.resize(numDOF);
	force.setZero();
	kesi_yita.resize(8, 3);
	kesi_yita <<
		-1 / sqrt(3), -1 / sqrt(3), -1 / sqrt(3),
		1 / sqrt(3), -1 / sqrt(3), -1 / sqrt(3),
		1 / sqrt(3), 1 / sqrt(3), -1 / sqrt(3),
		-1 / sqrt(3), 1 / sqrt(3), -1 / sqrt(3),
		-1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3),
		1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3),
		1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3),
		-1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3);
	w.resize(2);
	w << 1, 1;
	ww.resize(8);
	ww <<
		w(0)*w(0)*w(0),
		w(1)*w(0)*w(0),
		w(0)*w(0)*w(0),
		w(0)*w(1)*w(0),
		w(0)*w(0)*w(1),
		w(1)*w(0)*w(1),
		w(0)*w(0)*w(1),
		w(0)*w(1)*w(1);
	fv << 0, 0, -76.982;
	for (int i = 0; i < numEle; i++)
	{
		VectorXd noindex;
		noindex .resize(nodeCoord.cols());
		noindex = EleNode.row(i);
		MatrixXd xyz(8, 3);
		for (int j = 0; j < 8; j++)
		{
			xyz(j, 0) = xx(noindex(j));
			xyz(j, 1) = yy(noindex(j));
			xyz(j, 2) = zz(noindex(j));
		}
		VectorXd elef(24);
		elef.setZero();
		for (int j = 0; j < 8; j++)
		{
			double kesi= kesi_yita(j,0);
			double yita = kesi_yita(j, 1);
			double zeta = kesi_yita(j, 2);
			

		}
	}
    std::cout << "Hello World!\n"; 
}
Matrix3d HEX_J(int kesi, int yita, int zeta, Matrix3d xyz)
{
	MatrixXd NN(3, 8);
	NN <<
		-((yita - 1)*(zeta - 1)) / 8, ((yita - 1)*(zeta - 1)) / 8, -((yita + 1)*(zeta - 1)) / 8, ((yita + 1)*(zeta - 1)) / 8, ((yita - 1)*(zeta + 1)) / 8, -((yita - 1)*(zeta + 1)) / 8, ((yita + 1)*(zeta + 1)) / 8, -((yita + 1)*(zeta + 1)) / 8,
		-((kesi - 1)*(zeta - 1)) / 8, ((kesi + 1)*(zeta - 1)) / 8, -((kesi + 1)*(zeta - 1)) / 8, ((kesi - 1)*(zeta - 1)) / 8, ((kesi - 1)*(zeta + 1)) / 8, -((kesi + 1)*(zeta + 1)) / 8, ((kesi + 1)*(zeta + 1)) / 8, -((kesi - 1)*(zeta + 1)) / 8,
		-((kesi - 1)*(yita - 1)) / 8, ((kesi + 1)*(yita - 1)) / 8, -((kesi + 1)*(yita + 1)) / 8, ((kesi - 1)*(yita +1)) / 8, ((kesi - 1)*(yita - 1)) / 8, -((kesi + 1)*(yita - 1)) / 8, ((kesi + 1)*(yita + 1)) / 8, -((kesi - 1)*(yita + 1)) / 8;
	Matrix3d J;
	J = NN * xyz;
	return J;
}
MatrixXd HEX_N(int kesi, int yita, int zeta)
{
	double N1 = 1 / 8 * (1 - kesi)*(1 - yita)*(1 - zeta);
	double N2 = 1 / 8 * (1 + kesi)*(1 - yita)*(1 - zeta);
	double N3 = 1 / 8 * (1 + kesi)*(1 + yita)*(1 - zeta);
	double N4 = 1 / 8 * (1 - kesi)*(1 + yita)*(1 - zeta);
	double N5 = 1 / 8 * (1 - kesi)*(1 - yita)*(1 + zeta);
	double N6 = 1 / 8 * (1 + kesi)*(1 - yita)*(1 + zeta);
	double N7 = 1 / 8 * (1 + kesi)*(1 + yita)*(1 + zeta);
	double N8 = 1 / 8 * (1 - kesi)*(1 +yita)*(1 + zeta);

	MatrixXd N(3, 24);
	N <<
		N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0, 0,
		0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0,
		0, 0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8;
	return N;
	

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
