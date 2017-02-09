#pragma once
#include <map>
#include <string>

#include "Engine/UI/UICommon.hpp"
#include "Engine/Core/Events/NamedProperties.hpp"

class UISystem;
class WidgetStyle;
class GroupWidget;
class BaseWidget;
class IntVector2;


extern UISystem* g_theUISystem;

class UISystem
{
public:

	float m_screenWidth = 0.f;
	float m_screenHeight = 0.f;

	UISystem();
	UISystem(float screenHeight, float screenWidth, void* windowHandle);
	~UISystem();

	std::map<std::string, WidgetStyle*> m_styles;
	std::map<std::string, GroupWidget*> m_UILayouts;
	BaseWidget* m_highlightedWidget = nullptr;

	void* m_window = nullptr;


	void LoadAllStyles();
	WidgetStyle* FindStyle(std::string& styleName);
	void LoadPropertiesOfStyleName(std::string& styleName, WidgetType widgetType, std::vector<NamedProperties>& out_Properties);

	void LoadAllUIs();

	void Render() const;
	void Update(float deltaSeconds);

	BaseWidget* FindHighlightedWidget();
	IntVector2 GetCursorVirtualPos();
	BaseWidget* GetWidgetAtPos(IntVector2 cursorAtPos);
	void SetClickedState(NamedProperties& params);
	void FireClickEvent(NamedProperties& params);
};