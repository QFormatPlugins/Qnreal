#include "Assets/QuakeWadAsset.h"


UQuakeWadAsset::UQuakeWadAsset() {}

UQuakeWadAsset::~UQuakeWadAsset()
{
	//object->MarkPendingKill(); <-- this way we can kill stuff
	FWadManager::GetInstance()->RemoveWad(this);
}

void UQuakeWadAsset::LoadWadFromFile(FString FileName)
{
	this->SourceQWadFile = FileName;
	Textures.Empty();
	
	auto WadBaseFileName = FPaths::GetBaseFilename(FileName);
	WadFile = qformats::wad::QuakeWad::FromFile(std::string(TCHAR_TO_UTF8(*FileName)));
	
	for(const auto &Tex : WadFile->Textures())
	{
		if (Tex.second.header.type != qformats::wad::QuakeWadEntry::QuakeWadEntryType::QWET_MipsTexture)
			continue;
		
		const auto qtex = WadFile->GetTexture(Tex.first);
		auto Utex = FWadTexture2D();
		Utex.WadName = WadBaseFileName;
		Utex.SetName(FString(Tex.first.c_str()));
		auto PackagePath = GetPackage()->GetPathName();
		if (bExternalTextures)
		{
			PackagePath = FPaths::GetPath(this->GetPathName());
			FString BaseName = FPaths::GetBaseFilename(this->GetPathName());
			PackagePath = FPaths::Combine(PackagePath, WadBaseFileName + "_Textures", Utex.CleanName);
		}
		Utex.Generate(PackagePath, qtex);
		Textures.Emplace(Utex);
	}
}

FWadManager* FWadManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new FWadManager();
	}
	return Instance;
}

void FWadManager::Refresh(const FAssetRegistryModule &AssetRegistryModule)
{
	TArray<FAssetData> ObjectList;
	AssetRegistryModule.Get().GetAssetsByPaths({"/Game"}, ObjectList, true);
	for (const auto& Obj : ObjectList)
	{
		if (Obj.GetClass() == UQuakeWadAsset::StaticClass())
		{
			auto w = Cast<UQuakeWadAsset>(Obj.GetAsset());
			AddWadAsset(w);
		}
	}
}


FWadManager *FWadManager::Instance = nullptr;

void FWadManager::AddWadAsset(UQuakeWadAsset* Wa)
{
	if (Wa == nullptr || Wa->bAlreadyInManager) return;
	Wa->bAlreadyInManager = true;
	WadAssets.push_back(Wa);
}

void FWadManager::RemoveWad(UQuakeWadAsset* Wa)
{
	if (Wa == nullptr)
	{
		return;
	}
	if (const auto Elem = std::ranges::find(WadAssets, Wa); Elem != WadAssets.end())
	{
		WadAssets.erase(Elem);
	}
}

UTexture2D* FWadManager::FindTexture(const FName Name)
{
	if (TextureCache.contains(Name.ToString()))
	{
		return TextureCache[Name.ToString()];
	}

	for (const auto Wad : WadAssets)
	{
		for (const auto WadTex : Wad->Textures)
		{
			if (WadTex.OriginalName == Name && WadTex.Texture != nullptr && WadTex.Texture->IsValidLowLevelFast())
			{
				TextureCache[Name.ToString()] = WadTex.Texture;
				return WadTex.Texture;
			}
		}
	}
	return nullptr;
}

