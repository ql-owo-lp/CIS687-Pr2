///////////////////////////////////////////////////////////////
// This file is a designed test file, used to test that if I can find all
// of the type definitions
// I try to include every possible situation here, to test the correctness of
// my actions and rules
// The codes are formatted in strange way on purpose
// - Kevin Wang  2013 Spring
///////////////////////////////////////////////////////////////

#ifndef EXTENDEDACTIONSANDRULES_H
#define EXTENDEDACTIONSANDRULES_H

namespace // here we test a new line character, do not use tok[i+1]
Namespace1 {	// a test for namespace
	struct Struct0 : Struct1 {};struct Struct1 : Struct2 {std::string field1;Struct2 field2;};

	struct Struct2 : Struct3 {std::string field1;/*describe which type it is, class/struct/enum?*/Struct4 field2;
	} structVar2;

	class Class0 {public:private:class NestedClass0 {} field1;};	// Namespace1::Class0
	// NOTICE there is a nested class
}

struct Struct3 {std::string field1;/*describe which type it is, class/struct/enum?*/} structVar3;
union Union0 {}:
struct Struct4 {public:Struct0 field1;private:};

enum {
	MONDAY, THUSDAY, WEDNESDAY
} AnonymousEnumVar1;	// an anonymous enum, NOTICE here we have a global variable

enum Enum1 {MONDAY, THUSDAY, WEDNESDAY};

enum Enum2 {
	MONDAY, THUSDAY, WEDNESDAY
} AnonymousEnumVar3;	// well, a enum with name, at the same time a global variable is declared

enum std2::WEEKDAY {MONDAY=3, THUSDAY, WEDNESDAY};

typedef TypeDef1 typeDefined1;	// typedef test
typedef TypeDef2<std::string, std::string> typeDefined2;
typedef void(*FunctionPtrTypedef)();	// typedef -> function pointer

template<class V, typename E>	// templated function
void showVert(Vertex<V,E>& v){std::cout << "\n  " << v.id();}

///////////////////////////////////////////////////////////////
// here we mainly test the public & private & protected,
// mainly test the ;

class SimicolonDetector1 {public:private:};
class SimicolonDetector2 {public:private:;protected:};
class SimicolonDetector3 {public:;private:protected:;};

template<typename V, class E
>
class SimicolonDetector4 {public:;private:;};

class CatchExeception1 {};
class CatchExeception2 {};

// a test for anonymous class definition
class {public:;private:;} AnonymousClass1;

// well, templated class
template<typename V, class E>	// notice the class here -> don't be confused!
class std::TemplatedClass : public SimicolonDetector4<V,E>, private Namespace1::Class0<a,e*>, public SimicolonDetector2
{
private:*SimicolonDetector4& AAA;
		Namespace1::Class0* BBB;
		SimicolonDetector2 CCC;
		class NestedClass1 {	// nested class
			public:
				SimicolonDetector2 xxx;	// no one cares this field, just for detection
		};
public:
	// what about a typedef inside the class?
	typename typedef std::vector< Vertex<V,E*>>::iterator iterator;
	bool doTest(SimicolonDetector4<V,E>
	*
	// weird space? uhrrr
	& pTc)
	{
		const static std::string keys[] = { "class", "struct" };	// a static variable heres
		long int LongIntVar;	// two type(s)?
		SimicolonDetector3* sd3;
		if(sd3=new SimicolonDetector2())	// what if the variable is declared in the if?
		{
			for (SimicolonDetector1 i=0;i<2;i++)	// for loop test
				;	// do something
			for (SimicolonDetector3 x=22;;) {}	// do something
			for (auto & v:vector);	// is auto a type
			for (SimicolonDetector2 xx:ss){
				xx=ss;	// do something
			}
			for (;test<1;);
			try {
			}
			catch (CatchExeception1 e)
				;
			try {} catch (CatchException2 e2){}

			try	// sample form cplusplus.com
			{
				int * myarray= new int[1000];
				
				SimicolonDetector1 * testForArray = new SimicolonDetector1[10];
			}
			catch (bad_alloc &)	// this is a type, right?
			{
				cout << "Error allocating memory." << endl;	// test operator <<
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// more test

class std::PushType : public ArgumentNamespace1::ArgumentNamespace2::Inheritance1, ArgumentNamespace1::ArgumentNamespace2::Inheritance2 {
	Repository* p_Repos;
public:
	PushType(ArgumentNamespace1::ArgumentNamespace2::Repository* pRepos):cc(2)
	{
		p_Repos = pRepos;
	}
	inline void doAction(const ITokCollection*& pTc)
	{
		const static std::string keys[] = { "class", "struct" };	// should not capture class here?
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push type scope
		std::string name;
		for (int i=0,len;i<2;i++)
			if ((len = pTc->find(keys[i]))<pTc->call(new JustNewClass0))
				name = (*pTc)[len +1];
		element elem;
		elem.type = "type";
		elem.lineCount = p_Repos->lineCount();
		std::cout<<"\n "<<elem.show();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class HandleType : public Inheritance1<TEST,s&*>{	// no space between >{
	Aggregation* p_Repos;
	Composition p_Repos;
public:
	HandleType(Using0* pRepos) : aa(2),
		bb(22){
			p_Repos = pRepos;
	}
	~HandleType() {}	// deconstructor
	void doAction(Using*& pTc, 
		Using2<E*> & aa,*Using3 & aa, void(*FunctionPtrTypedef)(test functionPointer))
	{
		if(p_Repos->scopeStack().size() == 0)return;
		element elem = p_Repos->scopeStack().pop();
		if(elem.type == "type")
		{
			//std::cout << "\nHandleType";
			//std::cout << "\n--popping at line count = " << p_Repos->lineCount();
			std::cout << "\n  Type " << elem.name << ", lines = " << p_Repos->lineCount() - elem.lineCount + 1;
			while (WhileTest1.returnTrue()){};
			while (WhileTest2->returnTrue());
			do { }
			while (DoWhileTest1 test!=2);
			ArrayType0 type[]= { "ss" };
			ArrayType1 type[1];
			
			FancyClass clas(ss);
			
			new JustNewClassTest1(someVar);
			new JustNewClassTest2;
			((new JustNewClassTest3(
			)	// test for new line
			)
			);
			(new JustNewClassTest4(new JustNewClassTest5));
		}
	}
};

namespace Namespace2 {
	template <>
	class SpecializationClass <bool>
	{
	private:
		unsigned int *vector_data;int length;
		int size;ArrayType2 type[10];
	};

	template <  >
	class SpecializationClass2 <bool>{};

	template <  >
	class SpecializationClass2 <bool>{
		SpecializationClass2Field test;
	};
}

template <  >	// same class name, different namespace, again
class SpecializationClass2 <bool>{};

#endif