#include "Factory_BSLContextEffectsLibrary.h"

#include "BSLContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Factory_BSLContextEffectsLibrary)

class FFeedbackContext;
class UClass;
class UObject;

UFactory_BSLContextEffectsLibrary::UFactory_BSLContextEffectsLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UBSLContextEffectsLibrary::StaticClass();
	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UFactory_BSLContextEffectsLibrary::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UBSLContextEffectsLibrary* BSLContextEffectsLibrary = NewObject<UBSLContextEffectsLibrary>(InParent, InName, Flags);
	return BSLContextEffectsLibrary;
}
