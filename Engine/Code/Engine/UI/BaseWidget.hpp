#pragma once
#include <string>
#include <vector>
#include "Engine/Math/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/UI/UICommon.hpp"
#include "Engine/Core/Events/NamedProperties.hpp"


class NamedProperties;
struct XMLNode;

class BaseWidget
{
public:
	int m_layer = 0;
	WidgetType m_type = WidgetType_BASE;

	WidgetState m_activeState = WidgetState_ENABLED; //Display state
	WidgetState m_intendedState = WidgetState_ENABLED; //What I'd like to be, unless parent says otherwise.

	std::vector<NamedProperties> m_stateProperties;


	std::string m_name = "UNNAMED WIDGET";
	std::string m_label = "TEXT";
	std::string m_styleName = "";
	std::string m_onClickEventName = "";

	float m_textScale = 1.f;
	float m_width = 100.f;
	float m_height = 100.f;
	float m_opacity = 1.f;
	float m_borderWidth = 0.f;
	float m_textOpacity = 1.f;

	Rgba m_borderColor = Rgba::WHITE;
	Rgba m_backgroundColor = Rgba::BLACK;
	Rgba m_textColor = Rgba::WHITE;

	Vector2 m_position = Vector2::ZERO;
	Vector2 m_offset = Vector2::ZERO;
	
	
	AABB2 m_size = AABB2(Vector2::ZERO, Vector2(100.f, 100.f));

	bool m_isRootWidget = false;



	WidgetAlignment m_alignment = WidgetAlignment_CENTER;

	BaseWidget* m_parentWidget = nullptr;

	BaseWidget();
	virtual ~BaseWidget();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	virtual void SetValuesFromNode(XMLNode& node);
	virtual void StoreValuesIntoAllStateProperties();
	virtual void StoreNonStyleValuesIntoAllProperties();

	void SetParent(BaseWidget* parent);

	virtual void LoadFromStyle();

	void SetPosition(Vector2 pos);
	Vector2 GetWorldPosition();

	virtual void SetState(WidgetState newState);

	virtual void ShowWidgetEvent(NamedProperties& params);
	virtual void HideWidgetEvent(NamedProperties& params);
	virtual void OnClick();

	virtual BaseWidget* GetWidgetPointIsInside(Vector2& cursorPos);


};