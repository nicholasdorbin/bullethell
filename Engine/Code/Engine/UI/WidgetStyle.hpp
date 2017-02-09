#pragma once
#include <string.h>
#include <map>
#include <vector>
#include "Engine/Math/Rgba.hpp"
#include "Engine/UI/UICommon.hpp"

struct XMLNode;
class NamedProperties;

struct StateInfo
{
	WidgetState m_state = WidgetState_ENABLED;
	Rgba m_backgroundColor = Rgba::BLACK;
	Rgba m_borderColor = Rgba::WHITE;
	Rgba m_textColor = Rgba::WHITE;

	float m_borderWidth = 1.f;
	float m_textScale = 1.f;


	StateInfo();
	~StateInfo();
	void SetStateInfoValuesFromNode(XMLNode& stateNode);
};

struct WidgetStyleInfo
{
	WidgetType m_type = WidgetType_BASE;
	std::vector<StateInfo*> m_states; //index == the enum
	std::vector<NamedProperties> m_stateProperties;

	WidgetStyleInfo();
	~WidgetStyleInfo();

	void SetValuesFromNode(XMLNode& widgetNode);
	void SetNamedPropertiesFromStoredValues(WidgetState state);
	void SetAllStates(XMLNode& allStates);
	StateInfo* FindInfoForState(WidgetState widgetState);
};

class WidgetStyle
{
	public: 
		std::vector<WidgetStyleInfo*> m_widgetStyleInfos;
		std::string m_styleName = "UNNAMED STYLE";

		WidgetStyle(XMLNode& styleRootNode);
		~WidgetStyle();

		void SetAllWidgets(XMLNode& allWidgetsNode);
		StateInfo* FindInfoForWidgetState(WidgetType widgetType, WidgetState widgetState);
		//NamedProperties* FindPropertiesForWidgetState(WidgetType widgetType, WidgetState widgetState);
		void CopyPropertiesForWidget(WidgetType widgetType, std::vector<NamedProperties>& out_properties);
};