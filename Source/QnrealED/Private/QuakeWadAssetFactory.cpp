#include "QuakeWadAssetFactory.h"

#include "FileHelpers.h"
#include "Assets/QuakeWadAsset.h"

UClass* FQWadAssetTypeAction::GetSupportedClass() const
{
	return UQuakeWadAsset::StaticClass();
}
 
FText FQWadAssetTypeAction::GetName() const
{
	return INVTEXT("Quake WAD");
}
 
FColor FQWadAssetTypeAction::GetTypeColor() const
{
	return FColor::Purple;
}
 
uint32 FQWadAssetTypeAction::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

UQuakeWadAssetFactory::UQuakeWadAssetFactory()
{
	Formats.Add("wad;Quake WAD FIle");
	SupportedClass = UQuakeWadAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UQuakeWadAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UQuakeWadAsset* newWadObj = NewObject<UQuakeWadAsset>(InParent, InClass, InName, Flags);
	newWadObj->LoadWadFromFile(Filename);
	UEditorLoadingAndSavingUtils::SavePackages({newWadObj->GetPackage()}, false);
	return newWadObj;
}

bool UQuakeWadAssetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	return Obj->GetClass() == UQuakeWadAsset::StaticClass();
}

void UQuakeWadAssetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto ReimportMap = Cast<UQuakeWadAsset>(Obj);
	if (ReimportMap != nullptr)
	{
		ReimportMap->SourceQWadFile = NewReimportPaths[0];
		ReimportMap->Modify();
	}
}

EReimportResult::Type UQuakeWadAssetFactory::Reimport(UObject* Obj)
{
	UQuakeWadAsset* newWadObj = Cast<UQuakeWadAsset>(Obj);
	newWadObj->LoadWadFromFile(newWadObj->SourceQWadFile);
	UEditorLoadingAndSavingUtils::SavePackages({newWadObj->GetPackage()}, false);
	return EReimportResult::Succeeded;
}
