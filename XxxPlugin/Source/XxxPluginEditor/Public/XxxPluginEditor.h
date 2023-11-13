// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once

#include "XxxPluginEditorBase.h"
#include "XxxPluginEditor.generated.h"

/**
 * Editor object which handles all of the logic of the Plugin.
 */

UCLASS()
class XXXPLUGINEDITOR_API UXxxPluginEditor : public UXxxPluginEditorBase
{

	GENERATED_BODY()

public:

	// UXxxPluginEditorBase implementation
	void Init() override;

protected:

	// UXxxPluginEditorBase implementation
	void InitializeTheWidget();

public:

	/**
	 * Called when the test button has been pressed on the widget.
	 */
	void OnTestButtonPressed();

	// Test variable
	int32 NumberOfTestButtonPressed = 0;
};
