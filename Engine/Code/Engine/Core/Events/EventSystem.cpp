#include "Engine/Core/Events/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"



EventSystem* EventSystem::s_instance;

EventMap* EventSystem::s_events;


EventSystem* EventSystem::GetInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new EventSystem;
	}
	return s_instance;
}

void EventSystem::Init()
{
	s_events = new EventMap;
}

void EventSystem::Shutdown()
{
	for (const EventPair& subscriberIter : *s_events)
	{
		delete subscriberIter.second;
	}

	delete s_events;

	delete s_instance;
}

void EventSystem::RegisterEvent(const std::string& eventName, EventCallback* eventFunc, void* subscriber)
{
	EventSubscribers* subs = FindOrCreateEvent(eventName);

	EventSubscriber newSub = { eventFunc, subscriber };
	subs->push_back(newSub);
}

void EventSystem::TriggerEvent(const std::string& eventName, Event* eventData)
{
	//Find all subscribers and trigger them
	EventSubscribers* subs = FindEvent(eventName);

	if (subs != nullptr)
	{
		for (const EventSubscriber& subscriberIter : *subs)
		{
			subscriberIter.eventFunc(eventData, subscriberIter.subscriber);
		}
	}
}

void EventSystem::UnregisterFromEvent(void* subscriber, const std::string& eventName)
{
	EventSubscribers* subs = FindEvent(eventName);

	if (subs != nullptr)
	{
		auto subIter = subs->begin();
		while (subIter != subs->end())
		{
			if (subIter->subscriber == subscriber)
			{
				subIter = subs->erase(subIter);
				break;
			}
			else
			{
				++subIter;
			}
		}

	}
}

void EventSystem::UnregisterFromAllEvents(void* subscriber)
{
	auto eventIter = s_events->begin();

	while (eventIter != s_events->end())
	{
		EventSubscribers* subs = eventIter->second;
		if (subs != nullptr)
		{
			auto subIter = subs->begin();
			while (subIter != subs->end())
			{
				if (subIter->subscriber == subscriber)
				{
					subIter = subs->erase(subIter);
				}
				else
				{
					++subIter;
				}
			}

		}
		++eventIter;
	}

	
}

EventSubscribers* EventSystem::FindOrCreateEvent(const std::string& eventName)
{
	size_t eventID = StringHash(eventName);

	auto eventIter = s_events->find(eventID);

	if (eventIter != s_events->end())
	{
		//Found
		return eventIter->second;
	}
	else
	{
		//Not found. Add it.
		EventPair newPair(eventID, new EventSubscribers);
		s_events->insert(newPair);
		return newPair.second;
	}
}

EventSubscribers* EventSystem::FindEvent(const std::string& eventName)
{
	size_t eventID = StringHash(eventName);
	EventMap::iterator subIter = s_events->find(eventID);

	if (subIter == s_events->end())
	{
		return nullptr;
	}

	return subIter->second;
}
