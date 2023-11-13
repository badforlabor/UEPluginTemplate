// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Class handling UICommands of the editor.
 * Currently only "Open My Plugin" commands is required.
 * It is done via commands, because we want to have a keyboard shortcut for it.
 */

class XXXPLUGINEDITOR_API FXxxPluginEditorCommands : public TCommands<FXxxPluginEditorCommands>
{

public:

	FXxxPluginEditorCommands();
	void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> OpenXxxPluginWindow;
};
