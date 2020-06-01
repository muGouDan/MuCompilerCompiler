## To Build
```cpp
class __Example:public SyntaxDirected<ILGenerator>
{
public:
	__Example(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization();
	}
	virtual void SetupSemanticActionTable() override
	{

	}
};
```
```cpp
//Macro
param					    INPUT;						
std::vector<Token> 			 TokenSet;					
size_t 					    TokenIter;					
Token 						CurrentToken;				
your_class                    This;
    
void 						AddAction(action);			
void 						Initialization();			    
Type 						GetValue(Type,index);			
void* 						PassOn(index);				
Type*						Create(Type,...);			
Type*						CreateAs(Type,expr);			
decltype(expr)	 			 CreateFrom(expr);			
```

