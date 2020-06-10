# Toy Compiler Compiler

from MuGoudan.

## Quick start



## How to start a simple Custom Compiler

```cpp
// 1. correct inheritance
class ExampleCompiler :public SyntaxDirected<ExampleCompiler>
{
public:
	ExampleCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
        // 2. call Initialization() to get everything ready
		Initialization();
        //do your Initialization
	}
}
```

if **Semantic Actions** are not required, then everything is done. Input your to-be-processed file and enjoy it.

```cpp
void main()
{
    //we have prepared a to-be-processed text for you:"sample_test.txt"
    auto input = FileLoader("mytxt.txt");
	auto token_set = EasyScanner(input);
    std::cout<<"Scanner:"<<std::endl;
    for(const auto& token:token_set)
    	std::cout<<token<<std::endl;
    std::cout<<"Parser:"<<std::endl;
    //we have prepared a "Syntax Definition file" for you:"complete_syntax.syn"
	ExampleCompiler exampleCompiler("my_syntax.syn");
	ExampleCompiler.Parse(token_set);
}
```
feel bored? uncomment these(in ``SyntaxDirected.h``) to get some **feedback** or to see the **Process**

```cpp
//#define SEMANTIC_CHECK							//almostly useless,but you can have a try.
//#define SHOW_CATCHED_VAR							//show variables catched in your "Syntax Definition file" 
//#define HIGH_LIGHT								//show your SyntaxDefinition file in Hightlight 
//#define SHOW_PARSE_PROCESS						//when parsing token_set,show the "Reduce Process"
```

