#pragma once
#include "SyntaxDirected.h"
class SimpleCompiler:public SyntaxDirected
{
	static std::stack<double> E;
	static std::stack<double> T;
	static std::stack<double> F;
public:
	SimpleCompiler(std::string cfg_path):SyntaxDirected(cfg_path)
	{
		Initialization;
	}
	virtual void SetupSemanticActionTable() override
	{	
		// equal to AddSemanticAction("do_plus",do_plus);
		// use Macro in order to prevent spelling mistakes
		AddAction(do_E_to_E_);
		AddAction(do_plus);
		AddAction(do_minus);
		AddAction(do_E_to_T);
		AddAction(do_multiply);
		AddAction(do_divide);
		AddAction(do_T_to_F);
		AddAction(do_compress);
		AddAction(do_getid);
	}
	static void do_E_to_E_(size_t token_iter, const Token_Set& token_set)
	{
		E.pop();
	}
	static void do_E_to_T(size_t token_iter, const Token_Set& token_set)
	{
		E.push(T.top());
		T.pop();
	}
	static void do_plus(size_t token_iter, const Token_Set& token_set)
	{
		auto res = E.top() + T.top();
		std::cout << E.top() << "+" << T.top() << "=" << res << std::endl;
		E.pop();
		T.pop();
		E.push(res);
	}
	static void do_minus(size_t token_iter, const Token_Set& token_set)
	{
		auto res = E.top() - T.top();
		std::cout << E.top() << "-" << T.top() << "=" << res << std::endl;
		E.pop();
		T.pop();
		E.push(res);
	}
	static void do_multiply(size_t token_iter, const Token_Set& token_set)
	{
		auto res = T.top() * F.top();
		std::cout << T.top() << "*" << F.top() << "=" << res << std::endl;
		T.pop();
		F.pop();
		T.push(res);
	}
	static void do_divide(size_t token_iter, const Token_Set& token_set)
	{
		auto res = T.top() / F.top();
		std::cout << T.top() << "/" << F.top() << "=" << res << std::endl;
		T.pop();
		F.pop();
		T.push(res);
	}
	static void do_T_to_F(size_t token_iter, const Token_Set& token_set)
	{
		T.push(F.top());
		F.pop();
	}
	static void do_compress(size_t token_iter, const Token_Set& token_set)
	{
		F.push(E.top());
		E.pop();
	}
	static void do_getid(size_t token_iter, const Token_Set& token_set)
	{
		F.push(std::stod(token_set[token_iter].name));
	}
};


