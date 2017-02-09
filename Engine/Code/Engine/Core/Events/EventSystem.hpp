#pragma once




#include <string>
#include <map>
#include <vector>


//-----------------------------------------------------------------------------------------------
// #TODO - reevaluate having the event system as a singleton
// #TODO - evaluate removing event from event map if it has no subs


//-----------------------------------------------------------------------------------------------
// Should always be inherited
struct Event {};


//-----------------------------------------------------------------------------------------------
typedef void EventCallback(Event* eventData, void* subArg);


//-----------------------------------------------------------------------------------------------
struct EventSubscriber
{
	EventCallback* eventFunc;
	void* subscriber;
};
typedef std::vector<EventSubscriber> EventSubscribers;


//-----------------------------------------------------------------------------------------------
typedef std::pair<size_t, EventSubscribers*> EventPair;
typedef std::map<size_t, EventSubscribers*> EventMap;
typedef EventMap::iterator EventMapIter;


//-----------------------------------------------------------------------------------------------
template <class Subscriber, void(Subscriber::*Func)(Event*)>
void MethodEventStub(Event* eventData, void* subArg)
{
	Subscriber* sub = (Subscriber*)subArg;
	(sub->*Func)(eventData);
}


//-----------------------------------------------------------------------------------------------
class EventSystem
{
public:
	static EventSystem* GetInstance();
	void Init();
	void Shutdown();

	void RegisterEvent(const std::string& eventName, EventCallback* eventFunc, void* subscriber);

	template <class Subscriber, void(Subscriber::*Func)(Event*)>
	void RegisterEvent(const std::string& eventName, Subscriber* sub)
	{
		RegisterEvent(eventName, MethodEventStub<Subscriber, Func>, sub);
	}

	void TriggerEvent(const std::string& eventName, Event* eventData);

	void UnregisterFromEvent(void* subscriber, const std::string& eventName);
	void UnregisterFromAllEvents(void* subscriber);

private:
	EventSystem() {}
	~EventSystem() {}

	EventSubscribers* FindOrCreateEvent(const std::string& eventName);
	EventSubscribers* FindEvent(const std::string& eventName);

	static EventSystem* s_instance;
	static EventMap* s_events;
};

