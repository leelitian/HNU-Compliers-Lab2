#pragma once

/* 词法单元的类型 */
enum TokenType
{
	ENDFILE, ERROR,
	/* 保留字 */
	ELSE, IF, INT, RETURN, VOID, WHILE,
	/* 多字符词法单元 */
	ID, NUM,
	/* 特殊字符 */
	PLUS, MINUS, TIMES, OVER, LT, LE, GT, GE, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE
//	  +     -      *     /     <  <=   >  >=  ==  !=     =      ;      ,       (       )       [         ]         {      }
};

/* DFA状态 */
enum StateType { START, LBUF, RBUF, INCOMMENT, INNUM, INID, INEQ, INLE, INGE, INNEQ, DONE };

TokenType getToken(void);