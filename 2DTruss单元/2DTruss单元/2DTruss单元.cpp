// 2DTruss单元.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "tinyxml2.h"
#include <string>

using namespace tinyxml2;
using namespace Eigen;
using namespace std;


double E = 200000;								//弹性模量(MPa)
double A = 4532;									//截面积（mm^2）
//double scalefactor = 0.2;
MatrixXd NodeCoord;
MatrixXd EleNode;
int numNode=0 ;		//节点数量
int numEle=0 ;		//单元数量

//MatrixXf restrainedDof(1,3);
VectorXd DOF;		//自由度
VectorXd activeDof;		//激活的自由度
VectorXd restrainedDof;//约束的自由度
VectorXd xx;		//节点的x坐标[numNode * 1]
VectorXd yy;		//节点的y坐标[numNode * 1]
int numDOF=0;			//自由度总数 numNode*2
int numRestrainedDof=0;
int numActiveDof=0;
VectorXd displacement;	//节点的整体位移 [numDof*1]
VectorXd force;		//节点力[numDOF * 1]
MatrixXd stiffness;	//刚度矩阵 [numDOF*numDOF]


VectorXd sigma;	//单元的轴向应力 [numEl*1]
VectorXd reaction;

//MatrixXf displacement(7, 1);
VectorXd VectorXd_div(VectorXd v1, VectorXd v2);
void readXml(const char * xmlplath);
vector<string> splitto1Dstring(const string &s, const string &seperator);

int main()
{

	force.setZero();
	readXml("TrussElement.xml");

	numActiveDof = numDOF - numRestrainedDof;
	for (int i = 0; i < numDOF; i++)
	{
		DOF(i) = i;
	}
	activeDof.conservativeResize(numActiveDof);
	activeDof = VectorXd_div(DOF, restrainedDof);
	
	cout << " NodeCoord:"<<endl<<NodeCoord<<endl;
	cout << " EleNode :" << endl << EleNode << endl;
	cout << "force  :" << endl << force<< endl;
	cout << "restrainedDof  :" << endl << restrainedDof << endl;
	//cout << " :" << endl <<  << endl;
	cout << "计算......" << endl;

	for (int i = 0; i < NodeCoord.rows(); i++)
	{
		xx(i) = NodeCoord(i, 0);
		yy(i) = NodeCoord(i, 1);
	}
	/*cout << xx<<endl;
	cout << yy<<endl;
	cout << force << endl;*/

#pragma region 刚度矩阵
	stiffness.setZero();
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
	
	cout << "stiffness:"<<endl<<stiffness<<endl;
#pragma endregion
//#pragma region 集合的差集
//	vector<int> v_1{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13 };
//	vector<int> v_2{ 0,1,7 };
//	sort(v_1.begin(), v_1.end());//排序
//	sort(v_2.begin(), v_2.end());
//	vector<int> v_intersection;
//	vector<int> v_difference;
//	set_difference(v_1.begin(), v_1.end(), v_2.begin(), v_2.end(), back_inserter(v_difference)	);
//	for (int i = 0; i < 11; i++)
//	{
//		activeDof(i) = v_difference[i];
//	}
//	//cout << activeDof<<endl;
//	
//#pragma endregion	
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
	//cout << stiffness_1<<endl;

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
	 cout << "displacement:" << endl<< displacement << endl;
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
	 reaction.resize(numRestrainedDof);
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
VectorXd VectorXd_div(VectorXd v1, VectorXd v2)//数组的差
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
void readXml(const char * xmlplath)
{
	XMLDocument doc;
	if (doc.LoadFile(xmlplath))
	{
		doc.PrintError();
		exit(1);
	}
	XMLElement* param = doc.RootElement();
	//cout << "name:" << scene->Attribute("name") << endl << endl;
	XMLElement* param1 = param->FirstChildElement("paramclass");
	
	/*for (int i = 0; i<int(param1.childNodes.size()); i++)
	{
		if tp.childNodes[i].attributes["name"] == "仿真步长(s)")
		{
		h = atof(UTF8ToGBK(tp.childNodes[i].attributes["value"]).c_str());
		cout << "仿真步长=" << atof(UTF8ToGBK(tp.childNodes[i].attributes["value"]).c_str()) << endl;
		}
	}*/
	while (param1)
	{		
		vector<string> result;
		// 遍历属性列表
		/*const XMLAttribute* surfaceAttr = param1->FirstAttribute();
		while (surfaceAttr)
		{
			cout << surfaceAttr->Name() << ":" << surfaceAttr->Value() << "  ";
			surfaceAttr = surfaceAttr->Next();
		}
		cout << endl;*/

		// 遍历子元素
		XMLElement* param2 = param1->FirstChildElement();
		while (param2)
		{
			cout << param2->Attribute("name")<<endl;
			string paramName =( param2->Attribute("name"));
			XMLElement* param3 = param2->FirstChildElement("row");
			param3 = param3->NextSiblingElement("row");
			param3 = param3->NextSiblingElement("row");
			while (param3)
			{					
				result=(splitto1Dstring(param3->Attribute("value"), ","));

				if (paramName== "节点坐标")
				{	
					NodeCoord.conservativeResize(numNode + 1,2);
					
					NodeCoord(numNode,0) = atof(result[0].c_str());
					NodeCoord(numNode, 1) = atof(result[1].c_str());
					//cout<< param3->Attribute("value")<<endl;

					numNode = numNode + 1;
					numDOF = numNode * 2;

					DOF.resize(numDOF);
					DOF.setZero();

					force.conservativeResize(numDOF);
					force.setZero();

					displacement.resize(numDOF);
					displacement.setZero();

					stiffness.resize(numDOF, numDOF);
					stiffness.setZero();

					xx.resize(numNode);
					xx.setZero();
					yy.resize(numNode);
					yy.setZero();

					param3 = param3->NextSiblingElement("row");								
				}
				if (paramName == "单元的节点编号")
				{				
					EleNode.conservativeResize(numEle + 1, 2);

					EleNode(numEle,0) = atoi(result[0].c_str());
					EleNode(numEle, 1) = atoi(result[1].c_str());
					//cout << param3->Attribute("value") << endl;
					numEle = numEle + 1;
					param3 = param3->NextSiblingElement("row");
				}
				if (paramName == "载荷")
				{		
					
					force(atoi(result[0].c_str()))= atof(result[1].c_str());

					//cout << param3->Attribute("value") << endl;
					param3 = param3->NextSiblingElement("row");
				}
				if (paramName == "约束的自由度")
				{	

					
					restrainedDof.conservativeResize(numRestrainedDof+1);
					restrainedDof(numRestrainedDof)= atof(result[0].c_str());				

					numRestrainedDof = numRestrainedDof + 1;

					//cout << param3->Attribute("value") << endl;
					param3 = param3->NextSiblingElement("row");		

				}
			}
			
//			cout << param2->Name() << " = " << param2->GetText() << endl;
			param2 = param2->NextSiblingElement("param");
		}
		cout << "numNode="<<numNode<<endl;
		cout << "numEle=" << numEle<<endl;
		cout << endl;

		param1 = param1->NextSiblingElement("paramclass");
	}


}
vector<string> splitto1Dstring(const string &s, const string &seperator)
{
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size())
	{
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0)
		{
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x])
				{
					++i;
					flag = 0;
					break;
				}
		}
		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0)
		{
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x])
				{
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j)
		{
			result.push_back((s.substr(i, j - i).c_str()));
			i = j;
		}
	}
	return result;
}
// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
