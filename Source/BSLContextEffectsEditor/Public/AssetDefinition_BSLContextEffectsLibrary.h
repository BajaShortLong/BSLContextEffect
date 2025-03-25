// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetDefinitionDefault.h"
#include "BSLContextEffectsLibrary.h"

#include "AssetDefinition_BSLContextEffectsLibrary.generated.h"

UCLASS()
class UAssetDefinition_BSLContextEffectsLibrary : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	
	virtual FText GetAssetDisplayName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_BSLContextEffectsLibrary", "BSLContextEffectsLibrary"); }
	virtual FLinearColor GetAssetColor() const override { return FLinearColor(FColor(192, 98, 255)); }
	virtual TSoftClassPtr<UObject> GetAssetClass() const override { return UBSLContextEffectsLibrary::StaticClass(); }
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
	{
		static const auto Categories = { FAssetCategoryPath(FText::FromString(TEXT("ContextEffects"))) };
		return Categories;
	}
};
