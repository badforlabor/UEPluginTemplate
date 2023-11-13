// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "XxxPluginEditorBase.h"
#include "XxxPluginEditorWidget.h"

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif

#include "EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "LevelEditor.h"

void UXxxPluginEditorBase::Init()
{
	// Empty virtual function - to be overridden
}

void UXxxPluginEditorBase::InitializeTheWidget()
{
	// Empty virtual function - to be overridden
}

void UXxxPluginEditorBase::SetEditorTab(const TSharedRef<SDockTab>& NewEditorTab)
{
	EditorTab = NewEditorTab;
}

UEditorUtilityWidgetBlueprint* UXxxPluginEditorBase::GetUtilityWidgetBlueprint()
{
	// Get the Editor Utility Widget Blueprint from the content directory.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath("/XxxPlugin/XxxPluginWidget_BP.XxxPluginWidget_BP"));
#else
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath("/XxxPlugin/XxxPluginWidget_BP.XxxPluginWidget_BP");
#endif
	return Cast<UEditorUtilityWidgetBlueprint>(AssetData.GetAsset());
}

bool UXxxPluginEditorBase::CanCreateEditorUI()
{
	// Editor UI can be created only when we have proper Editor Utility Widget Blueprint available.
	return GetUtilityWidgetBlueprint() != nullptr;
}

TSharedRef<SWidget> UXxxPluginEditorBase::CreateEditorUI()
{
	// Register OnMapChanged event so we can properly handle Tab and Widget when changing levels.
	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().AddUObject(this, &UXxxPluginEditorBase::ChangeTabWorld);

	// Create the Widget
	return CreateEditorWidget();
}

TSharedRef<SWidget> UXxxPluginEditorBase::CreateEditorWidget()
{
	TSharedRef<SWidget> CreatedWidget = SNullWidget::NullWidget;
	if (UEditorUtilityWidgetBlueprint* UtilityWidgetBP = GetUtilityWidgetBlueprint())
	{
		// Create Widget from the Editor Utility Widget BP.
		CreatedWidget = UtilityWidgetBP->CreateUtilityWidget();

		// Save the pointer to the created Widget and initialize it.
		EditorWidget = Cast<UXxxPluginEditorWidget>(UtilityWidgetBP->GetCreatedWidget());
		if (EditorWidget)
		{
			InitializeTheWidget();
		}
	}

	// Returned Widget will be docked into the Editor Tab.
	return CreatedWidget;
}

void UXxxPluginEditorBase::ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType)
{
	// Handle the event when editor map changes.
	if (MapChangeType == EMapChangeType::TearDownWorld)
	{
		// If the world is destroyed - set the Tab content to null and null the Widget.
		if (EditorTab.IsValid())
		{
			EditorTab.Pin()->SetContent(SNullWidget::NullWidget);
		}
		if (EditorWidget)
		{
			EditorWidget->Rename(nullptr, GetTransientPackage());
			EditorWidget = nullptr;
		}
	}
	else if (MapChangeType == EMapChangeType::NewMap || MapChangeType == EMapChangeType::LoadMap)
	{
		// If the map has been created or loaded and the Tab is valid - put a new Widget into this Tab.
		if (EditorTab.IsValid())
		{
			EditorTab.Pin()->SetContent(CreateEditorWidget());
		}
	}
}
