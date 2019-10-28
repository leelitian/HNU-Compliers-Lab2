#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

#include "globals.h"

ifstream source;

int main(int argc, char * argv[])
{
	/* 命令行需要有两个参数：<本程序名> <源码文件名>*/
	if (argc != 2)
	{
		cerr << "usage: " << argv[0] << " <filename>" << endl;
		exit(1);
	}

	/* 加上后缀.cminus */
	string sourceFile = argv[1];
	if (sourceFile.find(".") == string::npos)
		sourceFile += ".cminus";

	/* 尝试打开文件 */
	source.open(sourceFile);
	if (!source)
	{
		cerr << "File " << sourceFile << " not found" << endl;
		exit(2);
	}

	/* 词法分析 */
	while (getToken() != ENDFILE);
	source.close();

	return 0;
}