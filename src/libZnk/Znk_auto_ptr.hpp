#ifndef INCLUDE_GUARD__Znk_auto_ptr_hpp__
#define INCLUDE_GUARD__Znk_auto_ptr_hpp__

/* This header is for C++ only. */

#include <Znk_base.h>

/**
 * @brief
 *   auto_ptr. which allows you to set the deleter function/functor in any way you like.
 *   The following is an class to discard it automatically by specifying a typical pointer type
 *   object and a deleter function/functor for the user.
 *
 * @exam
 *   ...
 *   test(){
 *     // MyObj can be pointer type suffixed with *, as in Data*
 *     // or can be typedef object as in MyData by 'typedef Data* MyData'.
 *     Znk_AutoPtr<MyObj> obj_ap( MyObj_create("MyName",...), MyObj_destroy );
 *     MyObj obj = obj_ap.get();
 *
 *     ... Processing using obj(MyObj type) ...
 *
 *     // Upon an exception or the exit of a function, obj is released by an automatic call
 *     // to MyObj_destroy( obj ).
 *   }
 *
 * @note
 *   There are already many smart pointers in the world, but I dare to state the background of this
 *   introduction.
 *
 *   First of all, we are against the use of smart pointers of this kind (including this Znk_AutoPtr),
 *   especially in large scale developments. The best option is to not use C++, but that's not going
 *   to happen. And since we are using C++, it is almost unpredictable when and where an exception
 *   will occur, and in that case, we need to wrap the dynamically allocated pointer object in this
 *   kind of class or else it will leak when an exception occurs. As I had to do, I reluctantly
 *   provided such a class in libZnk. 
 *
 *   The main motivation for using this kind of class is frankly just that. The often-quoted hype
 *   about not having to make the last release function call or not having to worry about forgetting
 *   to call the last function is not really a labor-saver or a relief at all. This is a minor issue
 *   compared to the more nefarious problems that exist with the existing smart pointers. I will
 *   briefly explain about this as well.
 *
 *
 *   std::auto_ptr:
 *
 *   This can't set its own functions/functors etc. for delete. ( We can override operator 'delete'
 *   to force it, but we should not override the global operator 'delete'.  This would only add to
 *   the list of things to worry about.  In addition, there can be some quirks and even glitches
 *   between the different compilers. )
 *
 *
 *   boost::shared_ptr:
 *
 *   This of course requires boost to be installed. It is possible to extract only the implementation
 *   from boost, but even if you focus on Windows/Linux only, you will need at least 5 or 6 files,
 *   which is a little too many for this function alone. And they are all headers. In particular,
 *   the implementation of lock/unlock(see boost/smart_ptr/detail/spinlock.hpp) and atomic operations
 *   ( for multi-threaded assignment processing) are not hidden.  Although it is enclosed in namespace,
 *   we don't like to have these implementation details visible to all the code that uses shared_ptr
 *   scattered around the global area.
 *
 *   The implementation should be hidden as much as possible, especially if it is complex and
 *   processor-dependent as described above. Also, the widespread proliferation of complex
 *   implementations over a wide range of compilations increases the risk of the widespread spread of
 *   errors due to unknown bugs in code that even the authors did not anticipate. Especially in this
 *   kind of infrastructural class, we have to be that much more cautious about that risk. For example,
 *   prior to boost v1.38, the following typical code actually had a compile error on bcc 5.5.1.
 *
 *     // where sp is shared_ptr and flag is of type bool
 *     if( sp && flag ){ ...Processing usisng sp... }
 *
 *   We can write sp.get() instead of sp, but it is even more troublesome because other compilers can
 *   compile the above without any error. That's probably why even good programmers like the authors of
 *   boost couldn't see this problem for a long time. To be more specific, this problem was caused by
 *   a rather tricky trick called unspecified_bool_type in boost::shared_ptr. You might think that you
 *   can just remove it, but doing so will cause problems in a different environment. Since v1.39, the
 *   following type conversion operator overrides have been added and countermeasures are taken.
 *
 *     operator bool () const { return px != 0; }
 *
 *   Even if we take an optimistic view and assume that unexpected and unknown defects no longer exist, 
 *   It often happens that certain quirks of a class and its usage are incompatible, and local problems
 *   are accidental. When this happens, it is OUR SIDE that uses it that must track the cause. And in
 *   the case of a class such as smart pointers, where there is a lot of automation going on behind the
 *   scenes that does not appear in the code at first glance, the cause analysis becomes extremely more
 *   difficult and even more so because it's easy to fall into wicked situations.
 *
 *   
 *   General reference-counter GC:
 *
 *   This is not really a case where the lifetime of a pointer object deviates from the per-function
 *   management scheme and this needs to be extended quasi-global. Even if such a thing seems necessary
 *   at first glance, it is often the case that it is poorly designed and should be reviewed first.
 *
 *   There are fewer cases than you might think where the lifetime of a pointer object is forced to
 *   deviate from the per-function management scheme. Even if such a thing is seemingly necessary,
 *   it can usually be avoided by its design. We should review that first.
 *
 *
 *   Znk_AutoPtr:
 *
 *   It's no surprise that providing excessive functionality in a single product will eventually
 *   cause more wrinkles than it's merits due to the complexity of the product. This kind of problem
 *   exists in any library or class, but the more complex the implementation, the more difficult
 *   the task becomes. In order to mitigate it as much as possible, it is important to defend against
 *   the complexity at the entrance by setting restrictions and error checks to eliminate it as much
 *   as possible, or else the complexity should be distributed and hidden instead of being
 *   concentrated in one place.
 *
 *   Now, after describing it at great length so far, Znk_AutoPtr is a simple enough implementation
 *   that it doesn't need to be explained, and it's the type of class we often see scattered about.
 *   There is no concept of ownership like std::auto_ptr, and it does not support any extra
 *   processing.  Even the assignment process between Znk_AutoPtr and Znk_AutoPtr is completely
 *   prohibited.  We can assume that it is only for initialization at the beginning of the function. 
 *
 *   The benefit of this is of course that its implementation is THE SIMPLEST. This means that you
 *   don't have to worry about potential bugs, and if they do occur, they are easy to track. 
 *   However, even if it is this simple, automatic calls to the destructor can cause situations that
 *   are difficult to track in the debugger. While the traditional C style of explicitly calling a
 *   release function at the end may seem cumbersome when writing code, it is still the most reliable
 *   and quickest way to solve a problem when it comes time to track down a problem.
 *
 *   That's why we disagree with the choice to use this kind of rapper for large scale development. 
 *   ( It may go against your intuition, though. )
 */
template< typename PtrT >
class Znk_AutoPtr{
public:
	typedef void (*DelT)( PtrT ptr );
	Znk_AutoPtr( PtrT ptr, DelT del )
		: m_ptr( ptr )
		, m_del( del )
	{}
	~Znk_AutoPtr(){
		m_del( m_ptr );
	}
	PtrT get() const // never throws
	{
		return m_ptr;
	}
private:
	PtrT m_ptr;
	DelT m_del;
	/* Assignment operator are forbidden. */
	Znk_AutoPtr( const Znk_AutoPtr& ); Znk_AutoPtr& operator=( const Znk_AutoPtr& );
};

/**
 * @brief
 *   Takes two arguments, the first one being a pointer to be released and the second one being of
 *   an arbitrary type (whose value is determined at the time of creation).
 *   Deleter functions/functors can be set freely.
 *
 *   This is a case of a release function that takes a main pointer type as its first argument and
 *   specifies an auxiliary value as its second argument. These cases are also common, and this is
 *   a Znk_AutoPtr for such a case.
 *
 * @exam
 *   The deleter to be registered must be the function which the return value is of void type,
 *   the first argument is of a pointer type to be released and the second argument is of an
 *   arbitrary type. When the function exits, deleter will be called automatically and it
 *   releases the pointer.
 *
 *   test1(){
 *     Znk_AutoPtrArg<Obj> obj_ap( Obj_create_null(), Obj_destroy, true );
 *     Obj obj = obj_ap.get();
 *
 *     ... Processing using obj(Obj type) ...
 *
 *     // Upon an exception or the exit of a function, obj is released by an automatic call
 *     // to Obj_destroy( obj, true ).
 *     // The value specified in the third argument of the constructor of AutoPtrArg is used as
 *     // it is as the second argument of the deleter specified by del.
 *   }
 *
 * @note
 *   Basically the same caveats as Znk_AutoPtr.
 */
template< typename PtrT, typename ArgT >
class Znk_AutoPtrArg{
public:
	typedef void (*DelT)( PtrT ptr, ArgT arg );
	Znk_AutoPtrArg( PtrT ptr, DelT del, ArgT arg )
		: m_ptr( ptr )
		, m_del( del )
		, m_arg( arg )
	{}
	~Znk_AutoPtrArg(){
		m_del( m_ptr, m_arg );
	}
	PtrT get() const // never throws
	{
		return m_ptr;
	}
	ArgT arg() const // never throws
	{
		return m_arg;
	}
private:
	PtrT m_ptr;
	DelT m_del;
	ArgT m_arg;
	/* Assignment operator are forbidden. */
	Znk_AutoPtrArg( const Znk_AutoPtrArg& ); Znk_AutoPtrArg& operator=( const Znk_AutoPtrArg& );
};


/**
 * @brief
 *   Declares AutoPtr or AutoPtrArg that holds type PtrT.
 *   At the same time declares a PtrT type pointer variable named 'name' and furthermore initializes
 *   the variable 'name' with the value obtained from the get() of this AutoPtr.
 *   This is a macro version of the patterns commonly used in AutoPtr/AutoPtrArg.
 */
#define Znk_AUTO_PTR( PtrT, name, ptr, del ) \
	Znk_AutoPtr< PtrT > name##__ap__( ptr, del ); \
	PtrT name = name##__ap__.get()

#define Znk_AUTO_PTR_ARG( PtrT, name, ptr, del, ArgT, arg ) \
	Znk_AutoPtrArg< PtrT, ArgT > name##__ap__( ptr, del, arg ); \
	PtrT name = name##__ap__.get()

#endif /* INCLUDE_GUARD */
