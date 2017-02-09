#pragma once

#include "Engine/UI/BaseWidget.hpp"
#include <vector>
struct XMLNode;

class ButtonWidget : public BaseWidget
{
public:
	ButtonWidget();
	virtual ~ButtonWidget() override;

	virtual void SetValuesFromNode(XMLNode& node);
	virtual void StoreValuesIntoAllStateProperties();

	virtual void Update(float deltaSeconds);
	virtual void Render() const;
};