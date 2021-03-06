CLLogger:
	c++语法：
	static数据成员要显式初始化，需要写类型，如 int CLLogger::m_Fd = -1，且一般必须在类定义外初始化static数据成员，只有int例外。
	在类声明中写了static，在定义初始化时不必写static.
	在static成员函数中访问非静态成员必须要通过对象访问，访问静态成员则不用。因为static成员是属于整个类的。
	restrict关键字：C99标准引入，用于修饰指针。表明“指针是访问一个数据对象的唯一且初始的方式。
	”例如：int *restrict pInt = (int *)malloc(4); int *pNewInt = pInt;有了restrict关键字，则所分配的4字节内存只能由pInt来访问，
	第二句中讲pInt赋值给pNewInt是错误的。但编译器不会对这种错误报错（gcc）。所以，restrict的限制只能由程序员来保证。这个关键字的本意是指示
	编译器对代码进行优化的。注意：c++并不支持restrict，不过可以使用“_restrict"。
	赋值构造函数 CLASSA & operator=(const CLASSA & a)要检查自赋值的情况,如果要返回this引用，则必须返回*this，不能直接返回this。
	如果不同的情况都需要相同的处理流程的时候，可以用try-catch来控制程序执行流来控制程序执行流程，而不用goto或带参函数。
	派生类初始化基类时只要用派生类构造函数上加基类构造函数即可，如：class B(int p):A(p){}，其中A是基类构造 
	前置声明：当一个类B需要使用另一个类A的引用或指针时，使用class A；声明就可以了，而不用#include"A.h"。而如果需要B继承A或者B中包含由A的对象，则需要#include"A.h"
	
	析构函数私有化（只能在堆上分配对象）：当我们规定类只能在堆上分配内存时，就可以将析构函数声明为私有的。如果在栈上分配空间，类在离开作用域时会调用析构函数释放空间，此时无法调用私有的析构函数。
	如果在堆上分配空间，只有在delete时才会调用析构函数。可以添加一个destroy()函数来释放,从而解决不能在析构函数中添加delete的问题。
	原因是C++是一个静态绑定的语言。在编译过程中，所有的非虚函数调用都必须分析完成，即使是虚函数，也需检查可访问性。当在栈上生成对象时，对象会自动析构，也就说析构函数必须可以访问。
	（若在栈上，析构过程由编译器控制，自动的检查可访问行，若为私有，就不可访问，会出错。而在堆上分配内存，必须由程序员控制对象的释放，通过）。而堆上生成对象，由于析构时机由程序员控制，所以不一定需要析构函数也可以通过编译，。
	当然外部调用delete 自然也无法通过编译，销毁对象对象的方式是在某个方法或友元内部调用delete this。这么做的	目的通常是不希望用户控制对象生存周期。堆中的内存由程序员控制何时分配与释放，栈内存由系统自动管理。
	纯虚函数是派生类必须实现的，若不实现派生类还是一个虚类，不能实例化对象。
	栈内存和堆内存的不同：栈内存的分配和释放都是顺序的（要么分配一块，要么把这块都释放掉，参见函数的入栈、出栈），而堆内存随着分配次数的增加，可能形成碎片，最后导致速度变慢，可用内存变小（需要整理后就好了）。栈上分配的只分配，自动释放。
	map类型实际上是一颗红黑树。
	
	在构造函数中虽然不能有返回值表明构造出错（如无效参数等，初始化失败），但可以抛出throw异常给上级调用者，告诉它所发生的错误。
	
	只有A类能构造B类：将B类的构造函数和析构函数都声明为私有的，同时将A类声明为B类的友元类。
	
	模板类写法：初始化静态成员
	template<typename TSharedObjectPool, typename TSharedObject>	CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>* 
CLSharedObjectAllocator<TSharedObjectPool,TSharedObject>::m_pAllocator = 0;

	定义内联函数（模板特化）
	template<>
	inline CLMutex* CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::GetMutex()
	{
		CLMutex* mutex = new CLMutex(MUTEX_FOR_SHARED_MUTEX_ALLOCATOR, &m_MutexForSharedMutex);
		return mutex;
	}
	
	c++ style:
	if( -1 == w )，一定要不左值放在右边，防止不小心写成 if( w = -1)，避免错误
	程序结束后用CLStatus返回执行状态并进行检查是个好习惯！很有必要！避免错误。
	
	编译链接：
	不要在头文件中定义，初始化任何变量。因为头文件会被原封不动的被复制到cpp文件中，这样就可能在链接阶段出现多重定义。头文件只用来声明，
	定义都放在cpp中。头文件声明一个全局变量可以用 extern int i;然后在与该头文件对应的cpp中定义初始化变量。但是const int i = 0则可以定义
	在头文件中。因为常量方法静态数据区，而未初始化的（包括初始化为0的）全局变量放在bss段。
	
	__cdecl:__cdecl 是C Declaration的缩写（declaration，声明），表示C语言默认的函数调用方法：所有参数从右到左依次入栈，这些参数由调用者清除，
	称为手动清栈。被调用函数不会要求调用者传递多少参数，调用者传递过多或者过少的参数，甚至完全不同的参数都不会产生编译阶段的错误。