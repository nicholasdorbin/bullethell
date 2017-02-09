
#include "Engine/UI/GroupWidget.hpp"

#include "Engine/UI/UISystem.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/UI/ButtonWidget.hpp"
#include "Engine/Renderer/Renderer.hpp"

GroupWidget::GroupWidget()
{
	m_type = WidgetType_GROUP;

	m_borderColor = Rgba::CLEAR;
	m_backgroundColor = Rgba::CLEAR;
	m_textColor = Rgba::CLEAR;
	m_opacity = 1.f;

	m_height = g_theUISystem->m_screenHeight;
	m_width = g_theUISystem->m_screenWidth;

	float halfWidth = m_width / 2.f;
	float halfHeight = m_height / 2.f;
	m_position = Vector2(halfWidth, halfHeight);

	m_size = AABB2(Vector2::ZERO, Vector2(m_width, m_height));

	StoreValuesIntoAllStateProperties();
}



GroupWidget::~GroupWidget()
{
	for each (BaseWidget* child in m_children)
	{
		delete child;
	}
}

void GroupWidget::SetValuesFromNode(XMLNode& node)
{

	BaseWidget::SetValuesFromNode(node);
	//Set Group Specific Attributes

	//#TODO Show / Hide Events

	//Create Children and store them
	int numberOfNodes = node.nChildNode();
	for (int i = 0; i < numberOfNodes; i++)
	{
		XMLNode childNode = node.getChildNode(i);

		std::string nodeName = childNode.getName();

		if (nodeName == "Group")
		{
			BaseWidget* newGroupWidget = new GroupWidget();
			newGroupWidget->SetParent(this);
			newGroupWidget->SetValuesFromNode(childNode);
			m_children.push_back(newGroupWidget);
		}
		else if (nodeName == "Button")
		{
			BaseWidget* newButtonWidget = new ButtonWidget();
			newButtonWidget->SetParent(this);
			newButtonWidget->SetValuesFromNode(childNode);
			m_children.push_back(newButtonWidget);
		}
		else
		{
			BaseWidget* newBaseWidget = new BaseWidget();
			newBaseWidget->SetParent(this);
			newBaseWidget->SetValuesFromNode(childNode);
			m_children.push_back(newBaseWidget);
		}
	}
}

void GroupWidget::StoreValuesIntoAllStateProperties()
{
	BaseWidget::StoreValuesIntoAllStateProperties();
}

void GroupWidget::Update(float deltaSeconds)
{
	BaseWidget::Update(deltaSeconds);
	for each (BaseWidget* child in m_children)
	{
		child->Update(deltaSeconds);	
	}
}

void GroupWidget::Render() const
{
	const NamedProperties& currentStateProperties = m_stateProperties[m_activeState];

	Rgba overlayColor; 
	currentStateProperties.Get("backgroundColor", overlayColor);
	Rgba borderColor;
	currentStateProperties.Get("borderColor", borderColor);
	float borderWidth;
	currentStateProperties.Get("borderWidth", borderWidth);
	float opacity;
	currentStateProperties.Get("opacity", opacity);

	borderColor.SetAFloat( borderColor.GetAFloat() * opacity);
	overlayColor.SetAFloat(overlayColor.GetAFloat() * opacity);

	Vector2 interiorMins = m_size.m_mins;
	interiorMins.x += borderWidth;
	interiorMins.y += borderWidth;

	Vector2 interiorMaxs = m_size.m_maxs;
	interiorMaxs.x -= borderWidth;
	interiorMaxs.y -= borderWidth;

	AABB2 interiorBounds = AABB2(interiorMins, interiorMaxs);

	g_theRenderer->DrawTexturedAABB(m_size, 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), borderColor);

	g_theRenderer->DrawTexturedAABB(interiorBounds, 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), overlayColor);

	for each (BaseWidget* child in m_children)
	{
		if (child->m_activeState != WidgetState_HIDDEN)
		{
			child->Render();
		}
	}
}

void GroupWidget::SetState(WidgetState newState)
{
	if (newState == WidgetState_HIGHLIGHTED || newState == WidgetState_PRESSED)
		return;
	m_intendedState = newState;
}

BaseWidget* GroupWidget::GetWidgetPointIsInside(Vector2& cursorPos)
{
	/*
		For each child
	{
		W* widget = child->GetWidgetPointIsInside(p);
		If (widget)
			Return widget;
	}
	//check self
	Return BaseWidget::GetWidgetPointIsInside

	*/

	for each (BaseWidget* child in m_children)
	{
		BaseWidget* widget = child->GetWidgetPointIsInside(cursorPos);
		if (widget)
			return widget;
	}
	return BaseWidget::GetWidgetPointIsInside(cursorPos);
}
