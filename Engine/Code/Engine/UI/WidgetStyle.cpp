#include "Engine/UI/WidgetStyle.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Core/Events/NamedProperties.hpp"


//---------------------------------------------------------------------------------------------------
// WidgetStyle
//---------------------------------------------------------------------------------------------------

WidgetStyle::WidgetStyle(XMLNode& styleRootNode)
{
	m_styleName = ReadXMLAttribute(styleRootNode, "name", m_styleName);

	

	//Create an instance of each Info for each Widget Type
	for (unsigned int i = 0; i < WidgetType_NUM_OF_TYPES; i++)
	{
		WidgetStyleInfo* newStyleInfo = new WidgetStyleInfo();
		newStyleInfo->m_type = (WidgetType)i;
		m_widgetStyleInfos.push_back(newStyleInfo);
	}


	XMLNode AllWidgetsNode = styleRootNode.getChildNode("AllWidgets");
	if (!AllWidgetsNode.isEmpty())
	{

		SetAllWidgets(AllWidgetsNode);
		
	}


	//Set values on a per widget basis (excluding the All Widgets node)
	int numberOfNodes = styleRootNode.nChildNode();
	for (int i = 0; i < numberOfNodes; i++)
	{
		XMLNode childNode = styleRootNode.getChildNode(i);

		std::string nodeName = childNode.getName();
		if (nodeName == "AllWidgets")
		{
			continue;
		}

		if (nodeName == "Group")
		{
			m_widgetStyleInfos[WidgetType_GROUP]->SetValuesFromNode(childNode);
		}
		else if (nodeName == "Button")
		{
			m_widgetStyleInfos[WidgetType_BUTTON]->SetValuesFromNode(childNode);
		}
		else
		{
			m_widgetStyleInfos[WidgetType_BASE]->SetValuesFromNode(childNode);
		}
	}

}




WidgetStyle::~WidgetStyle()
{
	for each (WidgetStyleInfo* info in m_widgetStyleInfos)
	{
		delete info;
	}
}

void WidgetStyle::SetAllWidgets(XMLNode& allWidgetsNode)
{
	//Iterate thru all containers and set them to these values
	for each (WidgetStyleInfo* widget in m_widgetStyleInfos)
	{
		widget->SetValuesFromNode(allWidgetsNode);

		
	}
}

StateInfo* WidgetStyle::FindInfoForWidgetState(WidgetType widgetType, WidgetState widgetState)
{
	return m_widgetStyleInfos[widgetType]->FindInfoForState(widgetState);
}

void WidgetStyle::CopyPropertiesForWidget(WidgetType widgetType, std::vector<NamedProperties>& out_properties)
{
	WidgetStyleInfo* info =  m_widgetStyleInfos[widgetType];


	for (int i = 0; i < WidgetState_NUM_OF_STATES; i++)
	{
		WidgetState state = (WidgetState)i;
		//Ask squirrel wtf
		out_properties[state].Set("borderColor", info->m_states[state]->m_borderColor);
		out_properties[state].Set("borderWidth", info->m_states[state]->m_borderWidth);
		out_properties[state].Set("backgroundColor", info->m_states[state]->m_backgroundColor);
		out_properties[state].Set("textColor", info->m_states[state]->m_textColor);
		out_properties[state].Set("textScale", info->m_states[state]->m_textScale);
		//out_properties[state] = NamedProperties(info->m_stateProperties[state]);
	}
}

//---------------------------------------------------------------------------------------------------
// WidgetStyleInfo
//---------------------------------------------------------------------------------------------------




WidgetStyleInfo::WidgetStyleInfo()
{
	//Make defaults for all States
	for (unsigned int i = 0; i < WidgetState_NUM_OF_STATES; i++)
	{
		StateInfo* newStateInfo = new StateInfo();
		newStateInfo->m_state = (WidgetState)i;
		m_states.push_back(newStateInfo);
	}
	m_stateProperties.resize(WidgetState_NUM_OF_STATES);
}

WidgetStyleInfo::~WidgetStyleInfo()
{
	//std::vector<StateInfo*> m_states; //index == the enum
	//std::vector<NamedProperties> m_stateProperties;
	

	for each (StateInfo* info in m_states)
	{
		delete info;
	}
	m_states.clear();
	
}

void WidgetStyleInfo::SetValuesFromNode(XMLNode& widgetNode)
{
	//Search for an AllStates node
	XMLNode AllStatesNode = widgetNode.getChildNode("AllStates");
	if (!AllStatesNode.isEmpty())
	{
		SetAllStates(AllStatesNode);
	}



	//Iterate thru all nodes (that aren't AllStates) and set them
	int numberOfNodes = widgetNode.nChildNode();

	/*
	WidgetState_DISABLED = 0,
	WidgetState_ENABLED,
	WidgetState_HIGHLIGHTED,
	WidgetState_PRESSED,
	WidgetState_HIDDEN,
	*/
	for (int i = 0; i < numberOfNodes; i++)
	{
		XMLNode childNode = widgetNode.getChildNode(i);

		std::string nodeName = childNode.getName();
		if (nodeName == "AllStates")
		{
			continue;
		}
		else if (nodeName == "Disabled")
		{
			m_states[WidgetState_DISABLED]->SetStateInfoValuesFromNode(childNode);
			SetNamedPropertiesFromStoredValues(WidgetState_DISABLED);
		}
		else if (nodeName == "Enabled")
		{
			m_states[WidgetState_ENABLED]->SetStateInfoValuesFromNode(childNode);
			SetNamedPropertiesFromStoredValues(WidgetState_ENABLED);
		}
		else if (nodeName == "Highlighted")
		{
			m_states[WidgetState_HIGHLIGHTED]->SetStateInfoValuesFromNode(childNode);
			SetNamedPropertiesFromStoredValues(WidgetState_HIGHLIGHTED);
		}
		else if (nodeName == "Pressed" || nodeName == "Clicked")
		{
			m_states[WidgetState_PRESSED]->SetStateInfoValuesFromNode(childNode);
			SetNamedPropertiesFromStoredValues(WidgetState_PRESSED);
		}
		else if (nodeName == "Hidden")
		{
			m_states[WidgetState_HIDDEN]->SetStateInfoValuesFromNode(childNode);
			SetNamedPropertiesFromStoredValues(WidgetState_HIDDEN);
		}
		//If we don't support this state then don't add anything
		
	}

}

void WidgetStyleInfo::SetNamedPropertiesFromStoredValues(WidgetState state)
{
	StateInfo* info = m_states[state];

	NamedProperties& data = m_stateProperties[state];

	/* Properties:
	borderColor
	borderWidth
	backgroundColor
	textColor
	*/

	data.Set("borderColor", info->m_borderColor);
	data.Set("borderWidth", info->m_borderWidth);
	data.Set("backgroundColor", info->m_backgroundColor);
	data.Set("textColor", info->m_textColor);
	data.Set("textScale", info->m_textScale);
}

void WidgetStyleInfo::SetAllStates(XMLNode& allStates)
{
	
	for each (StateInfo* state in m_states)
	{
		state->SetStateInfoValuesFromNode(allStates);
		SetNamedPropertiesFromStoredValues(state->m_state);
	}
}







StateInfo* WidgetStyleInfo::FindInfoForState(WidgetState widgetState)
{
	return m_states[widgetState];
}

//---------------------------------------------------------------------------------------------------
// StateInfo
//---------------------------------------------------------------------------------------------------





StateInfo::StateInfo()
{

}

StateInfo::~StateInfo()
{
	
}

void StateInfo::SetStateInfoValuesFromNode(XMLNode& stateNode)
{
	/* Properties:
		borderColor
		borderWidth
		backgroundColor
		textColor
	*/

	
	m_borderColor = ReadXMLAttribute(stateNode, "borderColor", m_borderColor);
	m_borderWidth = ReadXMLAttribute(stateNode, "borderWidth", m_borderWidth);

	m_backgroundColor = ReadXMLAttribute(stateNode, "backgroundColor", m_backgroundColor);

	m_textColor = ReadXMLAttribute(stateNode, "textColor", m_textColor);
	m_textScale = ReadXMLAttribute(stateNode, "textScale", m_textScale);
	
}
