#pragma once

#include <CoreMinimal.h>
#include "AssetTypeActions_Base.h"
#include "EditorReimportHandler.h"
#include "QuakeWadAssetFactory.generated.h"

class QNREALED_API FQWadAssetTypeAction final : public FAssetTypeActions_Base
{
public:
	virtual UClass *GetSupportedClass() const override;
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
};

UCLASS()
class QNREALED_API UQuakeWadAssetFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()
public:
	UQuakeWadAssetFactory();
	virtual UObject *FactoryCreateFile(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, const FString &Filename, const TCHAR *Parms, FFeedbackContext *Warn, bool &bOutOperationCanceled) override;
	virtual bool CanReimport(UObject *Obj, TArray<FString> &OutFilenames) override;
	virtual void SetReimportPaths(UObject *Obj, const TArray<FString> &NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject *Obj) override;

private:
	bool bIsReimport = false;
};
