#include "Engine/Core/Events/NamedProperties.hpp"


NamedProperties::NamedProperties()
{

}

NamedProperties::NamedProperties(NamedProperties const & copy)
{
	auto iter = copy.m_properties.begin();
	while (iter != copy.m_properties.end())
	{
		NamedPropertyBase * namedProperty = new NamedPropertyBase(*iter->second);
		m_properties.insert(std::pair<size_t, NamedPropertyBase*>(iter->first, namedProperty));
		++iter;
	}
}

NamedProperties::~NamedProperties()
{
	for (auto iter = m_properties.begin(); iter != m_properties.end(); ++iter)
	{
		delete iter->second;
	}
	m_properties.clear();
}

void NamedProperties::Set(const std::string& propertyName, const char* typedDataValue)
{
	size_t hashStr = std::hash<std::string>{}(propertyName);
	auto found = m_properties.find(hashStr);
	if (found == m_properties.end())
	{
		//Make a new entry in the map and insert it
		TypedNamedProperty<std::string>* newProperty = new TypedNamedProperty<std::string>(typedDataValue);
		m_properties.insert(std::pair<size_t, NamedPropertyBase*>(hashStr, newProperty));
		//return CREATED_NEW_DATA;
	}
	else
	{
		//Replace the current value
		TypedNamedProperty<std::string> * check = dynamic_cast<TypedNamedProperty<std::string>*>(m_properties[hashStr]);

		//check != null, same type
		if (check)
		{
			check->m_data = typedDataValue;
			//return TYPE_SET;
		}
		//check == null, different type
		else
		{
			delete m_properties[hashStr];
			m_properties[hashStr] = new TypedNamedProperty<std::string>(typedDataValue);
			//return TYPE_CHANGED;
		}
	}
}

/*
auto found = m_properties.find(propertyName);

if (found == m_properties.end())
{
	//Make a new entry in the map and insert it
	TypedNamedProperty<std::string>* newProperty = new TypedNamedProperty<std::string>(std::string(typedDataValue));
	m_properties.insert(std::pair<std::string, NamedPropertyBase*>(propertyName, newProperty));
}
else
{
	//Replace the current value

	NamedPropertyBase* property = found->second;
	TypedNamedProperty<std::string>* typeProperty = dynamic_cast<TypedNamedProperty<std::string>*>(property);

	TypedNamedProperty<std::string>* newProperty = dynamic_cast<TypedNamedProperty<std::string>*>(found->second);
	found->second = std::string(typedDataValue);




	m_properties[propertyName] = static_cast<NamedPropertyBase*>(new TypedNamedProperty<const char*>(typedDataValue));
}
*/