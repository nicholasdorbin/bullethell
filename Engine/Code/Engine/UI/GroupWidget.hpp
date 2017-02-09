#pragma once

#include "Engine/UI/BaseWidget.hpp"
#include <vector>
struct XMLNode;

class GroupWidget : public BaseWidget
{
public:

	std::vector<BaseWidget*> m_children;
	GroupWidget();
	virtual ~GroupWidget() override;


	virtual void SetValuesFromNode(XMLNode& node);
	virtual void StoreValuesIntoAllStateProperties();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	virtual void SetState(WidgetState newState);

	virtual BaseWidget* GetWidgetPointIsInside(Vector2& cursorPos);
};