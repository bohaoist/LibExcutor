Design Principle

General Pourpose:学习设计模式，学习封装Linux API，练习C++
Method：先看书，然后把书放在一边，自己设计，自己实现！（自己做很重要！照着书上敲代码很多时候是敲了也不知道别人为什么那样做！）

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
	
	
	Debug技巧：
	这里遇到一个运行时错误，对同一个对象的两次释放导致了错误。在Debug的时候，可以利用断点和单步，通过查看函数调用栈来定位出错的位置。同时，在写程序的时候，一定要判断每一步是否可能出错，如果可能，则必须进行错误检查，否则当程序规模大了以后，出错了也查不出来。
	CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
	{
		//delete m_pMsgQueue; //这里不要再次释放m_pMsgQueue了，因为在处理线程收到退出消息后退出时的Uninitiated（）的会调用ReleaseCommunicationPtr释放通信队列，如果这里再释放一次会造成二次释放，会出错。
							
	}
	
	segmentation fault (core dumped)错误调试：
	Segmentation fault(段错误)是由于虚拟内存管理单元的异常所致，而该异常则通常是由于解引用一个未初始化或非法值的指针引起的。
    Linux中提供了core dump的功能，使得对这类错误的调试更为容易。一般情况下，linux系统是不允许产生core文件的，因此首先要解除这个限制：ulimit -c unlimited。接下来，运行含segmentation fault的段程序，如：./test。这时，segmentation fault的错误信息会显示为：Segmentation fault (core dumped)。在该文件夹下会出现一个名为core的文件，
    使用生成的core文件进行调试：gdb test core；(gdb) bt //bt是gdb中查看堆栈情况的命令，根据堆栈调用情况找到出问题的地方。
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	