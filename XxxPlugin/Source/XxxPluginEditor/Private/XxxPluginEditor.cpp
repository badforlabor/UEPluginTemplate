// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "XxxPluginEditor.h"
#include "XxxPluginEditorWidget.h"

#include "EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "LevelEditor.h"

void UXxxPluginEditor::Init()
{
	// Put initialization code here
}

void UXxxPluginEditor::InitializeTheWidget()
{
	// Initialize the widget here
	EditorWidget->SetNumberOfTestButtonPressed(NumberOfTestButtonPressed);

	// Bind all required delegates to the Widget.
	EditorWidget->OnTestButtonPressedDelegate.BindUObject(this, &UXxxPluginEditor::OnTestButtonPressed);
}

void UXxxPluginEditor::OnTestButtonPressed()
{
	// Button on the widget has been pressed. Increase the counter and inform the widget about it.
	NumberOfTestButtonPressed++;
	EditorWidget->SetNumberOfTestButtonPressed(NumberOfTestButtonPressed);
}


