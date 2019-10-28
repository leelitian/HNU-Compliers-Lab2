#include "globals.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unordered_map>
using namespace std;

extern ifstream source;

static string lineBuf;        // 从文件读入一行作为缓冲
static size_t linepos;        // 在一行中的索引
static int lineno;            // 行号
static bool EOF_flag = false; // 用于纠正到EOF时回退函数的功能

bool EchoSource = true;       // 显示源文件
bool TraceScan = true;        // 显示词法分析结果

/* 从缓冲区获取下一个字符 */
char getNextChar(void)
{
	/* 当前行读取完毕，尝试读取下一行 */
	if (linepos >= lineBuf.length())
	{
		lineno++;
		if (getline(source, lineBuf))
		{
			lineBuf += "\n";
			if (EchoSource) cout << setw(4) << lineno << ": " << lineBuf;
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = true;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

/* 回退一个字符 */
void ungetNextChar()
{
	if (!EOF_flag) linepos--;
}

/* 关键字表 <关键字，类型> */
unordered_map<string, TokenType> reservedMap =
{
	{"else", ELSE}, {"if", IF}, {"int", INT},
	{"return", RETURN}, {"void", VOID}, {"while", WHILE}
};

/* 查关键字表，判断字符串的TokenType是ID还是reserved word */
TokenType reservedLookup(string s)
{
	return (reservedMap.find(s) == reservedMap.end()) ? ID : reservedMap[s];
}

/* 打印词法分析的结果 */
void printToken(TokenType token, string tokenString)
{
	switch (token)
	{
	/* 关键字类型 */
	case ELSE:
	case IF:
	case INT:
	case RETURN:
	case VOID:
	case WHILE:
		cout << "reserved word: " << tokenString << endl;
		break;
	/* 专用符号类型 */
	case PLUS:
		cout << "+\n";
		break;
	case MINUS:
		cout << "-\n";
		break;
	case TIMES:
		cout << "*\n";
		break;
	case OVER:
		cout << "/\n";
		break;
	case LT:
		cout << "<\n";
		break;
	case LE:
		cout << "<=\n";
		break;
	case GT:		
		cout << ">\n";
		break;
	case GE:
		cout << ">=\n";
		break;
	case EQ:
		cout << "==\n";
		break;
	case NEQ:
		cout << "!=\n";
		break;
	case ASSIGN:
		cout << "=\n";
		break;
	case SEMI:
		cout << ";\n";
		break;
	case COMMA:
		cout << ",\n";
		break;
	case LPAREN:
		cout << "(\n";
		break;
	case RPAREN:
		cout << ")\n";
		break;
	case LBRACKET:
		cout << "[\n";
		break;
	case RBRACKET:
		cout << "]\n";
		break;
	case LBRACE:
		cout << "{\n";
		break;
	case RBRACE:
		cout << "}\n";
		break;
	/* 其他类型 */
	case ENDFILE:
		cout << "EOF\n";
		break;
	case ERROR:
		cout << "ERROR: " << tokenString << endl;
		break;
	case ID:
		cout << "ID, name = " << tokenString << endl;
		break;
	case NUM:
		cout << "NUM, val = " << tokenString << endl;
		break;
	default:
		cout << "Unknown token :" << token << endl;
		break;
	}
}

/* 获取下一个词法单元的类型 */
TokenType getToken(void)
{
	string tokenString;
	TokenType currentToken;
	StateType state = START;
	bool save; // 是否保存到 tokenString
	while (state != DONE)
	{
		char c = getNextChar();
		save = true;
		switch (state)
		{
		case START:
		{
			if (isdigit(c)) state = INNUM;
			else if (isalpha(c)) state = INID;
			else if ((c == ' ') || (c == '\t') || (c == '\n')) save = false;
			else if (c == '=') state = INEQ;
			else if (c == '<') state = INLE;
			else if (c == '>') state = INGE;
			else if (c == '!') state = INNEQ;
			else if (c == '/') state = LBUF;
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = false;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '[':
					currentToken = LBRACKET;
					break;
				case ']':
					currentToken = RBRACKET;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case ',':
					currentToken = COMMA;
					break;
				default:
					/* ERROR: 出现其它未定义字符 */
					currentToken = ERROR;
					break;
				}
			}
			break;
		}
		case LBUF:
		{
			if (c == '*')
			{
				/* 进入INCOMMENT状态，清空tokenString */
				tokenString.clear();
				save = false;
				state = INCOMMENT;
			}
			else if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else
			{
				ungetNextChar();
				currentToken = OVER;
				state = DONE;
			}
			break;
		}
		case INCOMMENT:
		{
			save = false;
			if (c == '*') state = RBUF;
			else if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			break;
		}
		case RBUF:
		{
			save = false;
			if (c == '/') state = START;
			else if (c == '*') state = RBUF;
			else if (c == EOF)
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else state = INCOMMENT;
			break;
		}
		case INNUM:
		{
			if (!isdigit(c))
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = NUM;
			}
			break;
		}
		case INID:
		{
			if (!isalpha(c))
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = ID;
			}
			break;
		}
		case INEQ:
		{
			if (c == '=')
			{
				state = DONE;
				currentToken = EQ;
			}
			else
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = ASSIGN;
			}
			break;
		}
		case INLE:
		{
			if (c == '=')
			{
				state = DONE;
				currentToken = LE;
			}
			else
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = LT;
			}
			break;
		}
		case INGE:
		{
			if (c == '=')
			{
				state = DONE;
				currentToken = GE;
			}
			else
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = GT;
			}
			break;
		}
		case INNEQ:
		{
			if (c == '=')
			{
				state = DONE;
				currentToken = NEQ;
			}
			else
			{
				ungetNextChar();
				save = false;
				state = DONE;
				/* ERROR: !后面出现不是=的字符 */
				currentToken = ERROR;
			}
			break;
		}
		case DONE:
		default:
		{
			cout << "Scanner Bug:state=" << state << "\n";
			state = DONE;
			currentToken = ERROR;
			break;
		}
		}

		if(save) tokenString += c;
		/* 对于ID，需要检查它是否为关键字类型 */
		if (state == DONE && currentToken == ID)
			currentToken = reservedLookup(tokenString);
	}
	
	/* 打印行号和词法单元的信息 */
	if (TraceScan)
	{
		cout << "\t" << lineno << ": ";
		printToken(currentToken, tokenString);
	}
	return currentToken;
}