#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/WidgetStyle.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"

#include "Engine/UI/BaseWidget.hpp"
#include "Engine/UI/GroupWidget.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/MathUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

UISystem* g_theUISystem;

UISystem::UISystem()
{

}

UISystem::UISystem(float screenHeight, float screenWidth, void* windowHandle)
{
	m_window = windowHandle;
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;
	RegisterEventCallback("OnClickPress", this, &UISystem::SetClickedState, true);
	RegisterEventCallback("OnClickRelease", this, &UISystem::FireClickEvent, true);
}

UISystem::~UISystem()
{

	std::map< std::string, GroupWidget* >::iterator itLayout = m_UILayouts.begin();
	while (itLayout != m_UILayouts.end()) {
		delete(itLayout->second);
		itLayout = m_UILayouts.erase(itLayout);
	}

	
	//delete styles
	std::map< std::string, WidgetStyle* >::iterator itStyle = m_styles.begin();
	while (itStyle != m_styles.end()) {
		delete(itStyle->second);
		itStyle = m_styles.erase(itStyle);
	}
}

void UISystem::LoadAllStyles()
{
	std::vector< std::string > styleFiles = EnumerateFilesInFolder("Data/UI",
		"*.Style.xml");

	for each (std::string str in styleFiles)
	{
		XMLNode rootNode = XMLNode::openFileHelper(str.c_str());
		int styleCount = rootNode.nChildNode(); //# of UIStyle nodes
		
		if (rootNode.IsContentEmpty())
		{
			continue;
		}
		for (int i = 0; i < styleCount; i++)
		{
			XMLNode styleNode = rootNode.getChildNode(i);

			std::string nodeName = styleNode.getName();
			if (styleNode.IsContentEmpty() || nodeName != "UIStyle")
				continue;


			WidgetStyle* newStyle = new WidgetStyle(styleNode);


			m_styles.insert(std::pair<std::string, WidgetStyle*>(newStyle->m_styleName, newStyle));
		}


	}
}

WidgetStyle* UISystem::FindStyle(std::string& styleName)
{
	auto found = m_styles.find(styleName);

	if (found == m_styles.end())
	{
		return nullptr;
	}
	else
	{
		return found->second;
	}
}

void UISystem::LoadPropertiesOfStyleName(std::string& styleName, WidgetType widgetType, std::vector<NamedProperties>& out_Properties)
{
	WidgetStyle* style = FindStyle(styleName);

	if (style == nullptr)
		return;
	style->CopyPropertiesForWidget(widgetType, out_Properties);
	
}

void UISystem::LoadAllUIs()
{
	//Find all Files of UI type
	std::vector< std::string > layoutFiles = EnumerateFilesInFolder("Data/UI",
		"*.Layout.xml");

	for each (std::string str in layoutFiles)
	{
		XMLNode rootNode = XMLNode::openFileHelper(str.c_str());
		int layoutCount = rootNode.nChildNode(); //# of UILayout nodes

		if (rootNode.IsContentEmpty())
		{
			continue;
		}

		//Make the Root GroupWidget
		
		for (int i = 0; i < layoutCount; i++)
		{
			GroupWidget* newRootUIWidget = new GroupWidget();
			newRootUIWidget->m_isRootWidget = true;
			XMLNode layoutNode = rootNode.getChildNode(i);
			newRootUIWidget->SetValuesFromNode(layoutNode);
			m_UILayouts.insert(std::pair<std::string, GroupWidget*>(newRootUIWidget->m_name, newRootUIWidget));
		}
		
	}
}

void UISystem::Render() const
{
	auto iter = m_UILayouts.begin();

	while (iter != m_UILayouts.end())
	{
		GroupWidget* baseWidget = iter->second;

		if (baseWidget->m_activeState == WidgetState_ENABLED)
		{
			baseWidget->Render();
		}

		++iter;
	}
}

void UISystem::Update(float deltaSeconds)
{

	BaseWidget* newHighlighted = FindHighlightedWidget();
	if (newHighlighted != m_highlightedWidget)
	{
		if (m_highlightedWidget)
			m_highlightedWidget->SetState(WidgetState_ENABLED);
		if (newHighlighted)
			newHighlighted->SetState(WidgetState_HIGHLIGHTED);
		m_highlightedWidget = newHighlighted;
	}



	auto iter = m_UILayouts.begin();
	while (iter != m_UILayouts.end())
	{
		GroupWidget* baseWidget = iter->second;
		baseWidget->Update(deltaSeconds);

		++iter;
	}
}

BaseWidget* UISystem::FindHighlightedWidget()
{
	IntVector2 cursorVirtualPos = GetCursorVirtualPos();
	BaseWidget* widgetAtPos = GetWidgetAtPos(cursorVirtualPos);
	return widgetAtPos;

	
}

IntVector2 UISystem::GetCursorVirtualPos()
{
	POINT p;
	GetCursorPos(&p);
 	HWND window = (HWND)m_window;
 	ScreenToClient(window, &p);
	IntVector2 result = IntVector2((int)p.x, (int)p.y);
	result.y = (int)ClampFloatCircular((float)-result.y, 0.f, (float)m_screenHeight);
	return result;
}

BaseWidget* UISystem::GetWidgetAtPos(IntVector2 cursorAtPos)
{
	Vector2 position = Vector2((float)cursorAtPos.x, (float)cursorAtPos.y);
	//#TODO Figure out how to make this work if there are multiple levels of widget active. Care about how many layers deep we go. (Return value perhaps?)

	BaseWidget* highlighted = nullptr;
	auto iter = m_UILayouts.begin();
	while (iter != m_UILayouts.end())
	{
		GroupWidget* baseWidget = iter->second;
		
		highlighted = baseWidget->GetWidgetPointIsInside(position);
		++iter;
	}
	return highlighted;
}

void UISystem::SetClickedState(NamedProperties&)
{
	if (m_highlightedWidget)
	{
		m_highlightedWidget->SetState(WidgetState_PRESSED);
	}
}

void UISystem::FireClickEvent(NamedProperties&)
{
	if (m_highlightedWidget)
	{
		m_highlightedWidget->OnClick();
	}
}
