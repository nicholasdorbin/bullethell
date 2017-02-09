#include "Engine/UI/BaseWidget.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/UI/UISystem.hpp"
#include "Engine/MathUtils.hpp"


const std::string FONT_NAME = "Data/Fonts/ArialFont.fnt";
BaseWidget::BaseWidget()
{
	m_stateProperties.resize(WidgetState_NUM_OF_STATES);

	BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
	float textWidth = bitmapFont->CalcTextWidth(m_label, m_textScale);

	if (textWidth > m_width)
		m_width = textWidth;

	m_position = Vector2(300.f, 300.f);
	float halfWidth = m_width / 2.f;
	float halfHeight = m_height / 2.f;

	Vector2 mins = Vector2(m_position.x - halfWidth, m_position.y - halfHeight);

	Vector2 maxs = Vector2(m_position.x + halfWidth, m_position.y + halfHeight);


	m_size = AABB2(mins, maxs);
	StoreValuesIntoAllStateProperties();
}

BaseWidget::~BaseWidget()
{
	//Nothing

}

void BaseWidget::Update(float)
{
	if (m_parentWidget)
	{
		if (m_parentWidget->m_activeState == WidgetState_HIDDEN || m_parentWidget->m_activeState == WidgetState_DISABLED)
		{
			m_activeState = m_parentWidget->m_activeState;
		}
		else
		{
			m_activeState = m_intendedState;
		}
	}
	else
	{
		m_activeState = m_intendedState;
	}
}

void BaseWidget::Render() const
{
	/*
	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);

	float textWidth = bitmapFont->CalcTextWidth(m_label, m_textScale);
	Vector3 drawTextPosition = Vector3(m_position);
	drawTextPosition.x -= textWidth / 2.f;


	float fontHeight = bitmapFont->CalcTextHeight(m_label, m_textScale);
	drawTextPosition.y += (m_height / 2.f) - (fontHeight / 2.f);

	Rgba overlayColor = Rgba::BLUE;
	g_theRenderer->DrawTexturedAABB(m_size, 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), overlayColor);

	g_theRenderer->DrawTextMeta2D(drawTextPosition, m_textScale, m_label, m_textColor, bitmapFont);
	*/
}

void BaseWidget::SetValuesFromNode(XMLNode& node)
{
	m_name = ReadXMLAttribute(node, "name", m_name);
	m_label = ReadXMLAttribute(node, "label", m_label);
	m_styleName = ReadXMLAttribute(node, "style", m_styleName);
	m_textScale = ReadXMLAttribute(node, "textScale", m_textScale);
	m_height = ReadXMLAttribute(node, "height", m_height);
	m_width = ReadXMLAttribute(node, "width", m_width);
	m_opacity = ReadXMLAttribute(node, "opacity", m_opacity);

	m_borderColor = ReadXMLAttribute(node, "borderColor", m_borderColor);
	m_backgroundColor = ReadXMLAttribute(node, "backgroundColor", m_backgroundColor);
	m_borderWidth = ReadXMLAttribute(node, "borderWidth", m_borderWidth);
	m_textColor = ReadXMLAttribute(node, "textColor", m_textColor);
	m_position = ReadXMLAttribute(node, "position", m_position);
	m_offset = ReadXMLAttribute(node, "offset", m_offset);


	//Events
	std::string showEventName = "";
	showEventName = ReadXMLAttribute(node, "showOnEvent", showEventName);
	if (!showEventName.empty())
	{
		RegisterEventCallback(showEventName, this, &BaseWidget::ShowWidgetEvent, true);
	}


	std::string hideEventName = "";
	hideEventName = ReadXMLAttribute(node, "hideOnEvent", hideEventName);
	if (!hideEventName.empty())
	{
		RegisterEventCallback(hideEventName, this, &BaseWidget::HideWidgetEvent, true);
	}

	m_onClickEventName = ReadXMLAttribute(node, "onClickEvent", m_onClickEventName);


	if (!m_isRootWidget)
	{
		SetPosition(GetWorldPosition());
		LoadFromStyle();
		StoreNonStyleValuesIntoAllProperties();
	}
}

void BaseWidget::StoreValuesIntoAllStateProperties()
{
	for (int i = 0; i < WidgetState_NUM_OF_STATES; i++)
	{
		WidgetState state = (WidgetState)i;
		m_stateProperties[state].Set("name", m_name);
		m_stateProperties[state].Set("label", m_label);
		m_stateProperties[state].Set("style", m_styleName);
		m_stateProperties[state].Set("textScale", m_textScale);
		m_stateProperties[state].Set("height", m_height);
		m_stateProperties[state].Set("width", m_width);
		m_stateProperties[state].Set("opacity", m_opacity); 
		m_stateProperties[state].Set("textOpacity", m_textOpacity);

		m_stateProperties[state].Set("borderColor", m_borderColor);
		m_stateProperties[state].Set("borderWidth", m_borderWidth);
		m_stateProperties[state].Set("backgroundColor", m_backgroundColor);
		m_stateProperties[state].Set("textColor", m_textColor);
		m_stateProperties[state].Set("position", m_position);
		m_stateProperties[state].Set("offset", m_offset);

	}
}

void BaseWidget::StoreNonStyleValuesIntoAllProperties()
{
	for (int i = 0; i < WidgetState_NUM_OF_STATES; i++)
	{
		WidgetState state = (WidgetState)i;
		m_stateProperties[state].Set("name", m_name);
		m_stateProperties[state].Set("label", m_label);
		m_stateProperties[state].Set("style", m_styleName);
		m_stateProperties[state].Set("opacity", m_opacity);
		m_stateProperties[state].Set("textOpacity", m_textOpacity);

		m_stateProperties[state].Set("position", m_position);
		m_stateProperties[state].Set("offset", m_offset);

	}
}

void BaseWidget::SetParent(BaseWidget* parent)
{
	m_parentWidget = parent;

	if (!m_parentWidget->m_styleName.empty())
	{
		m_styleName = m_parentWidget->m_styleName;
	}
}

void BaseWidget::LoadFromStyle()
{
	g_theUISystem->LoadPropertiesOfStyleName(m_styleName, m_type, m_stateProperties);
}

void BaseWidget::SetPosition(Vector2 pos)
{
	m_position = pos;
	float halfWidth = m_width / 2.f;
	float halfHeight = m_height / 2.f;

	Vector2 mins = Vector2(m_position.x - halfWidth, m_position.y - halfHeight);

	Vector2 maxs = Vector2(m_position.x + halfWidth, m_position.y + halfHeight);;


	m_size = AABB2(mins, maxs);
}

Vector2 BaseWidget::GetWorldPosition()
{
	if (m_isRootWidget)
	{
		return m_position;
	}

	Vector2 root = Vector2::ZERO;
	if (m_parentWidget)
	{
		root = m_parentWidget->GetWorldPosition();
	}

	

	m_position = root + m_offset;

	return m_position;
}

void BaseWidget::SetState(WidgetState newState)
{
	m_intendedState = newState;
}

void BaseWidget::ShowWidgetEvent(NamedProperties&)
{
	SetState(WidgetState_ENABLED);
}

void BaseWidget::HideWidgetEvent(NamedProperties&)
{
	SetState(WidgetState_HIDDEN);
}

void BaseWidget::OnClick()
{
	if (m_onClickEventName.empty())
		return;

	FireEvent(m_onClickEventName);
}

BaseWidget* BaseWidget::GetWidgetPointIsInside(Vector2& cursorPos)
{
	
	//checks myself and children and returns the child that it's in.
	if (IsPointInBounds(cursorPos, m_size))
	{
		return this;
	}
	return nullptr;
}
