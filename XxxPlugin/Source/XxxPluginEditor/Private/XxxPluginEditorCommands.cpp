// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "XxxPluginEditorCommands.h"
#include "EditorStyleSet.h"

FXxxPluginEditorCommands::FXxxPluginEditorCommands() :
	TCommands<FXxxPluginEditorCommands>(
		TEXT("My Plugin Commands"), 
		FText::FromString(TEXT("Commands to control My Plugin")), 
		NAME_None, 
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
		FAppStyle::GetAppStyleSetName()
#else
		FEditorStyle::GetStyleSetName()
#endif
	)
{}

void FXxxPluginEditorCommands::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "XxxPluginLoc"
	UI_COMMAND(OpenXxxPluginWindow, "My Plugin", "Opens My Plugin Window", EUserInterfaceActionType::Check, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::M));
#undef LOCTEXT_NAMESPACE
}