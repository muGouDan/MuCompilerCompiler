# Mu Compiler Compiler

Toy Compiler Compiler from MuGoudan.

## Preview

### from Definition to Symbol Table

![success](.\pic\success.png)

### Syntax Error

![syntax_error](.\pic\syntax_error.png)

### Semantic Error

![semantic_error](.\pic\semantic_error.png)

### Config File

(Notepad Syntax Highlight File provided in `./myCompiler/misc`)

![config](.\pic\config.png)

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

feel bored? uncomment these(in `CustomCodeMacro.h`in filter `AutoSyntax`) to get some **feedback** or to see the **Process**

```cpp
// DEBUG OPTIONS:
// Custom Syntax File Debug Options:
//#define CUSTOM_SYNTAX_FILE_DEBUG
//#define SEMANTIC_CHECK
//#define SHOW_CATCHED_VAR
//#define HIGH_LIGHT

// Input Parsing Debug Options:
//#define SHOW_PARSE_PROCESS
```

