#include "QnrealED.h"
#include "QuakeWadAssetFactory.h"
#include "QnrealSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FQnrealEDModule"

void FQnrealEDModule::StartupModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"QnrealEditorSettings",
			LOCTEXT("RuntimeSettingsName", "Qnreal Settings"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Qnreal defaults"),
			GetMutableDefault<UQnrealSettings>()
			);

		auto Options = GetMutableDefault<UQnrealSettings>();
		
		if (!Options->bIsInitialized) {
			Options->MapAssetOptions.BaseMaterial = static_cast<UMaterial*>(
				StaticLoadObject(UMaterial::StaticClass(),
					nullptr,
					TEXT("/Qnreal/Materials/M_WadMaster"),
					nullptr,
					LOAD_None,
					nullptr));
			Options->MapAssetOptions.EntityClassOverrides = static_cast<UQEntityClassesData*>(
				StaticLoadObject(UQEntityClassesData::StaticClass(),
					nullptr,
					TEXT("/Qnreal/DefaultEntityClasses"),
					nullptr,
					LOAD_None,
					nullptr));
			Options->bIsInitialized = true;
		}
		
	}
	
	FQMapAssetTypeActions = MakeShared<FQMapAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQMapAssetTypeActions.ToSharedRef());

	FQWadAssetTypeActions = MakeShared<FQWadAssetTypeAction>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(FQWadAssetTypeActions.ToSharedRef());

}

void FQnrealEDModule::ShutdownModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "QnrealEditorSettings");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FQnrealEDModule, QnrealED)