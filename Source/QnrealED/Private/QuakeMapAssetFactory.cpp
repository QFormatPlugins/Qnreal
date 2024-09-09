#include "QuakeMapAssetFactory.h"

#include "Assets/QuakeMapAsset.h"
#include "EditorFramework/AssetImportData.h"

UClass* FQMapAssetTypeAction::GetSupportedClass() const
{
	return UQuakeMapAsset::StaticClass();
}
 
FText FQMapAssetTypeAction::GetName() const
{
	return INVTEXT("Quake Map");
}
 
FColor FQMapAssetTypeAction::GetTypeColor() const
{
	return FColor::Red;
}
 
uint32 FQMapAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

bool FQMapAssetTypeAction::IsImportedAsset() const
{
	return true;
}

void FQMapAssetTypeAction::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets,
	TArray<FString>& OutSourceFilePaths) const
{
		FAssetTypeActions_Base::GetResolvedSourceFilePaths(TypeAssets, OutSourceFilePaths);
    
    	for (auto& Asset : TypeAssets)
    	{
    		const auto QuakeMap = CastChecked<UQuakeMapAsset>(Asset);
    		if (QuakeMap->AssetImportData)
    		{
    			QuakeMap->AssetImportData->ExtractFilenames(OutSourceFilePaths);
    		}
    	}
}

UQuakeMapAssetFactory::UQuakeMapAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Formats.Add("map;Quake Map FIle");
	SupportedClass = UQuakeMapAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bAutomatedReimport = true;
	bEditAfterNew = true;
	ImportPriority = 1;
}

UObject* UQuakeMapAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQuakeMapAsset* newMapObj = NewObject<UQuakeMapAsset>(InParent, InClass, InName, Flags);
	newMapObj->SourceQMapFile = Filename;
	
	if (!bIsReimport)
	{
		newMapObj->LoadMapFromFile(Filename);
	}
	newMapObj->PostEditChange();
	
	newMapObj->AssetImportData->Update(Filename);
				
	GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetPostImport.Broadcast(this, newMapObj);
	if (newMapObj->AssetImportData != nullptr)
	{
		newMapObj->AssetImportData->Update(GetCurrentFilename());
	}
	return newMapObj;
}

bool UQuakeMapAssetFactory::ConfigureProperties()
{
	return Super::ConfigureProperties();
}

bool UQuakeMapAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (auto LensFile = Cast<UQuakeMapAsset>(Obj))
	{
		if (LensFile->AssetImportData != nullptr)
		{
			LensFile->AssetImportData->ExtractFilenames(OutFilenames);
		}
		else
		{
			OutFilenames.Add(FString{});
		}

		return true;
	}
	
	return false;
}

void UQuakeMapAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto ReimportMap = Cast<UQuakeMapAsset>(Obj);
	if (ReimportMap != nullptr)
	{
		ReimportMap->SourceQMapFile = NewReimportPaths[0];
		ReimportMap->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
		
		ReimportMap->Modify();
	}
}

int32 UQuakeMapAssetFactory::GetPriority() const
{
	return ImportPriority;
}

EReimportResult::Type UQuakeMapAssetFactory::Reimport(UObject* Obj)
{
	auto ReimportMap = Cast<UQuakeMapAsset>(Obj);
	const FString Filename = ReimportMap->AssetImportData->GetFirstFilename();
	const FString FileExtension = FPaths::GetExtension(Filename);
	bIsReimport = true;
	auto TempOptions = ReimportMap->Options;
	bool bOverrideOptions = ReimportMap->bOverrideDefaultOptions;
	FSimpleMulticastDelegate TmpMapUpdateDelegate = ReimportMap->MapData->QuakeMapUpdated;
	if( StaticImportObject( ReimportMap->GetClass(), ReimportMap->GetOuter(), *ReimportMap->GetName(), RF_Public|RF_Standalone, *Filename, ReimportMap, this ) )
	{
		bIsReimport = false;
		ReimportMap->bOverrideDefaultOptions = bOverrideOptions;
		ReimportMap->Options = TempOptions;
		ReimportMap->MapData->QuakeMapUpdated = TmpMapUpdateDelegate;
		ReimportMap->LoadMapFromFile(Filename);
		ReimportMap->MarkPackageDirty();
		ReimportMap->MapData->QuakeMapUpdated.Broadcast();
		auto sourceFiles = TArray<FAssetImportInfo::FSourceFile>();
		return EReimportResult::Succeeded;
	}
	
	return EReimportResult::Failed;
}
