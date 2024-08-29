#include "QImpED.h"
#include "QuakeWadAssetFactory.h"
#include "QImpSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FQImpEDModule"

void FQImpEDModule::StartupModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"QImpEditorSettings",
			LOCTEXT("RuntimeSettingsName", "QImp Settings"),
			LOCTEXT("RuntimeSettingsDescription", "Configure QImp defaults"),
			GetMutableDefault<UQImpSettings>()
			);

		auto Options = GetMutableDefault<UQImpSettings>();
		
		if (!Options->bIsInitialized) {
			Options->MapAssetOptions.BaseMaterial = static_cast<UMaterial*>(
				StaticLoadObject(UMaterial::StaticClass(),
					nullptr,
					TEXT("/QImp/Materials/M_WadMaster"),
					nullptr,
					LOAD_None,
					nullptr));
			Options->MapAssetOptions.EntityClassOverrides = static_cast<UQEntityClassesData*>(
				StaticLoadObject(UQEntityClassesData::StaticClass(),
					nullptr,
					TEXT("/QImp/DefaultEntityClasses"),
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

void FQImpEDModule::ShutdownModule()
{
	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "QImpEditorSettings");
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FQImpEDModule, QImpED)