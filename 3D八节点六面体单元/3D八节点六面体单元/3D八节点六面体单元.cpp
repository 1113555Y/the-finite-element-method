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
VectorXd DOF;
VectorXd restrainedDof;
VectorXd activeDof;
VectorXd noindex;
VectorXd xx, yy, zz;
VectorXd displacement;
VectorXd force;
MatrixXd kesi_yita;		//高斯积分节点坐标(查表)
VectorXd w;
VectorXd ww;
Vector3d fv;
VectorXd eleDof;
MatrixXd stiffness;
MatrixXd elasticMatrixD;
MatrixXd stress;
VectorXd reaction;

double ffff;
int numEle, numNode,numDOF;

Matrix3d HEX_J(double kesi, double yita, double zeta, MatrixXd xyz);		//Jacobian矩阵
MatrixXd HEX_N(double kesi, double yita, double zeta);			//形函数矩阵
MatrixXd HEX_G(double kesi, double yita, double zeta);
VectorXd VectorXd_div(VectorXd v1, VectorXd v2);
int main()
{
#pragma region 已知条件
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
		0, 4, 5, 6, 27, 28, 30, 29,
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
	numEle = EleNode.rows();
	numNode = nodeCoord.rows();
	restrainedNode.resize(6);
	restrainedNode << 0, 3, 6, 27, 29, 31;
	restrainedDof.resize(3* restrainedNode.size());
	for (int i = 0; i < restrainedNode.size(); i++)
	{
		restrainedDof(3*i) = 3 * restrainedNode(i);
		restrainedDof(3 * i+1) = 3 * restrainedNode(i)+1;
		restrainedDof(3 * i+2) = 3 * restrainedNode(i)+2;
	}

#pragma endregion
	//cout << restrainedDof << endl;

	xx.resize(nodeCoord.rows());
	yy.resize(nodeCoord.rows());
	zz.resize(nodeCoord.rows());
	xx = nodeCoord.col(0);
	yy= nodeCoord.col(1);
	zz = nodeCoord.col(2);
	numDOF = 3 * numNode;
	displacement.resize(numDOF);
	displacement.setZero();
	force.resize(numDOF);
	force.setZero();


#pragma region 积分点坐标及权重
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
#pragma endregion
#pragma region 载荷   //计算结果与matlab相同
	fv << 0, 0, -76.982;
	for (int i = 0; i < numEle; i++)
	{
		VectorXd noindex;
		noindex .resize(nodeCoord.cols());
		noindex = EleNode.row(i);
		//cout << noindex << endl << endl;
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
			Matrix3d J= HEX_J(kesi, yita, zeta, xyz);
			//cout << (HEX_N(kesi, yita, zeta)) << endl << endl;
			//cout << ww(j)*(HEX_N(kesi, yita, zeta)).transpose()*J.determinant()*fv << endl << endl;

			elef = elef + ww(j)*(HEX_N(kesi, yita, zeta)).transpose()*J.determinant()*fv;
		}
		//cout << elef<<endl << endl;
		//cout << EleNode.cols() << endl;
		eleDof.resize(3*EleNode.cols());

		for (int j = 0; j < EleNode.cols(); j++)
		{
			eleDof(3 * j) = 3*noindex(j);
			eleDof(3 * j+1) =3* noindex(j)+1;
			eleDof(3 * j+2) = 3*noindex(j)+2;
		}
		//cout << eleDof << endl << endl;
		for (int j = 0; j < eleDof.size(); j++)
		{
			force(eleDof(j)) = force(eleDof(j)) + elef(j);
		}
		//cout << force << endl<<endl;
	}
	
	/*for (int i = 0; i < numDOF; i++)
	{
		ffff = ffff + force(i);

	}
	cout << ffff << endl;*/
	/*cout <<" force" << endl ;
	cout << force << endl << endl;*/
#pragma endregion
#pragma region 刚度矩阵			
	stiffness.resize(numDOF, numDOF);
	stiffness.setZero();
	elasticMatrixD.resize(6, 6);
	elasticMatrixD <<
		1, v / (1 - v), v / (1 - v), 0, 0, 0,
		v / (1 - v), 1, v / (1 - v), 0, 0, 0,
		v / (1 - v), v / (1 - v), 1, 0, 0, 0,
		0, 0, 0, (1 - 2 * v) / (1 - v) / 2, 0, 0,
		0, 0, 0, 0, (1 - 2 * v) / (1 - v) / 2, 0,
		0, 0, 0, 0, 0, (1 - 2 * v) / (1 - v) / 2;
	elasticMatrixD = elasticMatrixD * (1.0 - v)*E / (1 + v) / (1 - 2 * v);
	//cout << elasticMatrixD << endl;


	noindex.resize(nodeCoord.cols());
	for (int i = 0; i < numEle; i++)
	{			
		noindex = EleNode.row(i);
		MatrixXd xyz(8, 3);
		for (int j = 0; j < 8; j++)
		{
			xyz(j, 0) = xx(noindex(j));
			xyz(j, 1) = yy(noindex(j));
			xyz(j, 2) = zz(noindex(j));
		}
		MatrixXd eleK(24,24);
		eleK.setZero();
		for (int j = 0; j < 8; j++)
		{
			double kesi = kesi_yita(j, 0);
			double yita = kesi_yita(j, 1);
			double zeta = kesi_yita(j, 2);
			Matrix3d J = HEX_J(kesi, yita, zeta, xyz);
			Matrix3d R = J.inverse();
			MatrixXd A(6, 9);
			A <<
				R(0, 0), R(0, 1), R(0, 2), 0, 0, 0, 0, 0, 0,
				0, 0, 0, R(1, 0), R(1, 1), R(1, 2), 0, 0, 0,
				0, 0, 0, 0, 0, 0, R(2, 0), R(2, 1), R(2, 2),
				0, 0, 0, R(2, 0), R(2, 1), R(2, 2), R(1, 0), R(1, 1), R(1, 2),
				R(2, 0), R(2, 1), R(2, 2), 0, 0, 0, R(0, 0), R(0, 1), R(0, 2),
				R(1, 0), R(1, 1), R(1, 2), R(0, 0), R(0, 1), R(0, 2), 0, 0, 0;

			//cout << A << endl;

			MatrixXd G(9, 24);
			G = HEX_G(kesi, yita, zeta);
			//cout << G << endl;
			MatrixXd B(6, 24);
			B = A * G;
			eleK = eleK + ww(j)*B.transpose()*elasticMatrixD*B*J.determinant();
		}
		//cout << eleK << endl;
		//eleDof.resize(24);
		for (int k = 0; k < EleNode.cols(); k++)
		{
			eleDof(3 * k) = 3 * noindex(k);
			eleDof(3 * k + 1) = 3 * noindex(k) + 1;
			eleDof(3 *k + 2) = 3 * noindex(k) + 2;
		}
		//cout << eleDof << endl;
		for (int k = 0; k < eleDof.size();k++)
		{
			for (int m = 0; m < eleDof.size(); m++)
			{
				stiffness(eleDof(k), eleDof(m)) = stiffness(eleDof(k), eleDof(m))+eleK(k,m);
			}
		}
			

	}
	//cout << stiffness.row(0)<< endl << endl;			

#pragma endregion
#pragma region 激活自由度
	DOF.resize(numDOF);
	for (int i = 0; i < numDOF; i++)
	{
		DOF(i) = i;
	}
	activeDof.resize(numDOF - restrainedDof.size());
	activeDof = VectorXd_div(DOF, restrainedDof);

#pragma endregion
#pragma region 划行划列(刚度矩阵 力 位移)
	MatrixXd stiffness_1(activeDof.size(), activeDof.size());
	VectorXd force_1;
	VectorXd displacement_1;
	for (int i = 0; i < activeDof.size(); i++)
	{
		for (int j = 0; j < activeDof.size(); j++)
		{
			stiffness_1(i, j) = stiffness(activeDof(i), activeDof(j));
		}
	}
	force_1.resize(activeDof.size());
	displacement_1.resize(activeDof.size());
	for (int i = 0; i < activeDof.size(); i++)
	{
		force_1(i) = force(activeDof(i));
		displacement_1(i) = displacement(activeDof(i));
	}
#pragma endregion
#pragma region 计算位移
	displacement_1 = stiffness_1.inverse()*force_1;
	for (int i = 0; i < activeDof.size(); i++)
	{
		displacement(activeDof(i)) = displacement_1(i);
	}
	/*cout << "displacement:" << endl;
	for (int i = 0; i < numNode; i++)
	{
		cout << "node"<<i<<"\t"<<displacement(3*i) << "\t" << displacement(3 * i+1) << "\t" << displacement(3 * i+2) << endl;
	}*/
#pragma endregion	
#pragma region 单元应力
	stress.resize(numEle*8, 6);
	MatrixXd kesi_yita_Node(8, 3);
	kesi_yita_Node <<
		-1, -1, -1,
		1, -1, -1,
		1, 1, -1,
		-1, 1, -1,
		-1, -1, 1,
		1, -1, 1,
		1, 1, 1,
		-1, 1, 1;
	noindex.resize(nodeCoord.cols());
	for (int i = 0; i < numEle; i++)
	{		
		noindex = EleNode.row(i);
		MatrixXd xyz_node(8, 3);
		VectorXd dd(24);
		for (int j = 0; j < 8; j++)
		{
			xyz_node(j, 0) = xx(noindex(j));
			xyz_node(j, 1) = yy(noindex(j));
			xyz_node(j, 2) = zz(noindex(j));
			dd(3 * j) = displacement(noindex(j) * 3);
			dd(3 * j+1) = displacement(noindex(j) * 3+1);
			dd(3 * j+2) = displacement(noindex(j) * 3+2);

		}
		for (int j = 0; j < 8; j++)
		{
			double kesi = kesi_yita_Node(j, 0);
			double yita = kesi_yita_Node(j, 1);
			double zeta = kesi_yita_Node(j, 2);
			Matrix3d J = HEX_J(kesi, yita, zeta, xyz_node);
			//cout << J;
			Matrix3d R = J.inverse();
			MatrixXd A(6, 9);
			A <<
				R(0, 0), R(0, 1), R(0, 2), 0, 0, 0, 0, 0, 0,
				0, 0, 0, R(1, 0), R(1, 1), R(1, 2), 0, 0, 0,
				0, 0, 0, 0, 0, 0, R(2, 0), R(2, 1), R(2, 2),
				0, 0, 0, R(2, 0), R(2, 1), R(2, 2), R(1, 0), R(1, 1), R(1, 2),
				R(2, 0), R(2, 1), R(2, 2), 0, 0, 0, R(0, 0), R(0, 1), R(0, 2),
				R(1, 0), R(1, 1), R(1, 2), R(0, 0), R(0, 1), R(0, 2), 0, 0, 0;
			MatrixXd G(9, 24);
			G = HEX_G(kesi, yita, zeta);
			MatrixXd B(6, 24);
			
			B = A * G;
			//cout << elasticMatrixD << endl;
			
			stress.row(i*8+j) = elasticMatrixD * B*dd;
		}
	}
	/*cout <<"stress"<<endl<< "element" << "\t" << "node" << "\t" << "σ1" << "\t" << "σ2" << "\t" << "σ3" << "\t" << "t12" << "\t" << "t13" << "\t" << "t23" << endl;
	for (int i = 0; i < numEle; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << i << "\t" << EleNode(j) << "\t" << stress(i * 8 + j, 0) << "\t" << stress(i * 8 + j, 1) << "\t" << stress(i * 8 + j, 2) << "\t" << stress(i * 8 + j, 3) << "\t" << stress(i * 8 + j, 4) << "\t" << stress(i * 8 + j, 5);
			
		}
	}*/

#pragma endregion
#pragma region 支座反力
	reaction.resize(numDOF);
	reaction.setZero();
	for (int i = 0; i < restrainedDof.size(); i++)
	{
		int rownum = restrainedDof(i);
		reaction(rownum) = stiffness.row(rownum)*displacement - force(rownum);
	}
	//cout << "reaction:" << endl;
	//cout << reaction << endl;
#pragma endregion


    std::cout << "Hello World!\n"; 
}
Matrix3d HEX_J(double kesi, double yita, double zeta, MatrixXd xyz)	//Jacbian矩阵
{
	MatrixXd NN(3, 8);

	NN <<
		-((yita - 1)*(zeta - 1)) / 8, ((yita - 1)*(zeta - 1)) / 8, -((yita + 1)*(zeta - 1)) / 8, ((yita + 1)*(zeta - 1)) / 8, ((yita - 1)*(zeta + 1)) / 8, -((yita - 1)*(zeta + 1)) / 8, ((yita + 1)*(zeta + 1)) / 8, -((yita + 1)*(zeta + 1)) / 8,
		-((kesi - 1)*(zeta - 1)) / 8, ((kesi + 1)*(zeta - 1)) / 8, -((kesi + 1)*(zeta - 1)) / 8, ((kesi - 1)*(zeta - 1)) / 8, ((kesi - 1)*(zeta + 1)) / 8, -((kesi + 1)*(zeta + 1)) / 8, ((kesi + 1)*(zeta + 1)) / 8, -((kesi - 1)*(zeta + 1)) / 8,
		-((kesi - 1)*(yita - 1)) / 8, ((kesi + 1)*(yita - 1)) / 8, -((kesi + 1)*(yita + 1)) / 8, ((kesi - 1)*(yita +1)) / 8, ((kesi - 1)*(yita - 1)) / 8, -((kesi + 1)*(yita - 1)) / 8, ((kesi + 1)*(yita + 1)) / 8, -((kesi - 1)*(yita + 1)) / 8;
	
	Matrix3d J = NN * xyz;
	/*cout << NN << endl;
	cout << xyz << endl;*/
	return J;
}
MatrixXd HEX_N(double kesi, double yita, double zeta)			//形函数矩阵
{
	double N1 =  (1 - kesi)*(1 - yita)*(1 - zeta)/ 8;
	double N2 = (1 + kesi)*(1 - yita)*(1 - zeta) / 8;
	double N3 = (1 + kesi)*(1 + yita)*(1 - zeta) / 8;
	double N4 = (1 - kesi)*(1 + yita)*(1 - zeta) / 8;
	double N5 = (1 - kesi)*(1 - yita)*(1 + zeta) / 8;
	double N6 = (1 + kesi)*(1 - yita)*(1 + zeta) / 8;
	double N7 = (1 + kesi)*(1 + yita)*(1 + zeta) / 8;
	double N8 = (1 - kesi)*(1 +yita)*(1 + zeta) / 8;

	MatrixXd N(3, 24);
	N <<
		N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0, 0,
		0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0,
		0, 0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8;
	return N;
	

}
MatrixXd HEX_G(double kesi, double yita, double zeta)
{
	MatrixXd G(9, 24);
	G <<
		-(yita - 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta - 1) / 8, 0, 0, -(yita + 1)*(zeta - 1) / 8, 0, 0, (yita + 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta + 1) / 8, 0, 0, -(yita - 1)*(zeta + 1) / 8, 0, 0, (yita + 1)*(zeta + 1) / 8, 0, 0, -(yita + 1)*(zeta + 1) / 8, 0, 0,
		-(kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi + 1)*(zeta - 1) / 8, 0, 0, -(kesi + 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta + 1) / 8, 0, 0, -(kesi + 1)*(zeta + 1) / 8, 0, 0, (kesi + 1)*(zeta + 1) / 8, 0, 0, -(kesi - 1)*(zeta + 1) / 8, 0, 0,
		-(kesi - 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita + 1) / 8, 0, 0, -(kesi - 1)*(yita + 1) / 8, 0, 0,
		0, -(yita - 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta - 1) / 8, 0, 0, -(yita + 1)*(zeta - 1) / 8, 0, 0, (yita + 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta + 1) / 8, 0, 0, -(yita - 1)*(zeta + 1) / 8, 0, 0, (yita + 1)*(zeta + 1) / 8, 0, 0, -(yita + 1)*(zeta + 1) / 8, 0,
		0, -(kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi + 1)*(zeta - 1) / 8, 0, 0, -(kesi + 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta + 1) / 8, 0, 0, -(kesi + 1)*(zeta + 1) / 8, 0, 0, (kesi + 1)*(zeta + 1) / 8, 0, 0, -(kesi - 1)*(zeta + 1) / 8, 0,
		0, -(kesi - 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita + 1) / 8, 0, 0, -(kesi - 1)*(yita + 1) / 8, 0,
		0, 0, -(yita - 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta - 1) / 8, 0, 0, -(yita + 1)*(zeta - 1) / 8, 0, 0, (yita + 1)*(zeta - 1) / 8, 0, 0, (yita - 1)*(zeta + 1) / 8, 0, 0, -(yita - 1)*(zeta + 1) / 8, 0, 0, (yita + 1)*(zeta + 1) / 8, 0, 0, -(yita + 1)*(zeta + 1) / 8,
		0, 0, -(kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi + 1)*(zeta - 1) / 8, 0, 0, -(kesi + 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta - 1) / 8, 0, 0, (kesi - 1)*(zeta + 1) / 8, 0, 0, -(kesi + 1)*(zeta + 1) / 8, 0, 0, (kesi + 1)*(zeta + 1) / 8, 0, 0, -(kesi - 1)*(zeta + 1) / 8,
		0,	0, -(kesi - 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita + 1) / 8, 0, 0, (kesi - 1)*(yita - 1) / 8, 0, 0, -(kesi + 1)*(yita - 1) / 8, 0, 0, (kesi + 1)*(yita + 1) / 8, 0, 0, -(kesi - 1)*(yita + 1) / 8	;
	return G;
}
VectorXd VectorXd_div(VectorXd v1, VectorXd v2)
{
	VectorXd VectorXd_div;
	vector<double> v_1;
	vector<double> v_2;
	vector<int> v_intersection;
	vector<int> v_difference;

	for (int i = 1; i < v1.size(); i++)
	{
		v_1.push_back(v1(i));
	}
	for (int i = 1; i < v2.size(); i++)
	{
		v_2.push_back(v2(i));
	}
	sort(v_1.begin(), v_1.end());
	sort(v_2.begin(), v_2.end());

	set_difference(v_1.begin(), v_1.end(), v_2.begin(), v_2.end(), back_inserter(v_difference));
	VectorXd_div.resize(v1.size() - v2.size());
	for (int i = 0; i < VectorXd_div.size(); i++)
	{
		VectorXd_div(i) = v_difference[i];
	}
	return VectorXd_div;
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
