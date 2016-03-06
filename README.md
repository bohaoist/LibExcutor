本文件是开发过程的一些记录，包括部分遇到的问题和解决办法，以及自己的积累的一些经验。
Design Principle

General Pourpose:学习设计模式，学习封装Linux API，练习C++

CLStatus:
	功能：用来反应函数执行完的状态，包括返回值和出错码。
	为了使得类外部可以直接使用变量名返回出错码 m_lErrorCode, 使用了两个public的const long &引用来实现外部只可以读不可写。
	直接 return CLStatus()对象会减少构造对象的次数，提高效率，故这样写。不要CLStatus A; return A；，这样会增加构造次数。

CLLogger:
	功能：每条日志包含一条日志消息和一个出错码。
	
	Version 0.1：无缓冲
	CLLogger是一个记录程序出错的日志类，需要读写文件。由于一个程序生命周期内所有记录都写到一个文件中，所以需要数据成员fd。
	又想让它全局唯一，所以把它设计成一个单例模式（Singleton），只能创建一个对象。因为要让 static GetInstance()不通过对象访问，
	所以要把 m_pLogger写成static的。
	单例模式要全局只能创建一个对象，那么构造函数，拷贝构造函数，赋值运算符，析构函数就不能被类外部访问，只能由类本身来控制。
	构造，拷贝构造，赋值运算符都不能被外部访问，那么怎么创建一个对象呢？答案是通过用一个类的函数：static CLLogger * GetInstance();
	有了单例模式来创建对象，每次写日志的时候都需要GetInstance，然后再调用WriteLog写比较麻烦，所以向通过一个封装函数，直接写。
	
	Version 0.2:有缓冲
	功能:为了减少磁盘读写，加快速度，有两种方法：一减少write调用次数，减少陷入内核；二尽量顺序写（已满足）。在Version 0.1基础上加入缓冲。
	task list:加入缓冲
	怎样描述一个缓冲区呢？首先，要给缓冲区指派一个存储空间，用一个指针指向其起始地址；其次，缓冲区有大小size，已用空间大小used，剩余空间大小empty，
	当前写入位置（用 缓冲区size-已用空间used 即可）。
	WriteLog的时候，对要写的消息和缓冲区大小，剩余大小的判断顺序很重要！采用从大到小的顺序原则进行判断。先判断要的写消息是否大于缓冲区大小，若是则直接写入；若否，则再判断
	要写入的消息是否大于缓冲区剩余空间，若是则刷新；若否，则直接写入缓冲区。
	需要增加刷新缓冲的函数：Flush()；
	此版利用不同的缓冲区大小,进行了性能测试，发现一个合适的缓冲区大小直接影响着性能，见measuring_efficiency_main.cpp。
	
	Version 0.3:程序退出时刷缓冲问题
	问题：1.之前版本程序退出前，必须有库使用者手动flush缓冲才能避免缓冲丢失，不方便使用。希望退出时自动清理缓冲。
		 2.main返回后并不意味着程序运行结束了，全局对象的析构函数才刚开始。若这些对象的析构中调用了WriteLogMesg则这写数据扔会保留在缓冲中。.
	解决方案：
		1.用atexit函数。在单例模式创建对象的时候注册一个OnProcessExit函数进行刷缓冲。注意：因为atexit函数规定了注册函数的原型，且要符合c语言默认
		调用约定（函数参数从右到左压栈，并由调用者清栈），若是成员函数，则多出来了this指针，不符合注册函数原型要求，所以OnProcessExit应该是一个静态
		static函数成员函数。
		方案1存在的问题：一、atexit注册的函数并没有说明全局对象的析构函数同同注册函数间的调用先后顺序关系，不保证能处理析构全局对象时写的日志（实际上不能处理）。
		二、如果由atexit注册的其他函数需要写日志，也无法保证能正确的刷缓冲。
		2.Improving slution：既然可能在调用OnProcessExit后还可能写日志到缓冲，那么我们就设置一个标志flag，当调用OnProcessExit后只能用无缓冲的写。
		3.如果程序是非正常退出，如异常try，进程调用_exit函数,_EXIT等，由atexit注册的函数都不会被调用。这些情况就只能由程序库用户自己手动刷新了。
		
线程的封装：
	内容：包括线程的创建、同步、线程之间的通信等。其中通信部分参考Windows的线程消息队列机制。
线程创建的封装：
	编译问题：pthread 库不是 Linux 系统默认的库，连接时需要使用静态库 libpthread.a，所以在使用pthread_create()创建线程，以及调用 pthread_atfork()函数建立fork
	处理程序时，需要链接该库。在编译中要加 -lpthread参数：gcc thread.c -o thread -lpthread。projects->properties->c/c++ builder->toolssetting->
	libraries->add填写pthread，不要在前面加-l。
	
	系统API：#include <pthread.h>
			int pthread_create(pthread_t *restrict tidp,
		                   const pthread_attr_t *restrict attr,
		                   void *(*start_rtn)(void), 
		                   void *restrict arg);
			Returns: 0 if OK, error number on failure。成功返回0，失败返回错误码。不像其他一些API返回0表示正确，-1表示错误，而出错码要通过全局变量errno获得，然后再用strerror()函数获得具体含义。
			pthread_t是存储线程ID的类型，定义为typedef unsigned long int pthread_t。参数中指针pthread_t *表面在调用前用先创建一个tidp。
	
	等待线程死亡并收回状态资源：主线程如果退出，不管其他线程是否在运行，都一律终止，整个进程也终止。所以需要用到等待线程死亡并收回线程状态所占用的资源：pthread_join。
	调用它的线程会进入阻塞状态。
	终止时自动收回所占资源：pthread_detach。若调用了pthread_detach后再调用phtread_join，则后者会返回EINVAL错误。
	
	标*的为比较重要的封装方法。
	
	基于对象的方法封装：（缺点：无法封装变化点）
	用一个Run函数来封装pthread_create即可。
	static void *StartFunctionOfThread(void *pContext)作为所有线程的统一入口，用来包装其他线程的业务逻辑函数RunThreadFunction，用户只需要实现这个业务逻辑即可。
	它要是静态成员函数。原因和OnProcessExit一样，	应为pthread_create规定了函数原型，普通成员函数默认含有this指针。按照c语言默认调用约定_cdecl,所以应该是静态成员函数，以满足原型要求。
	因为StartFunctionOfThread是静态的，所以没有this。但又想在其中使用数据成员，故要把this当做参数传递进去。
	这种方法的缺点是要改变业务逻辑函数RunThreadFunction，就要修改CLThread类，破坏了封装性。
	
	*面向对象的方法封装：（缺点：派生的业务逻类辑的实现依赖于现有基类和基类中的虚函数定义，基类不能变，虚函数接口必须遵循基类实现）
	本方法属于基于虚函数的，动态的面向对象方式。下面基于模板的是静态的面向对象思想。
	思想：把需要变化的不同点，通过多态（虚函数）和继承来解决。不变的部分作为基类。用派生类来封装不同的业务逻辑，即每一个新业务，只需要实现一个派生类即可。
	在基于对象的方法基础上，讲原来的业务逻辑函数变成纯虚函数:protected：virtual CLStatus RunThreadFunction()=0；新创建的业务类型只需要继承CLThread并实现其业务逻辑就可以了。
	同时，析构函数也要变成虚函数virtual ~CLThread()，二是数据成员变成protected，便于派生类访问。
	存在的问题：继承关系将会使派生类耦合于基类的接口，耦合于基类的实现，即派生类依赖于基类。耦合于接口指派生类的虚函数都要遵循基类中这个虚函数的定义。耦合于基类实现指这些业务逻辑和现有
	基类固定搭配，如果换了其	他基类则不能重复利用现有业务逻辑代码了。如果现在需求变了，要求以创建进程的方式来执行这些业务逻辑，如何才能把这些单独实现的业务逻辑再次重复利用呢？
	
	*基于接口的方法封装：（优点：容纳业务逻辑和执行方式两个变化点，只耦合于基类的接口，不耦合于基类的实现）
	拆分成两部分：1.业务逻辑。2.执行方式（线程 OR 进程）
	变化点：1.需要执行的业务逻辑由多种可能。2.执行业务逻辑的方式可能变化，即创建进程来执行还是创建线程来执行，可能变化。
	解决思路：有变化点，就（适当）抽象出一个继承体系来。将两个变化点分别抽象成一个更高层次的基类，变化的部分由派生类来具体实现。然后通过传递参数的方式讲二者组合起来。
	通过用参数传递的方式代替继承的方式来解决派生业务逻辑类耦合于基类实现的问题，如explicit CLExecutive(CLExcutiveFunctionProvider *pExecutiveFunctionProvider);将业务和执行方式独立。
	使用方法：{CLExecutiveFunctionProvider* printer = new CLParaPrinter();CLExecutive *pThread = new CLThread(printer);}
	指针是使用了基类的指针，具体创建new的时候是创建了派生类的对象。通过传递参数的方法来讲二者结合起来，解决业务逻辑耦合于执行方式（线程 OR 进程）的问题。
	
	基于模板的面向对象程序设计思想：（静态的面向对象，模板的第一种范式）
	本方法属于基于模板的，静态的面向对象的方式。与前面基于虚函数的动态面向对象不同，本方法通过模板参数来传递实际运行对象。
	思路：与用虚函数不同，业务继承基类，同时将需要变化的业务自身通过模板参数传递给基类，使之实例化具体要调用的方法。
	
	面向方面的程序设计思想：（模板的第二种范式，略复杂，可以装配（改变或控制）流程，更加灵活）
	思想：执行方式（进程或线程）和业务逻辑是两个方面，通过一个模板协调器类将二者结合起来，协同工作，次协调类是两个方面的派生类。这样可以分别实现二者。一个模板参数就代表了一个方面，
	而一个方面实际上就是对一类问题，即变化点的抽象。通过协调类来控制代码在CLThread还是CMyFunction中执行，具体实现是通过强制转换来实现基类和派生类之间的跳转。
	自始至终都在一个对象中运行，但由于要执行不同的功能，故要在基类和派生类之间转来转去。
	首先在main函数中创建协调器对象，参数存在协调器对象里，然后先要创建线程，故先将this（CLCoordinator）转为创建线程的TExecutive类。创建线程时由于需要用到线程入口函数，进入
	封装的入口函数后要执行真正的业务逻辑，而只有协调器才知道怎么执行业务逻辑，故又需要将this强转回协调器类（Coordinator），	调用业务逻辑函数，并利用其中的参数来执行业务逻辑函数。
	不断的从各种基类和派生类之间转来转去反应了程序的控制流程的变化，以协调器为中心，分别跳到创建进程和创建业务逻辑的基类中，执行完再通过强制类型转换从基类中跳回到协调器类中。
	赋予模板参数的过程称为装配。当业务流程变了，不是先创建线程再紧接着执行业务逻辑，而是创建线程后先做一些其他事，再执行业务逻辑，那么可以重写一个协调器类将执行体和业务装配起来，
	只要在ProcessByProxy中先做其他事情，再调用业务逻辑即可。
	
	*基于接口的程序设计思想再封装：（新变化点：业务流程可能发生改变）
	要解决的问题：第三个变化点：业务流程的变化，即先创建执行体然后再执行业务逻辑这一流程可能发生改变。（1.执行方式变化。2.业务变化）
	思路：结合面向方面的思想，只是面向方面用模板参数来装配流程，而面向接口的思想用函数参数来装配流程。加入一个协调器来控制业务流程，其中数据成员分别是执行方式的指针和业务指针。
	同时，为了封装不同的流程，弄一个协调器的继承体系出来。同时，为了控制执行流，在执行体类和业务类中要加入指向协调器的指针。Run的时候，先用协调器通过excutive指针建立线程，建立后
	再用线程和协调器的指针，将程序控制流转移到协调器中，然后再由协调器的ReturnControlRight调用业务逻辑。
	使用方法：
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLExecutiveFunctionProvider * printer = new CLParaPrinter();
	CLExecutive *pThread = new CLThread(pCoordinator);
	pCoordinator->SetExecObjects(pThread,printer);
	pCoordinator->Run((void*)2000);
	pCoordinator->WaitForDeath();
	
线程同步的封装：
	几个问题：声明是线程同步？为什么需要线程同步？什么是死锁？如何避免死锁？
	互斥锁：pthread库提供了互斥量的机制，互斥量实际是一把锁。当一线程需要访问共享资源时，首先需要进行加锁。pthread库中要使用互斥量首先要初始化它，使用完毕还要销毁它。
	互斥锁API：pthread_mutex_inti，pthread_mutex_destroy，pthread_mutex_lock(默认会阻塞),pthread_mutex_unlock，pthread_mutex_trylock。调用成功时返回0，出错时返回错误码。
	其中，如互斥量已经被加锁，若有其他线程再调用pthread_mutex_lock加锁，则会阻塞。用pthread_mutex_trylock则不会阻塞，返回EBUSY。这几个函数成功时返回0，出错时返回出错码。
	加锁流程：
		1.初始化一个pthread_mutex_t类型的互斥量。如果是静态分配的，则可以用常量PTHREAD_MUTEX_INITIALIZER来初始化。不用pthread_mutex_init函数。
		2.调用pthread_mutex_init函数对互斥量进行初始化。
		3.访问共享资源前用pthread_mutex_lock或trylock解锁。
		4.访问共享资源
		5.访问完共享资源，用pthread_mutex_unlock解锁
		6.当不需要互斥量的时候，用pthread_mutex_destroy函数销毁互斥量。
	
	线程安全版本的CLLogger类：
	不安全的表现：1.写日志时不安全，若在Flush的时候后，已用缓存标记未置为0前有数据写入缓冲区，则置为0后这部分信息就会被丢失。
	2.创建CLLogger类对象时不安全，若在创建了一半赋值个对象指针m_pLogger前被中断，则会导致可能创建多个对象，而只有一个指针，造成内存泄露。当多个线程都在调用WriteLogMsg函数记录的日志时，
	这些线程都有可能同时向CLLogger的缓存写入日志信息。
	注意：分清楚哪些是多线程共享区！然后才能利用合适的锁。相同的共享区用同一把锁。1.创建唯一的单例对象是共享区，需要加锁。2.涉及到缓冲区的部分是共享区，对共享变量的操作都要加锁。
	对不同的区域要不同的锁，所以要两把锁。锁——其实是对执行流的一种控制，执行流的一种阻滞，一种停滞，让某一段执行流必须只能单个线程通过，不能拥挤。
	性能：为了提高性能，将if(CLLogger::m_pLogger != NULL)放在第一行，直接判断对象是否创建成功。而不用等待加锁了之后再去判断。这会不会造成不一致的情况呢？即创建对象的时候写了一半
	就被中断，而另一个在此时读。不可能。因为通常情况下，读写一个对齐了的数据，都能在一个总线周期内完成。将一个地址值写入m_pLog或从m_pLog读出地址值，是不会被中断的。目前的版本无论是直接往文件
	写日志信息还是往缓存中写日志信息，都需要进行同步，而一同步就会影响程序性能。如何尽量减少同步时间呢？一种方案是：每个线程都由自己的日志缓存；每次写日志都直接写入自己独享的缓存中。当缓存满时可以
	同步刷缓存，或者把刷缓存任务交给某个独立的线程进行。
	bug：目前的版本还存在一个bug，就是如果线程B获得日志对象指针后，在线程A完成atexit函数调用、以及m_bFlagForProcessExit标志赋值之前就执行了写日志操作，那么存入缓存中的日志信息就有
	可能丢失。
	
	CLMutex：封装互斥量
	将互斥量初始化和销毁分别放在构造函数和析构函数中。对外提供接口有Lock和Unlock，用来加锁和解锁。但是如果直接拿来用CLMutex，可能涉及到要处理异常并解锁，则会很复杂。
	CLCriticalSection将CLMutex对象包装起来，利用局部对象的生存周期，随着CLCriticalSection对象建立而加锁，销毁而解锁，这样自动实现加锁和解锁。使用的时候必须用{}将CLCriticalSection对象
	放到其中。{CLCriticalSection cs(&(p->mutex));}
	
	条件变量：	
	互斥变量只能保证互斥的访问共享区，而条件变量不仅可以实现互斥访问，而且还能保证线程之间的先后顺序。比如：先有线程A执行某个操作，执行完后，才能由线程B执行。
	条件变量与互斥量一起使用时，允许线程以无竞争的方式等待特定的条件发生。条件本身是由互斥量保护的。线程在改变条件状态之前必须首先锁住互斥量。互斥量必须在锁定以后才能计算条件。条件变量可以用两种方式
	初始化，可以把常量PTHREAD_COND_INITIALIZER赋给静态分配的条件变量。如果是动态分配的，则需要使用pthread_cond_init函数对它进行初始化。释放条件变量底层的内存空间之前，可以使用pthread_cond_destroy函数进行反初始化。
	API:pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);接收两个指针，一个是条件变量的指针，第二个是互斥量的指针。为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。
	传递给pthread_cond_wait的互斥量对条件进行保护。
	无论哪种等待方式，都必须和一个互斥锁配合，以防止多个线程同时请求pthread_cond_wait()（或pthread_cond_timedwait()，下同）的竞争条件（Race Condition）。mutex互斥锁必须是普通锁（PTHREAD_MUTEX_TIMED_NP）
	或者适应锁（PTHREAD_MUTEX_ADAPTIVE_NP），且在调用pthread_cond_wait()前必须由本线程加锁（pthread_mutex_lock()），而在更新条件等待队列以前，mutex保持锁定状态，并在线程挂起进入等待前解锁。在条件满足从而离开
	pthread_cond_wait()之前，mutex将被重新加锁，以与进入pthread_cond_wait()前的加锁动作对应。
	关于条件变量的理解：http://www.ibm.com/developerworks/cn/linux/thread/posix_thread3/#1。如果没有收到signal信号前，pthread_cond_wait会阻塞到调用点，收到此信号从调用点返回。
	在pthread_cond_wait函数内部，它获得互斥量后，在使调用线程进入阻塞状态之前，会调用pthread_mutex_unlock函数进行解锁（因为阻塞后如果不解锁，那个互斥锁会被阻塞线程长期持有，导致别的线程无法加入等待队列，所以阻塞前要释放互斥锁）；
	而在线程被唤醒之后，在pthread_cond_wait函数返回之前，又会调用pthread_mutex_lock函数进行加锁（因为需要修改等待队列）。
	因此，需要等待的线程在调用pthread_cond_wait函数时，应该按照下面顺序执行：
	1.调用pthread_mutex_lock（）函数。
	2.调用pthread_cond_wait函数（）
	3.调用pthread_mutex_unlock()函数。
	唤醒signal应该在wait之后调用，才能唤醒wait的线程。	由于因为pthread_cond_wait里的线程可能会被意外唤醒，所以在pthread_cond_wait前面一般需要加一个while(flag）的判断，以判断pthread_cond_wait是否被指定的Wakeup函数唤醒的。
	所以，对于被等待线程而言，需要按照下列步骤执行signal：
	1.调用pthread_mutex_lock函数加锁。
	2.flag=1；
	3.调用pthread_mutex_unlock函数解锁.
	4.调用pthread_cond_signal函数唤醒等待线程（从性能的角度考虑signal应该放在临界区外）。
	
	封装条件变量：
	由于上述使用条件变量时，需要考虑加锁区，标志变量-循环判断（防止意外唤醒）等，过于麻烦，所以要封装的使用起来更简单一些。（模仿Windows的内核事件对象）
	所以，用一个类将上述调用wait和signal的过程封装起来，类CLEvent。使用的时候，只需要创建一个CLEvent对象，CLEvent *pEvent = (CLEvent *) new CLEvent();
	并将此指针当做参数传递给线程。在后在线程里面用pEvent->Set()即可发送signal。
	
	线程的再封装：
	{CLThread *p = new CLThread(); p->Run(); p->Run}这样会使得第二次调用Run覆盖第一次调用Run产生的tid，导致第一次创建的线程不受控制了。我们加一个标志来解决这个问题，如果一个对象创建过一个线程就不能再创建了。
	我们的许多类对象，如CLExecutive的派生类CLThread以及CLExecutiveFunctionProvider类的派生类都是从堆中分配的。通常，我们都希望它们从堆中分配，因为这两个类及其派生类是为了创建执行体以及运行执行体具体业务逻辑的。
	在程序的许多地方，都由可能希望使用这两个继承体系的对象。若是在栈上分配，那么随着函数的退出，对象也销毁了，不利于传递对象。但是分配在堆上不可避免的需要调用delete释放对象。能不能简化这个释放的工作呢？使得用户在堆上分配的派生类对象
	只管创建，由执行体库来自动delete。（目前是基于接口的思想封装的版本）在两个地方需要自动delete，一个是Run创建线程失败需要delete this;另一个是WaitForDeath()快执行完后。（在这两个地方增加标志是因为CLThread只有这两个public的接口）。
	Run如果创建线程失败，则显然要删除这个CLThread对象。如果用户执行了WaitForDeath，当pthread_join运行结束，表面要等待的线程已经结束。则自然要delete掉这个线程对象。但这样还存在一个问题：如果用户不执行WaitForDeath()，
	则此线程对象无法自动析构，即正常不用主线程等待死亡的线程执行完无法自动析构。所以要加入一个标志在创建CLThread对象时就说明是否需要等待死亡。如果不需要等待死亡，则直接让创建的线程分离，即pthread_detach()，并在线程执行完业务逻辑
	StartFunctionOfThread后自动delete this。
	（由于目前使用的是Coordinator的版本，在协调器对象析构的时候自动析构CLExcutive和CLExcutiveFunctionProvider，所以上面针对自动析构的部分可能出错。上面是基于面向对象的版本。）
	
	
	线程消息通信的封装
	思考步骤：
	1.把各种各样不同类的消息用一个继承体系封装起来，充当消息的数据结构类（消息的封装）
	2.把封装好的消息放到一个队列类里面，并且由队列类来处理生产者发送消息的同步问题（有消息产生时的通知机制）和从队列摘取或加入消息时的互斥问题（所以队列类中需要用到互斥量和条件变量）（消息队列的建立）
	3.在使用的时候，创建一个消息队列，并作为参数传递给业务线程（消费者），业务线程不断的从消息队列中取出消息，根据所取出消息的类型来决定如何处理。生产者将生产出的消息不断插入到消息队列中。（为了复用消息循环框架，封装了消息循环机制）
	windows系统会为每个线程配备一个线程队列。Windows系统的线程通信机制从本质上来说，就是一个生产者/消费者模型。
	因为有各种不同类型的消息，根据消息类型的不同，采用不同的处理方式。这是一个结构化的思想。我们要建立面向对象的消息机制。由于有不同类型的消息，我们用一个继承体系来封装这些变化点。首先我们建立一个消息的基类CLMessage，提取出消息共有的部分。
	如果用户要发送自定义消息时，就从基类CLMessage中派生一个类出来，派生类中包含消息的内容和自定义的ID（可用作常量）。用一个类作为一个结构来封装每一条消息的内容。
	建立消息队列：线程的消息队列一般是单读多写的，即只有消息队列的拥有者可以读，而可以同时由多个线程想消息队列写入消息。显然需要互斥量进行保护。而且，当有消息到达时，还需要通知等待线程。（消息队列没有消息时，处理线程处于阻塞状态）
	消息队列的通知机制：但是这里的互斥量不能用前面的CLEvent，因为前面的CLEvent中m_Flag（只能是0或1）只表明了是否有信号发送，而不能表明有多少条消息发送过来，这样会丢失消息。所以要修改CLEvent，增加一个标志，表明是否需要用m_Flag记录消息的条数。原来的CLEvent
	仅仅用来同步，现在的CLEvent和消息队列结合，可以用来记录消息的个数。
	为了简化起见，使用STL库中的容器queue来构建消息队列。
	
	消息循环机制的封装
	while(true)
	{
		pMsg = GetMessage(); //阻塞的等待消息到来
		DispatchMessage(pMsg);//分发到来的消息给处于线程
	}
	如上面的这段代码，称为消息循环。虽然不同的业务处理的消息不同，但消息循环框架本身不变。我们希望消息循环框架本身能够被复用。最简单的一个办法是将消息循环放到基类中去。
	基类CLMessageLoopManager并没有规定如何从消息队列中获取消息，以及如何处理消息，甚至没有规定消息队列是上面样子的。这意味着我们不一定使用STL queue来实现消息队列，还可以用管道、网络通信等实现线程间通信。
	对于不同的消息队列实现，我们用不同的消息循环派生类来管理消息。具体要执行的处理，需要由使用者来实现。如果库使用者想要接收线程消息，就得再从CLMsgLoopManagerForSTLQueue派生一个新类出来，并重写DispatchMessage函数。
	为了使得分配在堆上的对象实现不用程序员手动释放，可以形成析构链，消息对象没能形成析构链，可以在处理完消息的时候delete。对于退出消息循环还可能存在于消息队列中的消息来说，在消息队列的析构中删除就好了。
	
	消息处理机制封装
	前面的代码结构存在严重的问题，消息处理类CLMyMsgProcessor从CLMsgLoopManagerForSTLqueue继承而来。继承关系是一种强耦合关系，意味着CLMyMsgProcessor只能接收从CLMsgLoopManagerForSTLqueue传来的消息。如果要让它接收来自管道或
	网络的消息该怎么办呢？这时就得修改CLMyMsgProcessor了，破坏了代码封装。（经验：实际开发中如果遇到继承体系层数超过两层，说明代码结构十有八九有问题）
	原来的不能容纳上述变化点。CLMessageLoopManager实际有两类变化点：1：消息来自何处。2.如何处理消息。那么对于两类变化点，就应该建立两类继承体系，分别处理这两类变化点。（变化点就是不同的地方，相同的地方就是基类）
	用CLMessageObserver基类来封装对所有消息的处理。所有的消息处理函数都必须派生自CLMessageObserver。同时为了消除switch/case语句，用一个map来保存消息ID和对应的消息处理对象，初始时需要分别注册消息ID和处理对象，真正处理的时候需要在map中查找ID和相应处理对象。
	由于前面把对所有消息的处理都派生于一个基类CLMessageObserver，如果消息类型太多，会导致产生太多的类。想办法将所以类型的消息的处理放到一个类的成员函数中，然后在map中保存这个类的成员函数地址，这样可以减少类，但是新增加类的时候就必须修改原来的类，破坏了封装性，但对
	不同消息的处理作为成员函数有利于程序员维护和掌握，所以实际中还是倾向于变为成员函数的做法。（类太多不利于维护和掌握）
	
	线程创建于消息循环的结合
	当我们创建一个新线程，可能希望它马上执行业务逻辑，也可能让新线程进入消息循环，等待各种消息并以这种消息驱动的方式执行各类业务逻辑。将创建一个CLExecutiveFunctionProvider的派生类CLExecutiveFunctionForMsgLoop来让创建线程后进入消息循环。
	
	名字服务
	本质是给每个消息队列起了一个名字，并且将线程业务处理函数和特定名字的消息队列绑定。同时，由于线程的结束作为导火索，分别根据析构链来析构掉所有东西，包括消息队列及其中的消息。
	主线程向新线程发送消息，需要保留消息队列的指针。所指向的消息队列对象是在什么时候释放的呢？因为在CLThread对象所指向的指针释放的时候，导致CLMsgLoopManagerForSTLqueue类的析构也会删除消息队列指针。如果由多个线程向加法器发送加法消息，可能存在这样一种情况，
	即消息队列对象已经被删除了，但仍有线程使用消息队列指针pQ指针指向假发线程发送消息。如何才能让发送消息的线程指定消息队列已经删除了呢？
	另一方面，线程想要向加法线程发送消息，都要获得指向消息队列的指针，这样一类需要在众多函数中传递这个指针显然十分不方便。windows中，一个线程要向另一个线程发送消息，只需要指定对方的线程ID即可，而并不需要指定系统提高的内部消息队列的指针。
	在执行体库中希望实现一种名字服务功能，即建立线程名称到线程通信对象的关联关系，并采用引用技术的方式管理线程通信对象。使用者只要给出线程的名称，名字服务将返回与该线程相对应的通信对象；使用这个通信对象，就可以向该对应线程发送消息，而且不必担心通信对象无效的情况，也不用用户自己创建（管理释放）消息队列了。
	PostExecutiveMessage函数是异步语义，它投递消息后就立即返回，并不会等待消息被处理完成。
	有了名字服务，在创建线程进入消息循环的时候就要注册线程所对应的消息队列的名字，所以消息循环的实现CLMsgLoopManagerForSTLqueue时要在构造参数中加入名字。
	
	关于对象的析构：
	执行体库的对象大多都是在堆中分配，然后由执行体库来自动释放。自动释放路径分为几个部分：
	1.首先是CLThread类对象的释放引起的一系列析构链来释放作为其参数链的各种对象。CLThread通过自我删除来释放（delete this），分为两种情况：一是调用了WaitForDeath函数，该函数等待线程死亡后就执行delete this。二是如果不需要WaitForDeath，就会在线程执行完
	时执行delete this。CLThread对象的析构会导致其基类CLExecutive的析构函数被调用，进而删除CLExecutiveFunctionProvider为基类的继承体系对象，这有导致CLExecutiveFUnctionForMsgLoop类的析构被调用，进而又析构CLMessageLoopManager为基类的继承体系，
	接着CLMsgLoopManagerForSTLqueue析构被调用，但它什么也没做（消息队列由通信对象释放），其基类会析构CLMessageObserver继承体系的派生类对象，即删除CLMyMsgProcessor。
	这里关键是两点：一是c++语言提供的析构函数机制，特别是派生类析构函数被调用后，会调用基类的析构函数。二是在堆上创建的一系列对象，都用参数通过成员变量指针关联了起来，形成了析构链。
	2.除了上述析构链，还有几类对象不需要执行delete。（1）一是消息队列对象和通信对象（用来管理消息队列及其名字）。即CLMessageQueueBySTLqueue类对象和CLThreadCommunicationBySTLqueue，由名字服务CLExecutiveNameServer类负责删除，并采用
	引用计数。当引用计数为0时，自动删除通信对象，在删除CLThreadCommunicationBySTLqueue类对象时，其析构函数又会删除CLMessageQueueBySTLqueue，即删除消息队列，这样消息队列的删除由全局来控制，避免主线程删除了消息队列后别的线程还不知道。
	（2）二是各种消息对象，即CLMessage为基类的继承体系对象。这些消息对象在被处理过后，程序库都会为其调用delete。如果线程退出消息循环后，消息队列中还有消息没被处理，那么在消息队列对象被删除时，会自动释放这些遗留消息对象。这部分在CLMessageLoopManager中处理。
	(3)三是CLLogger类的对象和CLExecutiveNameServer类的对象。它们两个都是Singleton的，即全局唯一的对象。它们的生命周期应该和整个程序的生命周期一致。因此，不必为它们调用delete，让它们随程序结束析构就好了。
	
	由于前面封装的名字服务使用前需要调用GetInstance获取名字服务对象并检查返回值，然后要GetCommunicationPtr获取指定名字的消息队列并检查返回值，最后用完还要调用ReleaseCommunicationPtr归还通信对象指针，让通信对象引用计数减一。
	为了简化操作，把这些步骤都封装在一个类的静态函数中，使用时直接调用静态函数即可。这样的缺点是效率方面的问题。没法是异常消息就得重写获取、归还通信对象指针一次，而这将导致对map容器的查找。通常map容器中的节点书不会很多，因为每个节点对应了当前的一个线程，线程数是有限的。
	一般情况下使用静态函数PostExecutiveMessage所造成的效率损失还是可以容忍的。
	
	等待新线程将消息注册完成
	主线程创建新线程后，必须要确保新线程初始化完成后才能向消息队列发送消息，否则可能会导致新线程还没初始化好，消息队列没创建完成，主线程就往消息队列中发送消息，从而导致失败。一个想法是用CLEvent事件对象，让CLThread类的Run方法来保证（Run函数运行在主线程中）。由Run函数可以实现，但是这样的话，就要所有的线程都进入到消息循环中了。对于不进行消息循环，不借收消息的线程而已扩展（想一想其实可以，只要加一个是否使用消息循环的标志就好了）。
	除此之外，用CLEvent除了通知同步外，不能传递其他消息，如线程是否创建成功。所以，我们建立一个新的继承体系来解决这个问题。用CLExecutiveInitialFinishedNotifier及其派生类来解决通知机制的问题。
	为了区分是否要进入消息循环，我们要再建立一个类包装一下Thread和CLExecutiveInitialFinishedNotifier，即类CLThreadForMsgLoop。
	注意，在CLThreadForMsgLoop中，析构函数里才调用了WaitForDeath()函数，所以堆上创建的CLThreadForMsgLoop对象必须析构才行（也是避免内存泄露），才能调用WaitForDeath。
	CLMessageLoopManager的析构～CLMessageLoopManager（）中又释放了一次delete m_pMessageOberver;而在此类的EnterMessageLoop中当收到QUIT_MESSAGE_LOOP后，也会有一次delete m_pMessageOberver。原来的代码delete后没有将m_pMessageOberver=0，这将导致在析构函数中二次释放造成segmentation fault。所以修复方案是删除析构中的delete，直接由EnterMessageLoop中的地方来释放；或者是将EnterMessageLoop中的delete后m_pMessageOberver赋值为0


进程的封装：
	进程创建fork函数：fork函数一次调用两次返回，分别返回给父进程（返回子进程pid）和子进程（返回0）。刚创建子进程时，子进程只是父进程的副本（二者完全一样），二者内容一样。这样一来，就需要告诉子进程从哪里开始运行，答案是子进程从fork返回处开始运行。如何在编程中区分哪个是父进程哪个是子进程呢？根据它们的返回值的不同来区分。子进程返回0，父进程返回子进程pid来判断。关键点：子进程复制了父进程的地址空间，（在exec前）二者一样！
	
	父子进程间的文件共享：
	首先要搞清楚单个进程打开某个文件涉及的过程。打开文件，主要涉及5个数据结构：task_struct（进程控制块PCB）,files_struct（已打开文件表，内容是一个进程的文件描述符表，包含此进程的所有打开的文件描述符，此数据结构每个进程独享）,file(文件对象，由文件描述符指向这里，是每个文件描述符的具体内容，描述一个文件的打开状态，包括文件的非打开标志，读写模式，当前偏移量等，每个文件对象系统全局共享),dentry（目录结构体）以及inode（包括文件的属性）。
	注意：其中只有task_struct（进程控制块PCB）和files_struct（已打开文件表，文件描述符表）是每个进程独享的。后面的三个数据结构file（文件对象），dentry（目录结构体）以及inode（包括文件的属性）都是系统内全局共享的。所以，如果创建子进程前，父进程打开了一个文件，那么创建子进程后父子进程共享了同一个file文件对象（包含一些标志，文件偏移量等），如果父进程修改了文件偏移量，对于子进程而言是完全可见的，二者共享。
	
	等待新进程死亡：pid_t waitpid(pid_t pid, int *status, int options);其中status是所等待进程的终止状态，终止状态就是exit(参数)函数的参数就是终止状态，如果进程从main函数退出，main函数的返回值也是终止状态。通常情况不关心进程的终止状态，可以将Status设置为NULL。如果所等待的进程还没有死亡，则导致调用waitpid的进程陷入阻塞状态，可以通过options参数设置waitpid为非阻塞方式。
	exec函数：exec函数是一类函数，有execl,execv,execle,execve,execlp,execvp等等。
	进程封装的问题：
	1.子进程释放父进程对象的问题。2.文件描述符继承问题。3.越权访问的问题（文件描述符继承问题导致）
	问题1 由fork+exec来解决，exec替换后所有的内存地址，地址空间的所有内容都是新的
	问题2 文件描述符继承的消除：子进程使用fork创建都，都继承了父进程的文件描述符。这样子进程和父进程可能操作同一个文件对象，造成冲突。解决方法是子进程创建后立马把它继承的文件描述符关闭。在Linux中，/proc目录记录了各个进程运行时的信息。在/proc中，每个进程都有一个目录，该目录以进程ID命名，在每个进程的所属目录中，又有一个名为“fd”的目录。进程打开的文件描述符，在该目录中都对应了一个符号链接文件。而该文件的名称，即文件描述符的数值。我们只要遍历”/proc/进程ID/fd”目录，就能获得当前进程打开的所以文件描述符，然后为其调用close函数即可。
	使用opendir()打开目录，然后使用readdir()读取目录中的内容，readdir每次返回一个文件或子目录相关的结构体实例指针，即dirent结构体，该结构体的d_name字段指向文件名或子目录名的字符串指针。closedir函数关闭目录。
	用execv创建新进程时，需要拼接char *argv[]参数表，这个指针数组的最后一个值是0，表示参数结束。
	bug: 这个过程中由个bug，closedir调用失败了！不明原因。CLLogger::WriteLogMesg（）丢消息，并没有把错误信息写入日志，但我试了不带缓冲的WriteLogDirectly（）则成功的写入了消息。
	分析: closedir调用失败导致CLLogger创建了一个日志文件写消息，这时是在子进程中，这就在子进程中新打开了一个文件，文件描述符为3.
	原因1：closedir调用失败的原因是CLProcess::CloseFileDescriptor()中关闭了/proc/进程pid/fd中的所有打开文件（虽然是用close关闭的），这就包括了前面所打开的目录文件，所以再次closedir时就会发生错误（errno 9 :Bad file descriptor）。
	解决方案1：/proc/进程pid/fd中的所有文件都是符号链接，指向了真正打开的文件。比如我们打开某个进程的/proc/进程pid/fd目录，那就会有 3-> /proc/进程pid/fd，这样的符号链接。3表示文件名，也表示对应进程的文件描述符3.所以，我们只要遍历目录文件，只要符号链接和/proc/进程pid/fd不同，就不用close。
	原因2：closedir调用失败后用CLLogger::WriteLogMesg（）写错误信息没有写入日志文件的原因：WriteLogMesg（）的调用是在子进程中使用open打开文件并获得文件fd的，此fd只在子进程中有效，在父进程中无效。而WriteLogMesg先将消息写到缓存中，当父进程（注意是只要在父进程退出时才会调用OnProcessExit）快退出是，CLLogger注册的OnProcessExit函数被调用时才会刷缓存，而刷缓存使用的是m_Fd是子进程出错写日志时获得的，对父进程无效，因此父进程Flush调用write函数会出错，无法写入日志。换句话说，之所以父进程write函数会出错，就在于m_Fd保存的文件描述符不是由它打开的。（概况：原本的m_Fd被关闭了，而子进程CLLogger中记录的还是原来的fd，这样刷缓存的时候就会出错。）
	解决方案：一个解决方案是由用户使用者在使用库之前就调用一下CLLogger类的GetInstance方法创建日志对象。单这一稍微麻烦一点。还有之前的一些全局唯一的对象，我们采用了至分配，不释放的方案，等到程序结束让操作系统自动回收。我们现在的方案由于要手动调用GetInstance创建日志对象，那么干脆把这些全局对象也一并放在一个函数中，让用户使用执行体库前调用初始化，退出时调用反初始化。我们用一个初（反）始化类CLLibExecutiveInitializer来实现这个目标。
	要重构CLLogger类，去掉一些由于加入手动初始化和反初始化的成员，将CLLogger写成不带缓冲的版本，不丢日志。
	
	*僵尸进程和孤儿进程：[问题及危害]http://www.cnblogs.com/Anker/p/3271773.html
	孤儿进程：一个父进程退出，而它的一个或多个子进程还在运行，那么这些子进程就称为孤儿进程。孤儿进程将有init进程（pid为1）所收养，并在子进程运行结束后由init完成它们的状态收集工作。
	僵尸进程：一个进程使用fork创建子进程，如果子进程退出，而父进程并没有调用wait或waitpid来回收子进程的状态，那么子进程的进程描述符仍然保留在系统中，通过ps命令查看会显示<defunct>状态，称为僵尸进程。如果父进程一直没有回收子进程的状态，当父进程也结束掉后，那么子进程就由僵尸进程编程孤儿进程，由init进程结构，而init会为子进程调用wait函数，获取终止状态。如果父进程创建了子进程，子进程创建了孙进程，且子进程最先结束，那么孙进程就会变成孤儿进程。
	危害：孤儿进程的资源由init进程回收，因此并不会有什么危害。对应僵尸进程，因为系统所能使用的进程号是有限的，如果系统内大量产生僵尸进程，将会因为没有没有可用进程号而导致系统不能产生新进程，此即僵尸进程的危害。这种情况下，严格的说僵尸进程并不是问题的根源，问题的关键是产生大量僵尸进程的父进程。通过kill发送SIGTERM或者SIGKILL信号给父进程，那么这些僵尸进程就变成孤儿进程，由init进程回收。
	
	程序内直接调用系统调用用函数syscall函数，一个例子是可用用syscall(SYS_getpid)。用此方法可用用来克服getpid()的bug，即子进程创建孙进程的时候获得的是父进程的pid（原因是getpid实现的使用了缓存）。

进程同步方式封装：
	进程间同步有多种方法，比如记录锁，进程间共享的互斥量，条件变量等。
	
	记录锁：
	记录锁加锁文件的某个区域。当一个进程需要写某个文件时，为了防止其他进程读或写该文件，可用使用记录锁，对该文件要写的区域加锁。这样依赖，就达到了对加锁区域互斥访问的目的。记录锁的加锁或解锁由fcntl函数控制。记录锁有读写锁，如果加了读锁，则其他进程还可以加读锁；如果加了写锁，则其他进程再加锁会被阻塞。
	记录锁特点：只能用在不同进程间的互斥，不能用在同一进程内线程间的互斥。当一个进程对某一文件区间已经有了一把锁，这之后再对同一区间加锁，心锁将会替换掉老锁。所以只能记录锁只能用在不同进程间互斥，不能用在同一进程内不同线程间互斥（即记录锁不能用于进程内互斥）。
	记录锁有几点需要注意：
	1.记录锁可以用非阻塞的方式加锁，如果有别的进程占用而不能立即获得锁，fcntl函数返回出错值EACCESS或EAGAIN。
	2.使用记录锁加读锁，需要以只读或读写方式打开文件；加写锁，需要以只写或读写方式打开。
	3.当进程终止，这个进程建立的所以锁都会被释放掉。
	4.子进程并没有继承父进程的记录锁。
	
	可以用记录锁作为两个进程同步的通信工具：两个进程共同打开一个记录锁文件lock.txt，文件全范围加锁，当做互斥文件。对于两个进程正真要写入的文件a.txt，两个进程都打开，但并不对a.txt加记录锁，两个进程通过判断lock.txt文件是否加锁来判断是否有别的进程使用a.txt。这样的好处是lock.txt文件可以由程序库自动创建，对用户不可见，同时加锁粒度是整个文件，降低了程序库复杂度。但这种方法由于需要不停打开和关闭文件，所以效率比较差，不推荐使用，推荐使用共享互斥量。
	
	注意：在同一个进程中对同一个文件open两次fd1和fd2，然后对fd1加记录锁，再关闭close(fd2)，这会导致fd1上加的记录锁失效。
	——————————————————————————————————————————
	int fd1 = open("a.txt",...);
	int fd2 = open("a.txt",...);
	fcntl(fd1,add_lock);
	close(fd2);      //会导致上面对fd1加的记录锁失效，这将导致不能互斥
	fcntl(fd1,un_lock);
	close(fd1);
	__________________________________________
	这个问题导致了一个很隐晦的bug！见对新的CLMutex类的测试。两个进程，每个进程2个线程，每个线程对共同一个文件的头8byte读写，每次读入文件中的值，加一后再写入文件，最后得到3988259（每次不一样，有时候是正确值4000000），当小于1000000次循环时候不会出现这个bug，还时隐时现。
	原因分析：CLMutexText.cpp中ReadAndWriteFile中，每次for循环开始时获得一个mutex（有记录锁和pthread_mutex_t相结合的功能），进入临界区，每次循环结束后退出临界区，释放一次锁。这里就有一个问题了，退出临界区时，会调用CLMutexByRecordLocking的析构函数，此析构函数中会调用close函数关闭记录锁文件，同时，重要的是（！！！！）,CLMutexByRecordLocking的析构函数是在加锁区外被调用的（因为CLCriticalSection的析构函数先调用，然后才调用CLMutexByRecordLocking的析构），这就可能导致一种情况：假设父进程内，主线程获得了锁正真读写文件，而新线程正处于析构mutex对象，导致调用了close函数关闭文件，从而使主线程获得的记录锁被释放。子进程也存在同样的问题。这样就会导致子进程和父进程内各有一个线程在读写文件，从而无法做到进程间互斥，导致错误。
	解决方案：CLMutexByRecordLockingAndPThread的加锁过程是先加pthread_mutex_t类型的互斥量锁，再加记录锁。解锁顺序相反。如果把文件的打开操作放在两个锁加锁之间，文件的关闭操作放在两个锁解锁之间，问题就解决了。这是因为获得pthread_mutex_t互斥量锁，就意味着同一进程内没有其他线程获得锁。又由于关闭文件时，处在记录锁已解锁，互斥量锁未解锁期间，因此，这时除了刚才获得记录锁的线程外，再不可能有其他线程调用close函数，从而导致记录锁被意外释放。
	
	int fcntl(int fd, int cmd, ...)，fcntl可用执行很多命令，由第二个参数cmd控制，而cmd同时决定着第三个参数和返回值的含义。调用失败返回-1.
	封装记录锁的目的是后面封装共享互斥量，共享条件变量和共享事件对象的时候会用来进行进程间互斥创建共享互斥量池。记录锁作为最为原始的进程间通讯工具。
	
	工作目录：当我们使用fork或vfork创建子进程后，子进程就会继续父进程的工作目录，父进程的工作目录一般是运行目录。使用相对路径的时候切记要确认当前目录是什么，以免出错。
	
	进程安全的CLLogger类
	我们之前的CLLogger类用互斥量保证了线程安全。多进程同时写日志的时候，就不行了，我们使用记录锁来达到CLLogger线程安全和进程安全两个目标。在加记录锁的过程中，必须要保证加锁的范围和解锁的范围一致，否则会导致记录锁的组合和裂开。
	
	互斥量封装的重构
	我们有多种类型的互斥量，记录锁和pthread_mutex_t及记录锁和pthread_mutex_t类型的互斥量结合起来，实现任意进程间的互斥。这就形成了一个变化点，我们用一个继承体系将这个变化点封装起来，所以我们要建立一个基类。根据这几类互斥量的共同点，它们都有初始化，反初始化，加锁和解锁几个基本功能，所以这应作为基类的成员函数。
	我们希望重构CLMutex，使得它能以同样的方式使用这三种互斥量（pthread_mutex_t，记录锁以及记录锁和pthread_mutex_t类型的互斥量的结合CLMutexByRecordLocking），由CLMutex来统一使用这三种互斥量的基类CLMutexInterface来使用不同类型的互斥量，具体要使用哪种互斥量，则由CLMutex的调用者通过不同的构造参数来指定（多个构造函数）。为了保持对之前pthread_mutex_t的兼容性，保留之前的接口，并加入新的构造函数。由于后两种类型的互斥量都需要文件名来初始化，所以需要通过指明种类nType来表明要创建的互斥量的类型。
	
	共享存储及其封装
	通过将pthread_mutex_t类型的互斥量放在进程间的共享区内，达到互斥的目的。
	使用共享存储的若干相关进程，把自身地址空间中的某部分区域，在页表等机制的作用下，都映射到同一个物理存储区。32位处理器，段寄存器实际上就是段描述符索引。在全局描述符表寄存器GDTP或者局部扫描述符表寄存器LDTR的作用下，可以根据段描述符索引确定段基址。有了段基址，再加上偏移地址，就形成了线性地址。
	用shmget函数创建或打开一个共享存储区域，之后在调用shmat函数将共享存储区域映射到调用进程的地址空间，映射后就可以开始使用了。当进程步骤需要在对共享存储区域进行操作，可以调用shmdt函数接触于共享存储区域的关系。当所有进程均不使用共享存储时，可以调用shmctl函数删除共享存储区域。
	每一个共享区都有一个名字（和ftok生成共享区ID有关，ftok是基于文件名对应文件的Inod节点信息的，所以即使是相同的文件名，删除了之后再建立，得到的共享区ID也不同）。在创建共享存储区的时候，同过记录锁来互斥（也利用相同的文件名）。同时在用ftok生成共享区的ID的时候，也是利用这个文件的inod结点信息。
	
	共享互斥量封装
	由于现在要将pthread_mutex_t用于进程间线程的同步，因此，需要将互斥量属性设为进程共享属性。
	方法是：首先调用pthread_mutexattr_init函数对互斥量进行初始化，然后再调用pthread_mutexattr_setpshared函数将互斥量设为进程共享属性。这之后，才能调用pthread_mutex_init函数完成互斥量的初始化操作。当互斥量属性使用完毕后，需要调用pthread_mutexattr_destory函数销毁它。在线程互斥时，调用phtread_mutex_init时将第二个参数设置为了0，进程共享属性为PTHREAD_PROCESS_PRIVATE,即表示互斥量仅在进程内使用。
	
	共享互斥量的封装（用共享互斥量池来实现，池是单例的）
	思考方法：首先，封装共享互斥量想要达到什么样的效果？---最好的效果就是，过去的互斥量怎么使用，现在的这个共享互斥量还是怎么使用。共享互斥量的使用，不同点主要体现在分配和初始化的过程上。而在分配方面，需要从共享存储区域分配互斥量。
	如何分配共享对象的存储空间呢？
	如果一个共享互斥量一个共享存储对象，如果共享存储对象太多的话，就会消耗太多的系统资源，这样代价太大。
	所以，一个常见的做法是：构造一个共享互斥量池，这个池实际上也就是共享存储了。对于整个系统而言，执行体库的共享互斥量池应该是唯一的，且所有的相关进程都能访问到。在每个进程内，应该有且只有一个对象来管理这个池，显然这个共享互斥量池是单例的。在执行体库的初始化阶段，CLLibExecutiveInitializer类的Initialize函数中，完成对共享互斥量池及其管理对象的初始化操作。
	因为有共享互斥量，共享条件变量和共享事件对象等多种共享变量以及其他共享元素，所以需要创建一个共享对象池的基类，让不同类别的共享对象分别继承共享对象池基类，从而实现不同类别的共享对象池。最终，不同类型的对象有不同类型的共享池：共享互斥量池，共享条件变量池，共享事件对象池（其中，共享事件对象池使用了共享条件变量池中的对象（通过给共享条件变量构造一个名字构造共享条件变量对象）和共享互斥量池中的对象（给构造函数一个名字，构造成共享互斥量）。
	
	共享对象池（将共享对象集中管理，避免分配太多分散的共享对象）：设计一个共享对象池，首先，需要先设计好这个共享对象池的内存布局，然后根据内存布局具体的实现。我们将共享对象池底地址的头4字节设为Magic Number，它的作用主要是判断一个共享对象池是否已经被初始化了。从第5字节开始，是共享对象池的内容部分，即一个一个连续的共享对象。其中每一个共享对象由一个共享对象头部和具体的共享对象内容组成。加入共享对象头部的主要作用是用来管理每个共享对象，因为每个对象都是共享的，所以需要给每个对象指明一个当前的状态（未初始化，已初始化，已分配），每个共享对象的引用计数（有几个进程在共享这个对象），以及每个共享对象都有一个名字（用名字来访问和获取共享对象）。
	有了共享对象池的基类，具体类型的（共享互斥量池--！注意是共享互斥量池，基类是共享对象池）就由不同的子类来实现。
	在整个系统中，应该只会有一个共享互斥量池。而在一个进程中，应该只会有一个对象来管理这个共享互斥量池。在执行体程序库被使用之前，需要创建这个管理共享互斥量池的唯一对象，并且完成池的初始化操作。用CLLibExecutiveInitializer类来完成初始化和销毁操作。现在需要将CLSharedMutexImpl类的对象创建工作和CLLibExecutiveInitializer关联起来。其次，就是为创建互斥量对象池加上同步操作。我们用一个新类CLSharedMutexAllocator类完成。（我们把CLSharedMutexImpl类的构造函数和析构函数都声明为private，并将CLSharedMutexAllocator声明为其友元，这样就只能通过CLSharedMutexAllocator类来创建和析构共享对象池CLSharedMutexImpl了）
	CLSharedMutexAllocator类主要用来初始化共享互斥量池和对共享互斥量池进行互斥操作(通过记录锁和pthread_mutex_t的结合的互斥量来达到进程间互斥和线程间互斥的目标。)	封装共享互斥量想要达到的效果，对使用者而言，还是通过CLMutex类的接口来使用共享互斥量。所以我们要从CLMutexInterface类派生一个新类CLMutexBySharedPThread，实现从共享互斥量池分配，归还以及操作互斥量对象。
	CLMutexBySharedPThread与其他CLMUteInterface不同之处在于每一个CLMutexBySharedPThread对象都有一个名称，要通过名称来在共享互斥量池中访问和释放共享互斥量对象。
	CLMutexBySharedPThread类的Lock，Unlock函数的实现，和CLMutexByPthread类一样，就是调用pthread_mutex_lock, pthread_mutex_unlock函数。既然这两个函数实现方法类似，那么最好能复用重复的代码。回顾CLMutexByRecordLockingAndPThread类并不是直接使用pthread_mutex_t或者记录锁，而是通过使用CLMutex隐藏了CLMutexInterface继承体系。按照这个思路，范式要使用某一种互斥量，那么就得直接使用CLMutex类，而不是隐藏在它后面的其他类。也就是说，CLMutexBySharedPThread想要复用CLMutexByPThread类的代码，就得构造一个CLMutex类型的成员变量。利用CLMutex::CLMutex(pthread_mutex_t *pMutex)构造函数，将共享互斥量池中返回的pthread_mutex_t *pMutex指针传给它，就构造了CLMutexByPThread对象，因为它和共享互斥量的操作是一样的，所以就可以复用它的代码了。

	*(已实现)因为CLSharedMutexAllocator（可变成CLSharedObjectAllocator）可以用来创建各种类型的共享（对象）池，所以可以考虑用模板来实现。
	在指定内存地址分配对象的方法：以互斥量为例，用pthread_cond_init(&(pCondItem->cond),&attr)函数在指定的共享内存区域分配空间，定义struct SLSharedMutexItem结构主要是为了使用SLSharedMutexItem*的指针，构建这样一个内存结构，方便在初始化对象的时候指定对象内存的起始地址。对于自定义的对象，如共享事件对象CLEvent，初始化则定义一个结构体，用一个结构体指针指向要分配对象的地址，从而构造出那一块内存的结构，直接通过指针给相应的位置上赋值即可。如
	CLStatus CLSharedEventImpl::InitializeSharedObject(SLSharedObjectHead *pObject)
	{
		SLSharedEventItem *pEventItem = (SLSharedEventItem*)pObject;  //用结构体指针构造出内存空间的结构，然后通过指针给相应位置上直接赋值即可。在指定内存上分配一个对象本质上就是在那块内存对应的位置写上相应的值。
	
		pEventItem->EnentInfo.Flag = 0;
		pEventItem->EnentInfo.bSemaphore = 0;
		pEventItem->EnentInfo.Context = 0;
	
		return CLStatus(0,0);
	}
		
	bug1:CLMutexByRecordLocking::Lock()中因为会在/tmp目录下建立一个文件，所以默认情况没有权限，会导致出错。
	*bug2:会死锁
	
	共享条件变量及其封装
	和共享互斥量类似，封装条件变量也由两点：1.是在进程共享区域中分配。2.是需要为条件变量属性设置进程共享属性。除了需要给原来线程的条件变量设置属性，其他都一样。所以，我们希望通过改造CLConditionVariable类来重用原来的代码一个地方是增加一个接收pthread_cond_t类型指针的构造函数，来接收设置过共享属性的条件变量。另一个地方是wait函数，原来的wait是处理pthread_mutex_t类型互斥量而实现的（用GetMutexPointer方法获得指针），而没有处理跨进程封装类CLMutexBySharedPThread。
	CLConditionVariable类中，因为要对独享条件变量和共享条件变量分别使用不同的互斥量（CLMutexByPThread和CLMutexBySharedPThread），而接收的参数类型却是CLMutex*,所以要根据传入参数的实际值调用不同的GetMutexPointer方法，一个很容易想到的方式是利用基类纯虚函数，让不同的派生类分别实现。但因为基类CLMutexInterface中没有GetMutexPointer为虚函数，所以不能用多态，只能分别对两个候选类强制转换dynamic_cast<>，只有一个能转换成功，得到不为NULL即要求的值。因为CLMutexInterface的其他两个类没有GetMutexPointer方法，所以不放在基类中当纯虚函数。（还有一个方法是完全面向对象的方法，将CLMutexByPThread和CLMutexBySharedPThread类抽象成一个新的父类CLMutexUsingPThread，这个类派生于祖父类CLMutexInterface，在这个新的父类里面提供纯虚函数GetMutexPointer.）
	
	如同共享互斥量，在共享条件变量池的初始化中实现设置条件变量的共享属性等操作。

进程通信的封装
	进程通信等待方式有很多，比如管道、命名管道、消息队列、存储映射I/O以及共享存储。
	
	命名管道的使用
	命名管道和管道类似，唯一的差别在于没有名字。普通管道没有名字，它只能用于具有公共祖先的进程之间的通信。而命名管道则没有此限制，它可以用于任何进程间的通信。通常情况下，管道和命名管道都是半双工。即同一时刻数据只能在一个方向上流动。虽然有些系统实现的管道是全双工的，但是为了移植性，还是不要假设管道由此功能。命名管道的实现，可以理解为处于内核中的一种缓冲区，由于命名管道的实现依赖于底层的虚拟文件系统，因此读写管道的方法就是使用读写文件的函数（read、write等）。命名管道实际上是first-in-first-out队列。mkfifo（const char *pathname,mode_t mode)，通过这个名字pathname创建命名管道。
	对命名管道在读写之前需要调用open函数执行打开操作，但对命名管道的打开操作可能会阻塞（默认），可以通过open的第二个参数mode来设置非阻塞方式打开。命名管道由于阻塞和非阻塞打开方式的不同，由调用read和write有很多种情况。命名管道不能自动增长，如果数据读出不及时，管道可能被占满。
	当多个进程或线程向同一个管道写入数据时，就需要考虑互斥问题。当每次写入管道的数据量不大于PIPE_BUF时，Linux会保证写入操作是原子的。如果大于PIPE_BUF，则可能和其他写操作写入的数据出现交错。可以通过pathconf函数获得PIPE_BUF的值。在实际编程中，为了移植性，我们不能假设管道的大小是某个值，必须通过pathconf函数进行获取。
	在实际编程中，如果两个进程向管道写入的数据有先后顺序要求，不能单凭write函数调用的先后顺序来保证。因为可能存在一种情况是管道中剩余的空间不足以容纳进程1要写入的数据量，而此时恰好进程2需要写入，那么进程2就会成功写入，而进程1会阻塞。如果读端此时用close关闭，那么进程1会收到SIGPIPE信号，从write函数出错返回，出错码是EPIPE。
	
	消息对象的序列化和反序列化
	因为不同的进程之间具有不同的地址空间，彼此之间的地址空间是隔离的，传递消息时不能像进程内线程间那样只要传递一个指针就好了，需要借助进程地址空间之外的空间去传递消息。将有结构的消息对象编程一段带结构信息的流数据（注意因为是“流”数据，是顺序的序列化数据！），然后通过命名管道将这段数据发送给目标进程，然后目标进程将这段带结构的数据还原成消息对象。消息的序列化和反序列化是使用命名管道进行进程间消息通信的基础。
	
	我们前面的[消息的发送]需要依赖CLExecutiveCommunication为基类的继承体系，我们的命名管道的消息通信也是派生于这个体系的，派生一个新类出来实现基于命名管道的消息发送。
	现在的问题是：1.不同类型的消息将由不同的字段，也需要不同的方法进行序列化。程序库只能看到基类CLMessage，而不能预知具体的CLMessage派生类的结构是什么。如果不采取特殊的处理方法，新增的这个发送类就得switch消息ID，然后再针对不同的消息ID进行不同的序列化。如果库的使用者要增加新的消息类型时，就得修改消息发送类的代码了。而程序库是不可能允许使用者这样做的。
	如何进行序列化和反序列化依赖于具体的消息类型，而到底会有什么样的消息，程序库是不能提前预知的，而程序库的任务是对不同的消息，提供序列化和反序列化功能。即：有不同的消息类型，有不同的序列化方法，由不同的反序列化方法，此即三个变化点。（我们将用三个继承体系来封装）
	还有一个问题是，进程内通信消息的接收依赖于CLMessageLoopManager为基类的继承体系，我们由CLMsgLoopManagerForSTLqueue类，实现基于queue容器的线程间[消息接收]。我们现在的反序列化任务，就是从CLMessageLoopManager派生一个新类，从一个管道接收消息的新类，而这个新类需要执行反序列化操作，把带结构信息的数据还原成对象。
	我们同样需要设计出消息对象序列化后在管道等中的布局。消息对象序列化后所得数据段布局也分为头部结构和消息的具体字段两部分构成，其中消息字段实际上就是指消息的各个字段在该数据段中的布局。当进行实际的通信时，同一时刻管道中很有可能存在多条消息。因此，我们需要利用头部结构标示出每条消息的结束处。所以在头部结构中，需要由字段标明气候跟随的消息具体字段的长度。
	用CLMessageSerializer类实现序列化，Serialize函数至少要返回两个信息：一是包含有消息序列化结果的内存，二是这段内存的大小。调用Serialize函数时，不仅需要包括消息的具体字段，还要包括其头部结构大小。派生类重写Serialize函数内部分配空间后，并不会填写头部结构部分的内容，这不是它的任务，而应该由基于管道的消息发送者填写。
	反序列化CLMessageDeserializer类中，Deserialize函数接收一个指针，这个指针指向不包含头部的消息字段，然后从内存区域读入具体的各个消息字段，然后创建对象。
	这种方法需要用户自己实现类的序列化和反序列化类，比较麻烦。如果C++中有反射机制则会简单很多。可以通过一个程序查找源码的方式自动生成这两个类吗？[要GOOGLE一下别人自动化的序列化与反序列化实现]
	
	命名管道型消息队列的封装（即消息接收机制的封装）
	命名管道既可以用于进程内通信，有可以用于进程间通信。当用于进程间通信时，则传递的消息对象必须要反序列化。进程间通信的时候，则只需要传递消息指针即可,不需要反序列化。所以，对于这个变化点，我们建立一个继承体系，然后再派生两个类。基类为CLMessageQueueByNamedPipe，此继承体系封装了是进程间发送消息还是进程内发送消息这个变化点。
	
	命名管道与消息循环的对接(即消息接收机制与消息处理机制通过消息循环来连接起来。)
	将命名管道与消息循环对接的意义是：命名管道作为消息的接收方式，而通过消息循环，将接收到的消息与处理消息的方式联系起来。将命名管道作为一种消息的传递方式，集成到消息循环中来。将命名管道作为接收消息的方式，需要实现消息循环相应的接口。进入消息循环对接，还有一点就是根据用户参数不同，创建不同类型的CLMessageQueueByNamedPipe派生类对象作为管道发送消息的机制。（进程内 Or 进程间管道）
	在Initialize()函数中，将每一个进程或线程名与一个进程内通信对象连接起来，以后给某个进程或线程发送消息，通过名字服务CLExecutiveNameServer就可以直接用名称即可。跨进程的管道通信对象也需要向名字服务进程注册，只是不在这里的Initialize()函数中注册。
	
	
	进入消息循环改造
	对应前面线程的消息循环进入方式，进入消息循环有两种途径：1.直接进入消息循环，等待由别的进程（线程）发来的消息（CLNonThreadForMsgLoop类） 2.创建一个新线程，让新线程进入消息循环，等待消息到达并处理。主线程可以干别的事情或者给新线程发送消息。（CLThreadForMsgLoop类）
	
	命名管道的消息发送机制的封装（即CLExecutiveCommunication继承体系对命名管道的扩展）
	此封装的目的是可以通过命名管道发送消息。
	
	**重要一点：理解命名管道，可以把它理解成操作系统内部提供的一个先进先出（fifo）队列缓冲区，其实现依赖于底层的虚拟文件系统，所以读写管道的方法就是使用读写文件的函数。
	
	进程的终止方式：
		有8种方式使进程终止，其中前5种为正常终止，它们是
		1：从 main 返回
		2：调用 exit
		3：调用 _exit 或 _Exit
		4：最后一个线程从其启动例程返回
		5：最后一个线程调用pthread_exit
		异常终止有3种，它们是
		6：调用 abort
		7：接到一个信号并终止
		8：最后一个线程对取消请求做出响应
	
	Debug技巧：
	eclipse中Debug运行后的工作目录在workspace/项目/的根目录下。
	这里遇到一个运行时错误，对同一个对象的两次释放导致了错误。在Debug的时候，可以利用断点和单步，通过查看函数调用栈来定位出错的位置。同时，在写程序的时候，一定要判断每一步是否可能出错，如果可能，则必须进行错误检查，否则当程序规模大了以后，出错了也查不出来。
	CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
	{
		//delete m_pMsgQueue; //这里不要再次释放m_pMsgQueue了，因为在处理线程收到退出消息后退出时的Uninitiated（）的会调用ReleaseCommunicationPtr释放通信队列，如果这里再释放一次会造成二次释放，会出错。
							
	}
	
	segmentation fault (core dumped)错误调试：
	Segmentation fault(段错误)是由于虚拟内存管理单元的异常所致，而该异常则通常是由于解引用一个未初始化或非法值的指针引起的。
    Linux中提供了core dump的功能，使得对这类错误的调试更为容易。一般情况下，linux系统是不允许产生core文件的，因此首先要解除这个限制：ulimit -c unlimited。接下来，运行含segmentation fault的段程序，如：./test。这时，segmentation fault的错误信息会显示为：Segmentation fault (core dumped)。在该文件夹下会出现一个名为core的文件，
    使用生成的core文件进行调试：gdb test core；(gdb) bt //bt是gdb中查看堆栈情况的命令，根据堆栈调用情况找到出问题的地方。
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
