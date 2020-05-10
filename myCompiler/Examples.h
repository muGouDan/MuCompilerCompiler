#pragma once
#include "SetHelper.h"
namespace Example
{
	namespace LL1
	{
		// for LL1
		enum symbol
		{
			E,
			E_,
			T,
			T_,
			F,
			epsilon,
			//term		column
			mul,		//*			0
			plus,		//+			1
			lp,			//(			2
			rp,			//)			3
			id,			//			4
			end			//$			5
		};

		SetHelper<symbol>::Production_Table pro =
		{
			// E
			{
				{T,E_}			//E -> TE'
			},
			// E_
			{
				{plus,T,E_},	//E' -> +TE'
				{epsilon}		//E' -> epsilon
			},
			// T
			{
				{F,T_}			//T -> FT'
			},
			// T_
			{
				{mul,F,T_},		//T' -> *FT'
				{epsilon}		//T' -> epsilon
			},
			// F
			{
				{lp,E,rp},		//F -> (E)
				{id}			//F -> id
			}

		};
	}

	namespace LR
	{
		// for LR
		enum symbol
		{
			E_,
			E,
			T,
			F,
			epsilon,
			plus,
			mul,
			lp,
			rp,
			id,
			end
		};

		SetHelper<symbol>::Production_Table pro =
		{
			// E_->E
			{
				{E}
			},
			// E->E+T|T
			{
				{E,plus,T},
				{T}
			},
			// T->T*F|F
			{
				{T,mul,F},
				{F}
			},
			// F
			{
				{lp,E,rp},
				{id}
			}
		};
	}
}