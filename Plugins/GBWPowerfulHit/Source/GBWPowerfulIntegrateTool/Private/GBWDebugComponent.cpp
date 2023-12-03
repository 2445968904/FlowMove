// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWDebugComponent.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UGBWDebugComponent::UGBWDebugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGBWDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGBWDebugComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


// Called every frame
void UGBWDebugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DebugTick();
}

UGBWDebugComponent* UGBWDebugComponent::GetGbwDebugComponent(AActor* InActor)
{
	if (InActor)
	{
		TArray<UActorComponent*> Comps;
		InActor->GetComponents(UGBWDebugComponent::StaticClass(),Comps);

		if (Comps.IsEmpty())
		{
			return Cast<UGBWDebugComponent>(
				InActor->AddComponentByClass(
					UGBWDebugComponent::StaticClass(),
					false,
					FTransform(),
					false));
		}
			
		return Cast<UGBWDebugComponent>(Comps[0]);
	}

	return nullptr;
}

void UGBWDebugComponent::GBWDebug_CounterPrint(AActor* InActor, FString Key)
{
	if (UGBWDebugComponent* DebugComp = UGBWDebugComponent::GetGbwDebugComponent(InActor))
	{
		DebugComp->CounterPrint(Key);
	}
}

void UGBWDebugComponent::CounterPrint(FString Key)
{
	if (CounterPrintCache.Find(Key) != nullptr)
	{
		CounterPrintCache.Add(Key, CounterPrintCache.FindRef(Key)+1);
	}
	else
	{
		CounterPrintCache.Add(Key, 1);
	}
}

void UGBWDebugComponent::DebugTick()
{
	if (bIsDebug)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(GetOwner(), EGetWorldErrorMode::ReturnNull);
		FString Prefix;
		if (World)
		{
			if (World->WorldType == EWorldType::PIE)
			{
				switch(World->GetNetMode())
				{
				case NM_Client:
					Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID);
					break;
				case NM_DedicatedServer:
				case NM_ListenServer:
					Prefix = FString::Printf(TEXT("Server: "));
					break;
				case NM_Standalone:
					break;
				default: ;
				}
			}
		}
		
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			">>>>>>>>>>>>GBW Debug End<<<<<<<<<<<<<<<\n",
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "GBW Debug"));
		int Index = 0;
		for (auto CP:CounterPrintCache)
		{
			UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			FString::Printf(TEXT("%s : %d"), *CP.Key, CP.Value),
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "GBW Debug Content" + FString::Printf(TEXT("%d"), Index)));
			Index = Index+1;
		}
		UKismetSystemLibrary::PrintString(GetOwner()->GetWorld(),
			">>>>>>>>>>>>GBW Debug Start<<<<<<<<<<<<<<<",
			bPrintToScreen,bPrintToLog,DebugTextColor,0.0f,FName(Prefix + "GBW Debug" + FString::Printf(TEXT("%d"), 1)));
	}
}

