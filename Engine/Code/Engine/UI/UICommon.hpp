#pragma once

enum WidgetAlignment
{
	WidgetAlignment_TOP_LEFT = 0,
	WidgetAlignment_TOP_CENTER,
	WidgetAlignment_TOP_RIGHT,
	WidgetAlignment_CENTER_LEFT,
	WidgetAlignment_CENTER,
	WidgetAlignment_CENTER_RIGHT,
	WidgetAlignment_BOTTOM_LEFT,
	WidgetAlignment_BOTTOM_CENTER,
	WidgetAlignment_BOTTOM_RIGHT
};

enum WidgetState
{
	WidgetState_DISABLED = 0,
	WidgetState_ENABLED,
	WidgetState_HIGHLIGHTED,
	WidgetState_PRESSED,
	WidgetState_HIDDEN,
	WidgetState_NUM_OF_STATES
};

enum WidgetType
{
	WidgetType_BASE = 0,
	WidgetType_GROUP,
	WidgetType_BUTTON,
	WidgetType_NUM_OF_TYPES
};