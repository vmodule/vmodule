/*
 * SingletonHelper.h
 *
 *  Created on: Jan 1, 2017
 *      Author: jeffrey
 */
#pragma once
#include <memory>
namespace vmodlue
{

template<class T>
class GlobalsSingleton
{
	/**
	 * This thing just deletes the shared_ptr when the 'instance'
	 * goes out of scope (when the bss segment of the compilation unit
	 * that 'instance' is sitting in is deinitialized). See the comment
	 * on 'instance' for more information.
	 */
	template<class K>
	class Deleter
	{
	public:
		K* guarded;
		~Deleter()
		{
			if (guarded)
				delete guarded;
		}
	};

	/**
	 * Is it possible that getInstance can be called prior to the shared_ptr 'instance'
	 *  being initialized as a global? If so, then the shared_ptr constructor would
	 *  effectively 'reset' the shared pointer after it had been set by the prior
	 *  getInstance call, and a second instance would be created. We really don't
	 *  want this to happen so 'instance' is a pointer to a smart pointer so that
	 *  we can deterministally handle its construction. It is guarded by the
	 *  Deleter class above so that when the bss segment that this static is
	 *  sitting in is deinitialized, the shared_ptr pointer will be cleaned up.
	 */
	static Deleter<std::shared_ptr<T> > instance;

	/**
	 * See 'getQuick' below.
	 */
	static T* quick;
public:

	/**
	 * Retrieve an instance of the singleton using a shared pointer for
	 *  referenece counting.
	 */
	inline static std::shared_ptr<T> getInstance()
	{
		if (!instance.guarded)
		{
			if (!quick)
				quick = new T;
			instance.guarded = new std::shared_ptr<T>(quick);
		}
		return *(instance.guarded);
	}

	/**
	 * This is for quick access when using form (2) of the pattern. Before 'mdd' points
	 * it out, this might be a case of 'solving problems we don't have' but this access
	 * is used frequently within the event loop so any help here should benefit the
	 * overall performance and there is nothing complicated or tricky here and not
	 * a lot of code to maintain.
	 */
	inline static T* getQuick()
	{
		if (!quick)
			quick = new T;

		return quick;
	}

};

template<class T>
typename GlobalsSingleton<T>::template Deleter<std::shared_ptr<T> > GlobalsSingleton<
		T>::instance;
template<class T>
T* GlobalsSingleton<T>::quick;

/**
 * This is another bit of hackery that will act as a flag for
 *  whether or not a global/static has been initialized yet. An instance
 *  should be placed in the cpp file after the static/global it's meant to
 *  monitor.
 */
class InitFlag
{
public:
	InitFlag(bool& flag)
	{
		flag = true;
	}
};
};

/**
 * For pattern (2) above, you can use the following macro. This pattern is safe to
 * use in all cases but may be very slightly less efficient.
 *
 * Also, you must also use a #define to replace the actual global variable since
 * there's no way to use a macro to add a #define. An example would be:
 *
 * VMODULE_GLOBAL_REF(CApplication, g_Application);
 * #define g_Windowing VMODULE_GLOBAL_USE(CApplication)
 *
 */
#define VMODULE_GLOBAL_REF(classname,g_variable) \
  static std::shared_ptr<classname> g_variable##Ref(vmodlue::GlobalsSingleton<classname>::getInstance())

/**
 * This declares the actual use of the variable. It needs to be used in another #define
 * of the form:
 *
 * #define g_variable VMODULE_GLOBAL_USE(classname)
 */
#define VMODULE_GLOBAL_USE(classname) (*(vmodlue::GlobalsSingleton<classname>::getQuick()))
