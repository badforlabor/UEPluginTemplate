// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "XxxPluginEditorWidget.h"

void UXxxPluginEditorWidget::TestButtonPressed()
{
	OnTestButtonPressedDelegate.ExecuteIfBound();
}
