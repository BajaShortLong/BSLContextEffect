#pragma once

#include "Factories/Factory.h"

#include "Factory_BSLContextEffectsLibrary.generated.h"

class FFeedbackContext;
class UClass;
class UObject;

UCLASS(HideCategories=Object, MinimalAPI)
class UFactory_BSLContextEffectsLibrary : public UFactory
{

	GENERATED_UCLASS_BODY()

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override {	return true; }
	
};
