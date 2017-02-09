#pragma once
#ifndef NAMED_EVENT_SYSTEM
#define NAMED_EVENT_SYSTEM
#include <map>
#include <vector>

#include "Engine/Core/Events/NamedProperties.hpp"


class NamedEventSystem;

extern NamedEventSystem* g_theEventSystem;

typedef void (EventCallbackFunc)(NamedProperties& params);


struct RegisteredObjectBase
{

	bool m_isUsable = false;
	virtual void Execute(NamedProperties& params) = 0;

};

template< typename T_ObjectType, typename T_MethodType>
struct RegisterObjectMethod : public RegisteredObjectBase
{
	
	T_ObjectType* m_object;
	T_MethodType m_method;

	
	RegisterObjectMethod(T_ObjectType* obj, T_MethodType method) : m_object(obj), m_method(method) {};

	void Execute(NamedProperties& params)
	{
		(m_object->*m_method)(params);
	}
};


struct RegisterFunction : public RegisteredObjectBase
{
	EventCallbackFunc* m_func;


	RegisterFunction(EventCallbackFunc* func) : m_func(func) {};

	void Execute(NamedProperties& params)
	{
		(*m_func)(params);
	}
};

class NamedEventSystem
{
public:
	std::map <std::string, std::vector<RegisteredObjectBase*> > m_registeredObjs;
	~NamedEventSystem();

};

static void FireEvent(const std::string& eventName, bool checkIfUsable = false)
{

	NamedProperties emptyProperties;
	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);
	if (iter != g_theEventSystem->m_registeredObjs.end())
	{
		std::vector<RegisteredObjectBase*> listOfFuncs = iter->second;
		
		for (unsigned int i = 0; i < listOfFuncs.size(); i++)
		{
			if (checkIfUsable)
			{
				if (!listOfFuncs[i]->m_isUsable)
					continue;
			}
			listOfFuncs[i]->Execute(emptyProperties);
		}
	}
	
}

static void FireEvent(const std::string& eventName, NamedProperties& properties, bool checkIfUsable = false)
{
	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);
	if (iter != g_theEventSystem->m_registeredObjs.end())
	{
		std::vector<RegisteredObjectBase*> listOfFuncs = iter->second;

		for (unsigned int i = 0; i < listOfFuncs.size(); i++)
		{
			if (checkIfUsable)
			{
				if (!listOfFuncs[i]->m_isUsable)
					continue;
			}
			listOfFuncs[i]->Execute(properties);
		}
	}

}

static void RegisterEventCallback(const std::string& eventName, EventCallbackFunc* eventFunc, bool isUsable = false)
{
	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);
	if (iter != g_theEventSystem->m_registeredObjs.end())
	{

		//Confirm the EventFunc is not in the list already
		for (unsigned int i = 0; i < iter->second.size(); i++)
		{
			RegisterFunction* checkFunc = dynamic_cast<RegisterFunction*>(iter->second[i]);

			if (checkFunc == nullptr)
				continue;
			if (checkFunc->m_func == eventFunc)
			{
				return; //Already registered, exit
			}
		}
		//Add it to list
		RegisterFunction* newFunc = new RegisterFunction(eventFunc);
		newFunc->m_isUsable = isUsable;
		iter->second.push_back(newFunc);
	}
	else
	{
		//Insert new entry
		std::vector<RegisteredObjectBase*> newObjectVector;
		RegisterFunction* newFunc = new RegisterFunction(eventFunc);
		newFunc->m_isUsable = isUsable;
		newObjectVector.push_back(newFunc);

		g_theEventSystem->m_registeredObjs.insert(std::pair<std::string, std::vector<RegisteredObjectBase*>>(eventName, newObjectVector));
	}
}

template< typename T_ObjectType, typename T_MethodType>
static void RegisterEventCallback(const std::string& eventName, T_ObjectType* object, T_MethodType method, bool isUsable = false)
{

	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);

	if (iter != g_theEventSystem->m_registeredObjs.end())
	{
		//Insert into the vector

		for (unsigned int i = 0; i < iter->second.size(); i++)
		{
			RegisterObjectMethod<T_ObjectType, T_MethodType>* checkObj = dynamic_cast<RegisterObjectMethod<T_ObjectType, T_MethodType>*>(iter->second[i]);

			if (checkObj == nullptr)
				continue;

			if ((void*)checkObj->m_object == object && checkObj->m_method == method)
			{
				return; //Already registered, exit
			}
		}
		//Confirm the RegisterObjectMethod is not in the list already
		RegisterObjectMethod<T_ObjectType, T_MethodType>* newEventMethod = new RegisterObjectMethod<T_ObjectType, T_MethodType>(object, method);
		newEventMethod->m_isUsable = isUsable;
		iter->second.push_back(newEventMethod);
	}
	else
	{
		//Make a new vector
		RegisterObjectMethod<T_ObjectType, T_MethodType>* newEventMethod = new RegisterObjectMethod<T_ObjectType, T_MethodType>(object, method);
		newEventMethod->m_isUsable = isUsable;
		std::vector<RegisteredObjectBase*> newObjectVector;
		newObjectVector.push_back(newEventMethod);

		g_theEventSystem->m_registeredObjs.insert(std::pair<std::string, std::vector<RegisteredObjectBase*>>(eventName, newObjectVector));
	}
}

static void UnregisterEvent(const std::string& eventName, EventCallbackFunc* eventFunc)
{
	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);

	if (iter != g_theEventSystem->m_registeredObjs.end())
	{
		
		for (unsigned int i = 0; i < iter->second.size(); i++)
		{
			RegisterFunction* checkFunc = dynamic_cast<RegisterFunction*>(iter->second[i]);

			if (checkFunc == nullptr)
				continue;

			if (checkFunc->m_func == eventFunc)
			{
				delete checkFunc;
				iter->second[i] = nullptr;
				iter->second.erase(iter->second.begin() + i);
				return; 
			}
		}
	}
}

template< typename T_ObjectType, typename T_MethodType>
static void UnregisterEvent(const std::string& eventName, T_ObjectType* object, T_MethodType method)
{
	auto iter = g_theEventSystem->m_registeredObjs.find(eventName);

	if (iter != g_theEventSystem->m_registeredObjs.end())
	{

		for (unsigned int i = 0; i < iter->second.size(); i++)
		{
			RegisterObjectMethod<T_ObjectType, T_MethodType>* checkObj = dynamic_cast<RegisterObjectMethod<T_ObjectType, T_MethodType>*>(iter->second[i]);

			if (checkObj == nullptr)
				continue;

			if (checkObj->m_object == object && checkObj->m_method == method)
			{
				delete checkObj;
				iter->second[i] = nullptr;
				iter->second.erase(iter->second.begin() + i);
				return;
			}
		}
	}
}




void FireEventForEachFileFound(const std::string& eventToFire, const std::string& baseFolder, const std::string& filePattern, bool recurseSubfolders = false);

#endif