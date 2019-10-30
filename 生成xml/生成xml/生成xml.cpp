// 生成xml.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include "pch.h"
#include <iostream>
#include<fstream>

#include "tinyxml2.h"
#include <string>


using namespace std;
using namespace tinyxml2;

int createXML(const char* xmlPath);
int selectXML(const char* xmlPath);
int readXML(const char* xmlPath);
//string GBKToUTF8(const string& strGBK);



int main()
{
	createXML("TrussElement.xml");

	readXML("TrussElement.xml");
    std::cout << "Hello World!\n"; 
}
int createXML(const char* xmlPath)
{
	XMLDocument doc;
	//if (3 != doc.LoadFile(xmlPath))
	//{
	//	cout << "file has been existed !" << endl;
	//	return 0;
	//}

	//添加申明可以使用如下两行
	/*XMLDeclaration* declaration = doc.NewDeclaration("2DTruss");
	doc.InsertFirstChild(declaration);*/

	XMLElement* root = doc.NewElement("TrussElement");
	doc.InsertEndChild(root);
	XMLElement* param1 = doc.NewElement("paramclass");
	param1->SetAttribute("name", "Truss单元参数");
	param1->SetAttribute("value", "");
	root->LinkEndChild(param1);
#pragma region 节点坐标
	XMLElement* param2 = doc.NewElement("param");
	param2->SetAttribute("name", "节点坐标");
	param2->SetAttribute("type", "table");
	param2->SetAttribute("operate", "add");
	param1->InsertEndChild(param2);
	XMLElement *param3 = doc.NewElement("row");
	param3->SetAttribute("value", "x，y");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "double，double");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "-4500，0");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "-1500，0");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "1500，0");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "4500，0");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "-3000，3000");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "0，3000");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "3000，3000");
	param2->LinkEndChild(param3);
#pragma endregion
#pragma region 单元的节点编号
	param2 = doc.NewElement("param");
	param2->SetAttribute("name", "单元的节点编号");
	param2->SetAttribute("type", "table");
	param2->SetAttribute("operate", "add");
	param1->InsertEndChild(param2);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "节点1，节点2");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "int,int");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "0，1");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "1，2");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "2，3");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "4，5");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "5，6");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "0，4");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "1，5");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "2，6");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "1，4");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "2，5");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "3，6");
	param2->LinkEndChild(param3);
#pragma endregion
#pragma region 载荷
	param2 = doc.NewElement("param");
	param2->SetAttribute("name", "载荷");
	param2->SetAttribute("type", "table");
	param2->SetAttribute("operate", "add");
	param1->InsertEndChild(param2);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "自由度，力");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "int,double");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "9，-100000");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "11，-100000");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "13，-100000");
	param2->LinkEndChild(param3);

#pragma endregion
#pragma region 约束
	param2 = doc.NewElement("param");
	param2->SetAttribute("name", "约束");
	param2->SetAttribute("type", "table");
	param2->SetAttribute("operate", "add");
	param1->InsertEndChild(param2);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "自由度");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "int");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "0");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "1");
	param2->LinkEndChild(param3);
	param3 = doc.NewElement("row");
	param3->SetAttribute("value", "7");
	param2->LinkEndChild(param3);

#pragma endregion


	return doc.SaveFile(xmlPath);
}
int selectXML(const char* xmlPath)
{
	XMLDocument doc;
	if (doc.LoadFile(xmlPath) != 0)
	{
		cout << "load xml file failed" << endl;
		return false;
	}
	XMLElement* root = doc.RootElement();
	XMLElement* userNode = root->FirstChildElement("User");
	while (userNode != NULL)
	{
		if (0 == strncmp("liangbaikai", (userNode->Attribute("Name")), 11))
		{
			cout << userNode->Attribute("Name") << endl;
			cout << userNode->Attribute("Password") << endl;
			cout << userNode->FirstChildElement("Age")->GetText() << endl;
			cout << userNode->FirstChildElement("Gender")->GetText() << endl;
			cout << userNode->FirstChildElement("Mobile")->GetText() << endl;
			cout << userNode->FirstChildElement("Email")->GetText() << endl;
			userNode = userNode->NextSiblingElement();
		}
		else
		{
			userNode = userNode->NextSiblingElement();
		}
	}
	return 0;
}
int readXML(const char* xmlPath)
{
	XMLDocument doc;
	/*if (doc.LoadFile("test.xml"))
	{
		doc.PrintError();
		exit(1);
	}*/

	// 根元素
	XMLElement* TrussElement = doc.RootElement();
	//cout << "name:" << TrussElement->Attribute("name") << endl << endl;

	// 遍历<surface>元素
	XMLElement* paramclass = TrussElement->FirstChildElement("paramclass");
	while (paramclass)
	{
		// 遍历属性列表
		const XMLAttribute* param = paramclass->FirstAttribute();
		while (param)
		{
			cout << param->Name() << ":" << param->Value() << "  ";
			param = param->Next();
		}
		cout << endl;

		// 遍历子元素
		/*XMLElement* surfaceChild = param->FirstChildElement();
		while (surfaceChild)
		{
			cout << surfaceChild->Name() << " = " << surfaceChild->GetText() << endl;
			surfaceChild = surfaceChild->NextSiblingElement();
		}
		cout << endl;

		surface = surface->NextSiblingElement("surface");*/
	}

	return 0;
	
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
