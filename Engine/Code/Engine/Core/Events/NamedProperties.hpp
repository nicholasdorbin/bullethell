#pragma once
#include <vector>
#include <map>


enum PropertyFetchResult
{
	PROPERTY_FETCH_SUCCESS = 0,
	PROPERTY_FETCH_FAILED_NO_PROPERTIES,
	PROPERTY_FETCH_FAILED_NO_SUCH_PROPERTY,
	PROPERTY_FETCH_FAILED_WRONG_TYPE
};

class NamedPropertyBase
{
	public:
		virtual ~NamedPropertyBase()
		{

		}
};

template <typename T>
class TypedNamedProperty : public NamedPropertyBase
{
public:
	TypedNamedProperty(const T& data) : m_data(data) 
	{

	}
	TypedNamedProperty(TypedNamedProperty const & copy)
	{
		m_data = copy.m_data;
	}
	virtual ~TypedNamedProperty() override
	{

	}

	T m_data;
};

class NamedProperties
{
public:

	NamedProperties();
	NamedProperties(NamedProperties const & copy);
	~NamedProperties();

	template< typename T >
	NamedProperties(const std::string& name1, const T& value1);
	template< typename T >
	NamedProperties(const std::string& name1, const T& value1, const std::string& name2, const T& value2);
	template< typename T >
	NamedProperties(const std::string& name1, const T& value1, const std::string& name2, const T& value2, const std::string& name3, const T& value3);


	std::map<size_t, NamedPropertyBase*> m_properties;

	template< typename T >
	void Set(const std::string& propertyName, const T& typedDataValue);

	void Set(const std::string& propertyName, const char* typedDataValue);
	

	template< typename T >
	PropertyFetchResult Get(const std::string& propertyName, T& out_typedDataValue) const;

};

template< typename T >
NamedProperties::NamedProperties(const std::string& name1, const T& value1)
{
	Set(name1, value1);
}

template< typename T >
NamedProperties::NamedProperties(const std::string& name1, const T& value1, const std::string& name2, const T& value2)
{
	Set(name1, value1);
	Set(name2, value2);
}

template< typename T >
NamedProperties::NamedProperties(const std::string& name1, const T& value1, const std::string& name2, const T& value2, const std::string& name3, const T& value3)
{
	Set(name1, value1);
	Set(name2, value2);
	Set(name3, value3);
}

template< typename T >
PropertyFetchResult NamedProperties::Get(const std::string& propertyName, T& out_typedDataValue) const
{
	//dynamic cast, if null then it's not of that type
	//find the thing
	//if not found, ERROR_MISSING


	if (m_properties.size() == 0)
	{
		return PROPERTY_FETCH_FAILED_NO_PROPERTIES;
	}

	size_t hashStr = std::hash<std::string>{}(propertyName);
	auto found = m_properties.find(hashStr);

	if (found == m_properties.end())
	{
		return PROPERTY_FETCH_FAILED_NO_SUCH_PROPERTY;
	}

	NamedPropertyBase* property = found->second;
	if (property == nullptr)
	{
		return PROPERTY_FETCH_FAILED_NO_SUCH_PROPERTY;
	}
	TypedNamedProperty<T>* typeProperty = dynamic_cast<TypedNamedProperty<T>*>(property);

	if (typeProperty == nullptr)
	{
		return PROPERTY_FETCH_FAILED_WRONG_TYPE;
	}


	out_typedDataValue = typeProperty->m_data;
	return PROPERTY_FETCH_SUCCESS;
}

template< typename T >
void NamedProperties::Set(const std::string& propertyName, const T& typedDataValue)
{
	size_t hashStr = std::hash<std::string>{}(propertyName);
	auto found = m_properties.find(hashStr);
	if (found == m_properties.end())
	{
		//Make a new entry in the map and insert it
		TypedNamedProperty<T>* newProperty = new TypedNamedProperty<T>(typedDataValue);
		m_properties.insert(std::pair<size_t, NamedPropertyBase*>(hashStr, newProperty));
		//return CREATED_NEW_DATA;
	}
	else
	{
		//Replace the current value
		TypedNamedProperty<T> * check = dynamic_cast<TypedNamedProperty<T>*>(m_properties[hashStr]);

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
			m_properties[hashStr] = new TypedNamedProperty<T>(typedDataValue);
			//return TYPE_CHANGED;
		}
	}
}