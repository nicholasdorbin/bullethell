#include "Engine/UI/ButtonWidget.hpp"

#include "Engine/UI/UISystem.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/UI/ButtonWidget.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"



const std::string FONT_NAME = "Data/Fonts/ArialFont.fnt";

ButtonWidget::ButtonWidget()
{
	m_type = WidgetType_BUTTON;

	m_borderColor = Rgba::WHITE;
	m_backgroundColor = Rgba::BLACK;
	m_textColor = Rgba::WHITE;
	m_opacity = 1.f;

	m_height = 50.f;
	m_width = 50.f;

	StoreValuesIntoAllStateProperties();
}

ButtonWidget::~ButtonWidget()
{
	//Nothing
}

void ButtonWidget::SetValuesFromNode(XMLNode& node)
{
	BaseWidget::SetValuesFromNode(node);
}

void ButtonWidget::StoreValuesIntoAllStateProperties()
{
	BaseWidget::StoreValuesIntoAllStateProperties();
}

void ButtonWidget::Update(float deltaSeconds)
{
	if (m_activeState == WidgetState_HIGHLIGHTED || m_intendedState == WidgetState_HIGHLIGHTED)
	{
		//int breakhere = 0;
	}
	BaseWidget::Update(deltaSeconds);
}

void ButtonWidget::Render() const
{
	
	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);

	const NamedProperties& currentStateProperties = m_stateProperties[m_activeState];

	std::string label = m_label;

	float textScale;
	currentStateProperties.Get("textScale", textScale);

	float textWidth = bitmapFont->CalcTextWidth(label, textScale);



	Vector3 drawTextPosition = Vector3(m_position);
	drawTextPosition.x -= textWidth / 2.f;


	float fontHeight = bitmapFont->CalcTextHeight(label, textScale);
	drawTextPosition.y += (m_height / 2.f) - (fontHeight / 2.f);


	Rgba overlayColor;
	currentStateProperties.Get("backgroundColor", overlayColor);
	Rgba borderColor;
	currentStateProperties.Get("borderColor", borderColor);
	float borderWidth;
	currentStateProperties.Get("borderWidth", borderWidth);
	float opacity;
	currentStateProperties.Get("opacity", opacity);

	borderColor.SetAFloat(borderColor.GetAFloat() * opacity);
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


	Rgba textColor;
	currentStateProperties.Get("textColor", textColor);
	float textOpacity;
	currentStateProperties.Get("textOpacity", textOpacity);
	textColor.SetAFloat(textColor.GetAFloat() * textOpacity);

	g_theRenderer->DrawTextMeta2D(drawTextPosition, textScale, label, textColor, bitmapFont);
	
}
