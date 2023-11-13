// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "XxxPluginEditorModule.h"
#include "XxxPluginEditor.h"
#include "XxxPluginEditorCommands.h"
#include "XxxPluginEditorStyle.h"

#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"

#include "PropertyEditorModule.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FXxxPluginEditorModule"

IMPLEMENT_MODULE(FXxxPluginEditorModule, FXxxPluginEditor)

// Id of the XxxPlugin Tab used to spawn and observe this tab.
const FName XxxPluginTabId = FName(TEXT("MyXxxPlugin"));
static const FName XxxPluginMoreActionsTabName("XxxPluginMoreActions");

void FXxxPluginEditorModule::StartupModule()
{
	// Register Styles.
	FXxxPluginEditorStyle::Initialize();
	FXxxPluginEditorStyle::ReloadTextures();

	// Register UICommands.
	FXxxPluginEditorCommands::Register();

	// Register OnPostEngineInit delegate.
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FXxxPluginEditorModule::OnPostEngineInit);

	// Create and initialize Editor object.
	Editor = NewObject<UXxxPluginEditorBase>(GetTransientPackage(), UXxxPluginEditor::StaticClass());
	Editor->Init();

	OnRegisterStartupCallbackHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FXxxPluginEditorModule::OnRegisterStartupCallback));
	
	// Register Tab Spawner. Do not show it in menu, as it will be invoked manually by a UICommand.
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		XxxPluginTabId,
		FOnSpawnTab::CreateRaw(this, &FXxxPluginEditorModule::SpawnEditor),
		FCanSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> bool { return CanSpawnEditor(); })
	)
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FXxxPluginEditorStyle::GetStyleSetName(), "XxxPluginEditorStyle.MenuIcon"));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(XxxPluginMoreActionsTabName, FOnSpawnTab::CreateRaw(this, &FXxxPluginEditorModule::OnSpawnPluginTabXxxPluginMoreActions))
	.SetDisplayName(LOCTEXT("XxxPluginMoreActions", "XxxPluginMoreActions"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

}

void FXxxPluginEditorModule::ShutdownModule()
{
	// Unregister Tab Spawner
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(XxxPluginTabId);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(XxxPluginMoreActionsTabName);

	// Cleanup the Editor object.
	Editor = nullptr;

	// Remove OnPostEngineInit delegate
	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);

	// Unregister UICommands.
	FXxxPluginEditorCommands::Unregister();

	UToolMenus::UnRegisterStartupCallback(OnRegisterStartupCallbackHandle);

	// Unregister Styles.
	FXxxPluginEditorStyle::Shutdown();
}

void FXxxPluginEditorModule::OnPostEngineInit()
{
	// This function is for registering UICommand to the engine, so it can be executed via keyboard shortcut.
	// This will also add this UICommand to the menu, so it can also be executed from there.
	
	// This function is valid only if no Commandlet or game is running. It also requires Slate Application to be initialized.
	if ((IsRunningCommandlet() == false) && (IsRunningGame() == false) && FSlateApplication::IsInitialized())
	{
		if (FLevelEditorModule* LevelEditor = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor")))
		{
			// Create a UICommandList and map editor spawning function to the UICommand of opening My Plugin Editor.
			TSharedPtr<FUICommandList> Commands = MakeShareable(new FUICommandList());
			Commands->MapAction(
				FXxxPluginEditorCommands::Get().OpenXxxPluginWindow,
				FExecuteAction::CreateRaw(this, &FXxxPluginEditorModule::InvokeEditorSpawn),
				FCanExecuteAction::CreateRaw(this, &FXxxPluginEditorModule::CanSpawnEditor),
				FIsActionChecked::CreateRaw(this, &FXxxPluginEditorModule::IsEditorSpawned)
			);

			// Register this UICommandList to the MainFrame.
			// Otherwise nothing will handle the input to trigger this command.
			IMainFrameModule& MainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
			MainFrame.GetMainFrameCommandBindings()->Append(Commands.ToSharedRef());

			// Create a Menu Extender, which adds a button that executes the UICommandList of opening My Plugin Window.
			TSharedPtr<FExtender> MainMenuExtender = MakeShareable(new FExtender);
			MainMenuExtender->AddMenuExtension(
#if (ENGINE_MAJOR_VERSION == 5)
				FName(TEXT("Tools")),
#else
				FName(TEXT("General")),
#endif
				EExtensionHook::After, 
				Commands,
				FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.AddMenuEntry(
						FXxxPluginEditorCommands::Get().OpenXxxPluginWindow,
						NAME_None,
						FText::FromString(TEXT("My Plugin")),
						FText::FromString(TEXT("Opens My Plugin Window")),
						FSlateIcon(FXxxPluginEditorStyle::GetStyleSetName(), "XxxPluginEditorStyle.MenuIcon")
					);
				})
			);

			// Extend Editors menu with the created Menu Extender.
			LevelEditor->GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
		}
	}
}

void FXxxPluginEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Prevent Editor Object from being garbage collected.
	if (Editor)
	{
		Collector.AddReferencedObject(Editor);
	}
}

#if (ENGINE_MAJOR_VERSION == 5)
FString FXxxPluginEditorModule::GetReferencerName() const
{
	return TEXT("XxxPluginModuleGCObject");
}
#endif

bool FXxxPluginEditorModule::CanSpawnEditor()
{
	// Editor can be spawned only when the Editor object say that UI can be created.
	if (Editor && Editor->CanCreateEditorUI())
	{
		return true;
	}
	return false;
}

TSharedRef<SDockTab> FXxxPluginEditorModule::SpawnEditor(const FSpawnTabArgs& Args)
{	
	// Spawn the Editor only when we can.
	if (CanSpawnEditor())
	{
		// Spawn new DockTab and fill it with newly created editor UI.
		TSharedRef<SDockTab> NewTab = SAssignNew(EditorTab, SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				Editor->CreateEditorUI()
			];

		// Tell the Editor Object about newly spawned DockTab, as it will 
		// need it to handle various editor actions.
		Editor->SetEditorTab(NewTab);

		// Return the DockTab to the Global Tab Manager.
		return NewTab;
	}

	// If editor can't be spawned - create an empty tab.
	return SAssignNew(EditorTab, SDockTab).TabRole(ETabRole::NomadTab);
}

bool FXxxPluginEditorModule::IsEditorSpawned()
{
	// Checks if the editor tab is already existing in the editor
	return FGlobalTabmanager::Get()->FindExistingLiveTab(XxxPluginTabId).IsValid();
}

void FXxxPluginEditorModule::InvokeEditorSpawn()
{
	// Tries to invoke opening a plugin tab
	FGlobalTabmanager::Get()->TryInvokeTab(XxxPluginTabId);
}

void FXxxPluginEditorModule::OnRegisterStartupCallback()
{
	ExtendToobarMenu();
}

void FXxxPluginEditorModule::ExtendToobarMenu()
{
	//UToolMenu* LevelEditorToolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
	UToolMenu* LevelEditorToolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& RootSection = LevelEditorToolbar->AddSection("XxxPlugin");
	RootSection.AddEntry(FToolMenuEntry::InitComboButton(
		"XxxPlugin",
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FXxxPluginEditorModule::GenerateGameToolsMenu),
		LOCTEXT("XxxPlugin", "XxxPlugin"),
		LOCTEXT("XxxPlugin", "XxxPlugin"),
		FSlateIcon(FSlateIcon(FXxxPluginEditorStyle::GetStyleSetName(), "XxxPluginEditorStyle.MenuIcon"))
	));

	
	UToolMenu* C7Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.XxxPlugin");
	FToolMenuSection& ExportSection = C7Toolbar->AddSection("ExportTools");
	
	ExportSection.AddMenuEntry(
		"TestAction1",
		LOCTEXT("TestAction1", "TestAction1"),
		LOCTEXT("TestAction1", "TestAction1"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateStatic(&FXxxPluginEditorModule::TestAction1), FCanExecuteAction())
		);
	
}

TSharedRef<SWidget> FXxxPluginEditorModule::GenerateGameToolsMenu()
{
	TSharedPtr<FUICommandList> InCommandList = nullptr;
	TSharedPtr<FExtender> MenuExtender = nullptr;

	FToolMenuContext MenuContext(InCommandList, MenuExtender);
	return UToolMenus::Get()->GenerateWidget("LevelEditor.LevelEditorToolBar.XxxPlugin", MenuContext);	
}

void FXxxPluginEditorModule::TestAction1()
{
	FGlobalTabmanager::Get()->TryInvokeTab(XxxPluginMoreActionsTabName);	
	
}

TSharedRef<SDockTab> FXxxPluginEditorModule::OnSpawnPluginTabXxxPluginMoreActions(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = LOCTEXT("WindowWidgetText", "WindowWidgetText window's contents");

	TSharedRef<SDockTab> Ret = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			// SNew(SBox)
			// .HAlign(HAlign_Center)
			// .VAlign(VAlign_Center)
			// [
			// 	SNew(STextBlock)
			// 	.Text(WidgetText)
			// ]
			SNew(SVerticalBox)
			// +SVerticalBox::Slot().AutoHeight()
			// [
			// 	SNew(SButton).Text(FText::FromString("GenVolumes"))
			// 	.OnClicked_Lambda([this]()
			// 		{
			// 			FWPEditorTools::GenVolumes();
			// 			return FReply::Handled();
			// 		})
			// ]
			+SVerticalBox::Slot().AutoHeight()
			[
				SNew(SButton).Text(FText::FromString("TestAction"))
				.OnClicked_Lambda([this]()
					{
						UE_LOG(LogTemp, Log, TEXT("TestAction!"));
						return FReply::Handled();
					})
			]
		];

	return Ret;
}

#undef LOCTEXT_NAMESPACE
