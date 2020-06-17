# Mu Compiler Compiler

Toy Compiler Compiler from MuGoudan.

## Preview

### from Definition to Symbol Table

![success](https://github.com/muGouDan/myCompiler/blob/develop/pic/success.png)

### Syntax Error

![syntax_error](https://github.com/muGouDan/myCompiler/blob/develop/pic/syntax_error.png)

### Semantic Error

![semantic_error](https://github.com/muGouDan/myCompiler/blob/develop/pic/semantic_error.png)

### Config File

(Notepad Syntax Highlight File provided in `./misc`)

![config](https://github.com/muGouDan/myCompiler/blob/develop/pic/config.png)

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

## How to create your own Syntax File

new a text file in the workplace with extension ".syn", edit it with Notepad(or any other text editor, but a syntax highlight file is offered to work with Notepad in `./misc/syn_highlight`), input the highlight file.

## Write your own Syntax

there're two parts in the Syntax Definition File, separated by `$$`. (get a view of "complete_syntax.syn")

The first part describes how to transfer a **Lexeme Token** to a **Terminator**. And the second part describes the **Produtions** and the **function names of Semantic Action**.

### Transfer Token to  CFG Terminator

First part can be written as:

```syn
// 1
bTy		->	keyword && "int";
bTy		->	keyword && "float";
bTy		->	keyword && "char";
record	->	keyword && "struct";
record	->	keyword && "class";
// 3
str		->	raw_string;
num		->	digit;
id		->	identifier;
cTy		->	custom_type;
```

1. the first line means: every token with type `TokenType::keyword `(defined in `BaseWord.h`) and named "int" will be cast to bTy (short for Base Type). 

```syn
// 2
bTy		->	"int";
```

2. this is also ok, but not so safe, cause every token named "int" will be cast to bTy even though some are just raw string. Token will be prior cast to certain **Terminator** with strong restriction. e.g `bTy->keyword && "int"` has higher priority than `bTy2->"int"`, when the Token's type is `TokenType::keyword ` with name "int".

3. the line `num		->	digit;` means: every token with type `TokenType::digit`(defined in `BaseWord.h`) will be cast to num. This kind of rule --"Type-Only Rule", has the lowest priority, after "Type&&Name Rule" and "Name-Only Rule".

**Type&&Name Rule** is useful in specifying keyword.

**Type-Only Rule** is useful in specifying something general, like numbers, variable identifiers and some types that will not be used in Scanning Phase, but can be set by Semantic Actions, like custom_type. ( type `TokenType::identifier ` of a token may be replaced by `TokenType::custom_type` when Parsing.)

BE CAREFUL when using **Only-Name Rule**.

### Proudctions

pay attention to **Scope Label**, **Production**, **Function Name**.
