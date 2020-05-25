#pragma once
#include "SyntaxDirected.h"
class SimpleCompiler:public SyntaxDirected
{
	struct Id
	{
		bool computable = false;
		size_t priority = 0;
		double C = 0.0;
		double K = 1.0;
		std::string name;
	};
public:
	SimpleCompiler(std::string cfg_path):SyntaxDirected(cfg_path)
	{
		Initialization;
	}
	virtual void SetupSemanticActionTable() override
	{	
		// equal to AddSemanticAction("do_plus",do_plus);
		// use Macro in order to prevent spelling mistakes
		AddAction(do_E__to_E);
		AddAction(do_plus);
		AddAction(do_minus);
		AddAction(do_E_to_T);
		AddAction(do_multiply);
		AddAction(do_divide);
		AddAction(do_T_to_F);
		AddAction(do_compress);
		AddAction(do_getid);
	}
	static void* do_E__to_E(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		std::cout << "result:" << std::endl;
		const auto& res = GetValue(Id, 0);
		if(res.computable)
			std::cout << GetValue(Id, 0).name<< std::endl;
		else
		{
			if (res.priority == 0)
			{
				if (res.C > 0)
					std::cout << res.name << "+" << res.C << std::endl;
				else if (res.C < 0)
					std::cout << res.name << res.C << std::endl;
				else
					std::cout << res.name << std::endl;
			}
			else if (res.priority == 1)
			{
				std::cout << res.name << "*" << res.K << std::endl;
			}
				
		}
			
		return nullptr;
	}
	static void* do_E_to_T(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		return PassOn(0);
	}
	static void* do_plus(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		auto res = Create(Id);
		const auto& x = GetValue(Id, 0);
		const auto& y = GetValue(Id, 2);
		res->priority = 0;
		if (x.computable && y.computable)
		{
			res->computable = true;
			res->C = x.C + y.C;
			res->name = std::to_string(res->C);
			res->name.erase(5);
		}
		else
		{
			res->computable = false;
			if (!x.computable && y.computable)
			{
				if (x.priority ==0)
					// A+3  +4
				{
					res->C = x.C + y.C;				
					res->name = x.name;
				}
				else// A*3  +4
				{
					res->C = y.C;
					res->name = x.name + "*" + std::to_string(x.C).erase(5);
				}
			}
			else if (x.computable && !y.computable)
				//3+  A
			{
				res->C = y.C;
				res->name = x.name + "+" + y.name;
			}
			else
			{
				res->C = y.C;
				if (x.C == 0)
					res->name = x.name + "+" + y.name;
				else if (x.C > 0)
					res->name = x.name + "+" + std::to_string(x.C).erase(5) + "+" + y.name;
				else
					res->name = x.name + std::to_string(x.C).erase(5) + "+" + y.name;
			}
		}
		return res;
	}
	static void* do_minus(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		auto res = Create(Id);
		const auto& x = GetValue(Id, 0);
		const auto& y = GetValue(Id, 2);
		res->priority = 0;
		if (x.computable && y.computable)
		{
			res->computable = true;
			res->C = x.C - y.C;
			res->name = std::to_string(res->C);
			res->name.erase(5);
		}
		else
		{
			res->computable = false;
			if (!x.computable && y.computable)
			{
				if (x.priority == 0)
					// A+3  -4
				{
					res->C = x.C - y.C;
					res->name = x.name;
				}
				else// A*3  -4
				{
					res->C = y.C;
					res->name = x.name + "*" + std::to_string(x.C).erase(5);
				}
			}
			else if (x.computable && !y.computable)
				//3 - A
			{
				res->C = y.C;
				res->name = x.name + "-" + y.name;
			}
			else
			{
				res->C = y.C;
				if (x.C == 0)
					res->name = x.name + "-" + y.name;
				else if (x.C > 0)
					res->name = x.name + "+" + std::to_string(x.C).erase(5) + "-" + y.name;
				else
					res->name = x.name + std::to_string(x.C).erase(5) + "-" + y.name;
			}
		}
		return res;
	}
	static void* do_multiply(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		auto res = Create(Id);
		const auto& x = GetValue(Id, 0);
		const auto& y = GetValue(Id, 2);
		res->priority = 0;
		if (x.computable && y.computable)
		{
			res->computable = true;
			res->C = x.C * y.C;
			res->name = std::to_string(res->C);
			res->name.erase(5);
		}
		else
		{
			res->computable = false;
			if (!x.computable && y.computable)
			{
				//A*3 * 4
				res->K = x.K * y.C;
				res->name = x.name;
			}
			else if (x.computable && !y.computable)
				//3 * A
			{
				res->K = y.K;
				res->name = x.name + "*" + y.name;
			}
			else// A * 4 * B * 4
			{
				res->K = y.K;
				if (x.K == 1)
					res->name = x.name + "*" + y.name;
				else
					res->name = x.name + "*" + std::to_string(x.K).erase(5) + "*" + y.name;
			}
		}
		return res;
	}
	static void* do_divide(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{	
		auto res = Create(Id);
		const auto& x = GetValue(Id, 0);
		const auto& y = GetValue(Id, 2);
		res->priority = 0;
		if (x.computable && y.computable)
		{
			res->computable = true;
			res->C = x.C / y.C;
			res->name = std::to_string(res->C);
			res->name.erase(5);
		}
		else
		{
			res->computable = false;
			if (!x.computable && y.computable)
			{
				//A*3 / 4
				res->K = x.K / y.C;
				res->name = x.name;
			}
			else if (x.computable && !y.computable)
				//3 / A 
			{
				res->K = y.K;
				res->name = x.name + "/" + y.name;
			}
			else// A*3 / B
			{
				res->K = y.K;
				if (x.K == 1)
					res->name = x.name + "/" + y.name;
				else
					res->name = x.name + "*" + std::to_string(x.K).erase(5) + "/" + y.name;
			}
		}
		return res;
	}
	static void* do_T_to_F(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		return PassOn(0);
	}
	static void* do_compress(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		const auto& value = GetValue(Id, 1);
		auto res = Create(Id);
		if (value.computable == false)
		{
			res->computable = false;
			if (value.C == 0)
				res->name = "(" + value.name + ")";
			else if (value.C > 0)
				res->name = "(" + value.name + "+" + std::to_string(value.C).erase(5) + ")";
			else
				res->name = "(" + value.name + std::to_string(value.C).erase(5) + ")";
			res->C = 0;
		}
		else
		{
			res->computable = true;
			res->C = value.C;
			res->name = value.name;
		}
		return res;
	}
	static void* do_getid(SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set)
	{
		auto res = Create(Id);
		if (token_set[token_iter].type == Scanner::TokenType::digit)
		{
			res->computable = true;
			res->name = token_set[token_iter].name;
			res->C = std::stod(token_set[token_iter].name);
		}
		else
		{
			res->computable = false;
			res->name = token_set[token_iter].name;
		}
		return res;
	}
};


