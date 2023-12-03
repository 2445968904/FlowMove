// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWPowerfulToolsFuncLib.h"

#include "GBWToolsComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

UGBWToolsComponent* UGBWPowerfulToolsFuncLib::GetGBWtToolsComponent(AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}
	UGBWToolsComponent* AComp = nullptr;
	if (UActorComponent* Component = InActor->GetComponentByClass(UGBWToolsComponent::StaticClass()))
	{
		AComp = Cast<UGBWToolsComponent>(Component);
	}

	if (!AComp && InActor->HasAuthority())
	{
		AComp = Cast<UGBWToolsComponent>(
			InActor->AddComponentByClass(UGBWToolsComponent::StaticClass(),
				false,
				FTransform(),
				false));
		AComp->SetIsReplicated(true);
	}

	return AComp;
}

void UGBWPowerfulToolsFuncLib::execSetGBWProperty(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_OBJECT(AActor,Z_Param_InActor);
	P_GET_PROPERTY(FStrProperty,Z_Param_Key);
	
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const void* StructPtr = Stack.MostRecentPropertyAddress;
	FProperty* StructProperty = Stack.MostRecentProperty;
	
	P_FINISH;
	P_NATIVE_BEGIN;
	if (UGBWToolsComponent* TComp = UGBWPowerfulToolsFuncLib::GetGBWtToolsComponent(Z_Param_InActor))
	{
		const FString STStr = UGBWPowerfulToolsFuncLib::StructToText_Imp(StructPtr, StructProperty);
		TComp->AddProperty(Z_Param_Key,STStr);
	}
	P_NATIVE_END;
}

void UGBWPowerfulToolsFuncLib::execGetGBWPropertyAsStruct(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_OBJECT(AActor,Z_Param_InActor);
	P_GET_PROPERTY(FStrProperty,Z_Param_Key);
	
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	if (UGBWToolsComponent* TComp = UGBWPowerfulToolsFuncLib::GetGBWtToolsComponent(Z_Param_InActor))
	{
		FString Value;
		if (TComp->GetProperty(Z_Param_Key, Value))
		{
			*static_cast<bool*>(Z_Param__Result) = UGBWPowerfulToolsFuncLib::TextToStruct_Imp(Value, StructPtr, Stack.MostRecentProperty);
		}
		else
		{
			*static_cast<bool*>(Z_Param__Result) = false;
		}
	}
	else
	{
		*static_cast<bool*>(Z_Param__Result) = false;
	}
}

void UGBWPowerfulToolsFuncLib::GetPlayerCamera(
	AActor* InActor,
	bool& bIsGet,
	UCameraComponent*& Camera,
	USpringArmComponent*& SpringArm)
{
	if (!InActor)
	{
		bIsGet = false;
		return;
	}
	TArray<UActorComponent*> OutComponents;
	InActor->GetComponents(UCameraComponent::StaticClass(),OutComponents);
	for (const auto Component : OutComponents)
	{
		if (Component->IsActive())
		{
			Camera = Cast<UCameraComponent>(Component);
			bIsGet = true;
			break;
		}
	}

	InActor->GetComponents(USpringArmComponent::StaticClass(),OutComponents);
	for (const auto Component : OutComponents)
	{
		if (Component->IsActive())
		{
			SpringArm = Cast<USpringArmComponent>(Component);
			bIsGet = true;
			break;
		}
	}
}

void UGBWPowerfulToolsFuncLib::GetPlayerController(AActor* InActor, bool& bIsGet, bool& bIsLocalController,
	APlayerController*& PlayerController)
{
	bIsGet = false;
	bIsLocalController = false;
	PlayerController = nullptr;
	
	if (!InActor)
	{
		return;
	}

	if (const APawn* Pawn = Cast<APawn>(InActor))
	{
		PlayerController = Cast<APlayerController>(Pawn->GetController());
	}
	
	if (!PlayerController)
	{
		if (const APlayerState* PlayerState = Cast<APlayerState>(InActor))
		{
			PlayerController = PlayerState->GetPlayerController();
		}
	}

	if (!PlayerController)
	{
		PlayerController = Cast<APlayerController>(InActor);
	}

	if (PlayerController)
	{
		bIsGet = true;
		bIsLocalController = PlayerController->IsLocalController();
	}
}

EGBWActorNetType UGBWPowerfulToolsFuncLib::GetActorNetType(AActor* InActor)
{
	if (!InActor)
	{
		return EGBWActorNetType::None;
	}

	const ENetRole LocalRole = InActor->GetLocalRole();
	const ENetRole RemoteRole = InActor->GetRemoteRole();
	const APawn* PlayerActor = UGameplayStatics::GetPlayerPawn(InActor, 0);
	if (!(PlayerActor && PlayerActor->GetController() && PlayerActor->GetController()->IsLocalController()))
	{
		PlayerActor = nullptr;
	}
	
	if (LocalRole == ENetRole::ROLE_Authority)
	{
		if (PlayerActor == InActor)
		{
			return EGBWActorNetType::InServer_LocalPlayer;
		}

		if (RemoteRole == ENetRole::ROLE_AutonomousProxy)
		{
			return EGBWActorNetType::InServer_RemotePlayer;
		}
		
		return EGBWActorNetType::InServer_NotPlayer;
	}

	if (LocalRole != ENetRole::ROLE_None)
	{
		if (PlayerActor == InActor)
		{
			return EGBWActorNetType::InClient_LocalPlayer;
		}

		if (RemoteRole == ENetRole::ROLE_AutonomousProxy)
		{
			return EGBWActorNetType::InClient_RemotePlayer;
		}

		return EGBWActorNetType::InClient_NotPlayer;
	}

	return EGBWActorNetType::None;
}

void UGBWPowerfulToolsFuncLib::GetAllPropertiesStr(UObject* InObject, TArray<FString>& PropertyNames, TArray<FString>& PropertyValues)
{
	PropertyNames = {};
	PropertyValues = {};
	if (!InObject)
	{
		return;
	}
	for(const FProperty* Property = InObject->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FString ValueStr = "";
		Property->ExportTextItem(
			ValueStr,
			Property->ContainerPtrToValuePtr<void*>(InObject),
			nullptr,
			InObject,
			PPF_None,
			nullptr);
		PropertyNames.Add(Property->GetName());
		PropertyValues.Add(ValueStr);
	}
}

FString UGBWPowerfulToolsFuncLib::StructToText_Imp(const void* StructPtr, FProperty* Property)
{
	FString STStr;
	const FStructProperty* StructProp = CastField<FStructProperty>(Property);
	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (StructPtr != nullptr)))
	{
		FString Name;
		StructProp->Struct->GetName(Name);
		STStr = Name + "<:>";
		StructProp->Struct->ExportText(STStr,StructPtr,nullptr,nullptr,PPF_None,nullptr);
	}

	return STStr;
}

void UGBWPowerfulToolsFuncLib::execStructToText(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	const void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	*static_cast<FString*>(Z_Param__Result) = UGBWPowerfulToolsFuncLib::StructToText_Imp(StructPtr, Stack.MostRecentProperty);
}

bool UGBWPowerfulToolsFuncLib::TextToStruct_Imp(const FString& InText, void* StructPtr, FProperty* Property)
{
	const FStructProperty* StructProp = CastField<FStructProperty>(Property);
	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (StructPtr != nullptr)))
	{
		FString SourceName;
		FString SourceText;
		InText.Split("<:>",&SourceName,&SourceText);
		FString TargetName;
		StructProp->Struct->GetName(TargetName);
			
		if (TargetName != SourceName)
		{
			return false;
		}
		if (nullptr != StructProp->Struct->ImportText(*SourceText, StructPtr, nullptr, PPF_None, nullptr, FString("")))
		{
			return true;
		}
	}
	return false;
}

void UGBWPowerfulToolsFuncLib::execTextToStruct(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_PROPERTY(FStrProperty,InText);
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.MostRecentProperty = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;
	*static_cast<bool*>(Z_Param__Result) = TextToStruct_Imp(InText, StructPtr, Stack.MostRecentProperty);
}
