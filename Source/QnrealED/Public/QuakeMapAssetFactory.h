#pragma once

#include <CoreMinimal.h>

#include "AssetTypeActions_Base.h"
#include "EditorReimportHandler.h"

#include <QuakeMapAssetFactory.generated.h>

class QNREALED_API FQMapAssetTypeAction final : public FAssetTypeActions_Base
{
public:
	virtual UClass *GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual bool IsImportedAsset() const override;
	virtual void GetResolvedSourceFilePaths(const TArray<UObject *> &TypeAssets, TArray<FString> &OutSourceFilePaths) const override;
};

UCLASS()
class QNREALED_API UQuakeMapAssetFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()
public:
	UQuakeMapAssetFactory(const FObjectInitializer &ObjectInitializer);
	virtual UObject *FactoryCreateFile(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, const FString &Filename, const TCHAR *Parms, FFeedbackContext *Warn, bool &bOutOperationCanceled) override;
	virtual bool ConfigureProperties() override;
	virtual int32 GetPriority() const override;
	virtual bool CanReimport(UObject *Obj, TArray<FString> &OutFilenames) override;
	virtual void SetReimportPaths(UObject *Obj, const TArray<FString> &NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject *Obj) override;

private:
	bool bIsReimport = false;
};
