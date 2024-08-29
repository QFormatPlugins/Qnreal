#pragma once

#include "CoreMinimal.h"
#include "QuakeMapAssetFactory.h"
#include "QuakeWadAssetFactory.h"

#include "Modules/ModuleManager.h"

class FQImpEDModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
private:
    TSharedPtr<FQMapAssetTypeAction> FQMapAssetTypeActions;
    TSharedPtr<FQWadAssetTypeAction> FQWadAssetTypeActions;
};
