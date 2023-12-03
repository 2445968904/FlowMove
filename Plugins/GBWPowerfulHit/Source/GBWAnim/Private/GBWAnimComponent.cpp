// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWAnimComponent.h"

#include "GBWAnimFuncLib.h"
#include "GBWPowerfulToolsFuncLib.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimSequence.h"

void FGBWAnimPlayedCache::AddAnimSequenceCache(UAnimSequence* InAnimSequence)
{
	if (InAnimSequence)
	{
		AnimSequencePlayedCacheIndexNow++;
		if (AnimSequencePlayedCacheIndexNow>=MaxAnimSequencePlayedCacheCount)
		{
			AnimSequencePlayedCacheIndexNow = 0;
		}

		if (AnimSequencePlayed.Num() > AnimSequencePlayedCacheIndexNow)
		{
			AnimSequencePlayed[AnimSequencePlayedCacheIndexNow] = InAnimSequence;
		}
		else
		{
			AnimSequencePlayed.Add(InAnimSequence);
		}
	}
}

bool FGBWAnimPlayedCache::GetLastAnimSequenceFromArr(TArray<UAnimSequence*> AnimArr, UAnimSequence*& LastAnim,
	int& LastAnimIndex)
{
	int Count = 0;
	LastAnimIndex = -1;
	for (int i = AnimSequencePlayedCacheIndexNow; Count < AnimSequencePlayed.Num(); i--)
	{
		if (i<0)
		{
			i = AnimSequencePlayed.Num() - 1;
		}
		Count++;
		if (AnimArr.Find(AnimSequencePlayed[i],LastAnimIndex))
		{
			LastAnim = AnimSequencePlayed[i];
			return true;
		}
	}

	return false;
}

UAnimSequence* FGBWAnimPlayedCache::GetAnimSequenceNow()
{
	if (AnimSequencePlayedCacheIndexNow>=0)
	{
		return AnimSequencePlayed[AnimSequencePlayedCacheIndexNow];
	}
	return nullptr;
}

UAnimSequence* FGBWAnimPlayedCache::GetPreviousAnimSequence(int Offset)
{
	if (AnimSequencePlayedCacheIndexNow>=0)
	{
		if (AnimSequencePlayedCacheIndexNow-Offset>=0)
		{
			return AnimSequencePlayed[AnimSequencePlayedCacheIndexNow-Offset];
		}
		else
		{
			return AnimSequencePlayed.Last(Offset-AnimSequencePlayedCacheIndexNow);
		}
	}
	return nullptr;
}

FString FGBWAnimPlayedCache::GetAnimSequencePlayedCacheDebugString(int CacheCount)
{
	FString Res = "";
	int Count = 0;
	for (int i = AnimSequencePlayedCacheIndexNow; Count < AnimSequencePlayed.Num() && Count < CacheCount; i--)
	{
		if (i<0)
		{
			i = AnimSequencePlayed.Num() - 1;
		}
		Res = Res + "\n" + (AnimSequencePlayed[i]? AnimSequencePlayed[i]->GetName() : "None");
		Count++;
	}

	return Res;
}

// Sets default values for this component's properties
UGBWAnimComponent::UGBWAnimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGBWAnimComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	Parameters.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AnimPlayState, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CharacterOwner, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AnimStateGetType, Parameters)
}

// Called when the game starts
void UGBWAnimComponent::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterOwner();
}

// Called every frame
void UGBWAnimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!IsReadyToUse())
	{
		TickCount++;
	}
	UpdateAnimPlayState(DeltaTime);
}

void UGBWAnimComponent::UpdateAnimPlayState(float DeltaTime)
{
	if (IsInServer())
	{
		FGBWAnimPlayStateSet AnimPlayStateSet = FGBWAnimPlayStateSet();
		
		if (UGBWAnimFuncLib::GetAnimPlayState(
			GetCharacterOwner()->GetMesh()->GetAnimInstance(),
			AnimStateGetType,
			DeltaTime,
			AnimPlayState,
			AnimPlayStateSet
			))
		{
			FGBWAnimPlayedCache AnimPlayedCache = AnimPlayState.AnimPlayedCache;
			if (AnimPlayStateSet.MainAnimPlayState.AssetType == EGBWAnimAssetType::AnimSequence
				&& AnimPlayStateSet.MainAnimPlayState.AnimAsset != AnimPlayState.MainAnimPlayState.AnimAsset
				&& Cast<UAnimSequence>(AnimPlayStateSet.MainAnimPlayState.AnimAsset))
			{
				AnimPlayedCache.AddAnimSequenceCache(Cast<UAnimSequence>(AnimPlayStateSet.MainAnimPlayState.AnimAsset));
			}
			AnimPlayStateSet.AnimPlayedCache = AnimPlayedCache;
			AnimPlayStateSet.FrameCount = GFrameCounter;
			SetAnimPlayState_Server(AnimPlayStateSet);
		}
	}
	else
	{
		FGBWAnimPlayStateSet AnimPlayStateSet_Local = FGBWAnimPlayStateSet();
		
		if (UGBWAnimFuncLib::GetAnimPlayState(
			GetCharacterOwner()->GetMesh()->GetAnimInstance(),
			AnimStateGetType,
			DeltaTime,
			AnimPlayState_Local,
			AnimPlayStateSet_Local
			))
		{
			AnimPlayState_Local = AnimPlayStateSet_Local;
			AnimPlayState_Local.FrameCount = GFrameCounter;
		}
	}

	/*GEngine->AddOnScreenDebugMessage(-1,5,
		IsInServer()? FColor::Green : FColor::Red,
		FString::Printf(
		TEXT("%d %s %f  ----   %d %s %f")
		,AnimPlayState.FrameCount
		,*(AnimPlayState.MainAnimPlayState.AnimAsset? AnimPlayState.MainAnimPlayState.AnimAsset->GetName() : "None")
		,AnimPlayState.MainAnimPlayState.AnimPlayDuration
		,AnimPlayState.FrameCount
		,*(AnimPlayState_Local.MainAnimPlayState.AnimAsset? AnimPlayState_Local.MainAnimPlayState.AnimAsset->GetName() : "None")
		,AnimPlayState_Local.MainAnimPlayState.AnimPlayDuration
		));*/
}

ACharacter* UGBWAnimComponent::GetCharacterOwner()
{
	if (!CharacterOwner)
	{
		CharacterOwner = Cast<ACharacter>(this->GetOwner());
	}

	return CharacterOwner;
}

bool UGBWAnimComponent::IsLocalOwn() const
{
	bool bRes = false;
	
	if (this->GetOwner())
	{
		const EGBWActorNetType ActorNetType = UGBWPowerfulToolsFuncLib::GetActorNetType(this->GetOwner());
		bRes = ActorNetType == EGBWActorNetType::InClient_LocalPlayer || ActorNetType == EGBWActorNetType::InServer_LocalPlayer;
	}
	
	return bRes;
}

bool UGBWAnimComponent::IsInServer()
{
	if (GetCharacterOwner() && GetCharacterOwner()->HasAuthority())
	{
		return true;
	}
	return false;
}

bool UGBWAnimComponent::IsReadyToUse()
{
	return TickCount >= 10;
}

void UGBWAnimComponent::SetAnimPlayState_Server_Implementation(FGBWAnimPlayStateSet NewAnimPlayState)
{
	AnimPlayState = NewAnimPlayState;
	/*if (AnimPlayState.FrameCount % 60 == 0)
	{
		SetAnimPlayState_Multicast(NewAnimPlayState);
	}*/
}

void UGBWAnimComponent::SetAnimPlayState_Multicast_Implementation(FGBWAnimPlayStateSet NewAnimPlayState)
{
	AnimPlayState = NewAnimPlayState;
}

bool UGBWAnimComponent::GetAnimPlayState(FGBWAnimPlayStateSet& AnimPlayStateSet)
{
	if (GetCharacterOwner())
	{
		AnimPlayStateSet = AnimPlayState;
		return true;
	}

	return false;
}

bool UGBWAnimComponent::GetAnimPlayState_Local(FGBWAnimPlayStateSet& AnimPlayStateSet)
{
	if (GetCharacterOwner())
	{
		if (IsInServer())
		{
			AnimPlayStateSet = AnimPlayState;
		}
		else
		{
			AnimPlayStateSet = AnimPlayState_Local;
		}
		return true;
	}

	return false;
}

void UGBWAnimComponent::SetIncludeMainInstance(bool bInclude)
{
	if (IsLocalOwn() || IsInServer())
	{
		SetIncludeMainInstance_Server(bInclude);
	}
}
void UGBWAnimComponent::SetIncludeMainInstance_Server_Implementation(bool bInclude)
{
	AnimStateGetType.IncludeMainAnimInstance = bInclude;	
}

void UGBWAnimComponent::SetIncludeLinkedInstance(bool bInclude)
{
	if (IsLocalOwn() || IsInServer())
	{
		SetIncludeLinkedInstance_Server(bInclude);
	}
}
void UGBWAnimComponent::SetIncludeLinkedInstance_Server_Implementation(bool bInclude)
{
	AnimStateGetType.IncludeLinkedAnimInstance = bInclude;	
}

void UGBWAnimComponent::AddIncludeLinkedInstanceClass(
	TArray<TSubclassOf<UAnimInstance>> IncludeLinkedAnimInstanceClassSet)
{
	if (IsLocalOwn() || IsInServer())
	{
		AddIncludeLinkedInstanceClass_Server(IncludeLinkedAnimInstanceClassSet);
	}
}
void UGBWAnimComponent::AddIncludeLinkedInstanceClass_Server_Implementation(
	const TArray<TSubclassOf<UAnimInstance>>& IncludeLinkedAnimInstanceClassSet)
{
	for (auto Element : IncludeLinkedAnimInstanceClassSet)
	{
		AnimStateGetType.IncludeLinkedAnimInstanceClassSet.AddUnique(Element);
	}
}

void UGBWAnimComponent::PlayMontage(USkeletalMeshComponent* SKMComponent, UAnimMontage* MontageToPlay, float PlayRate,
                                    float StartingPosition, FName StartingSection)
{
	if (IsLocalOwn() || IsInServer())
	{
		PlayMontage_Server(SKMComponent,MontageToPlay,PlayRate,StartingPosition,StartingSection);
	}
}
void UGBWAnimComponent::PlayMontage_Server_Implementation(USkeletalMeshComponent* SKMComponent,
                                                          UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition, FName StartingSection)
{
	PlayMontage_Multicast(SKMComponent,MontageToPlay,PlayRate,StartingPosition,StartingSection);
}
void UGBWAnimComponent::PlayMontage_Multicast_Implementation(USkeletalMeshComponent* SKMComponent,
                                                             UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition, FName StartingSection)
{
	PlayMontage_Imp(SKMComponent,MontageToPlay,PlayRate,StartingPosition,StartingSection);
}
void UGBWAnimComponent::PlayMontage_Imp(const USkeletalMeshComponent* SKMComponent, UAnimMontage* MontageToPlay,
                                        float PlayRate, float StartingPosition, FName StartingSection)
{
	if (SKMComponent && SKMComponent->GetAnimInstance())
	{
		SKMComponent->GetAnimInstance()->Montage_Play(MontageToPlay,
			PlayRate,
			EMontagePlayReturnType::MontageLength,
			StartingPosition,
			true);
		SKMComponent->GetAnimInstance()->Montage_JumpToSection(StartingSection,MontageToPlay);
		if (IsInServer())
		{
			AnimPlayState.AnimPlayedCache.LastMontage = MontageToPlay;
		}
	}
}

void UGBWAnimComponent::StopMontage(USkeletalMeshComponent* SKMComponent)
{
	if (IsLocalOwn() || IsInServer())
	{
		StopMontage_Server(SKMComponent);
	}
}
void UGBWAnimComponent::StopMontage_Server_Implementation(USkeletalMeshComponent* SKMComponent)
{
	StopMontage_Multicast(SKMComponent);
}
void UGBWAnimComponent::StopMontage_Multicast_Implementation(USkeletalMeshComponent* SKMComponent)
{
	StopMontage_Imp(SKMComponent);
}
void UGBWAnimComponent::StopMontage_Imp(const USkeletalMeshComponent* SKMComponent)
{
	if (SKMComponent && SKMComponent->GetAnimInstance())
	{
		SKMComponent->GetAnimInstance()->Montage_Stop(0.1f);
	}
}

UAnimMontage* UGBWAnimComponent::GetNextMontageFromList(TArray<UAnimMontage*> MontageList)
{
	UAnimMontage* Res = nullptr;

	if (!AnimPlayState.AnimPlayedCache.LastMontage)
	{
		for (const auto Element : MontageList)
		{
			if (Element)
			{
				Res = Element;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < MontageList.Num(); i++)
		{
			if (MontageList[i])
			{
				if (MontageList[i] == AnimPlayState.AnimPlayedCache.LastMontage)
				{
					for (int j = i+1; j < MontageList.Num(); j++)
					{
						if (MontageList[j])
						{
							Res = MontageList[j];
							break;
						}
					}
					if (Res)
					{
						break;
					}
					else
					{
						for (const auto Element : MontageList)
						{
							if (Element)
							{
								Res = Element;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (!Res)
	{
		for (const auto Element : MontageList)
		{
			if (Element)
			{
				Res = Element;
				break;
			}
		}
	}

	return Res;
}

UAnimSequence* UGBWAnimComponent::GetNextAnimSequenceFromList(
		bool bHoldNowWhenAnimActive,
		bool bHoldNowWhenAnimIsPreAnim,
		TArray<UAnimSequence*> AnimSequenceList)
{
	UAnimSequence* Res = nullptr;
	UAnimSequence* LastAnim = nullptr;
	int LastAnimIndex = -1;
	if (bHoldNowWhenAnimActive)
	{
		if (AnimSequenceList.Find(AnimPlayState.AnimPlayedCache.GetAnimSequenceNow(),LastAnimIndex))
		{
			Res = AnimPlayState.AnimPlayedCache.GetAnimSequenceNow();
		}
	}
	if (Res)
	{
		return Res;
	}
	
	if (bHoldNowWhenAnimIsPreAnim)
	{
		for (int i = 0; i < 3; i++)
		{
			UAnimSequence* A = AnimPlayState.AnimPlayedCache.GetPreviousAnimSequence(i);
			if (AnimSequenceList.Find(A,LastAnimIndex))
			{
				Res = A;
			}
		}
	}
	if (Res)
	{
		return Res;
	}

	if (AnimPlayState.AnimPlayedCache.GetLastAnimSequenceFromArr(AnimSequenceList,LastAnim,LastAnimIndex)
		&& LastAnimIndex < AnimSequenceList.Num() - 1)
	{
		Res = AnimSequenceList[LastAnimIndex+1];
	}
	else if (!AnimSequenceList.IsEmpty())
	{
		Res = AnimSequenceList[0];
	}
	return Res;
}

EGBWAnimPlayStateType UGBWAnimComponent::GetAnimPlayStateByNodeIndex(int32 NodeIndex, bool& bIsReplay)
{
	bIsReplay = false;
	for (const auto Element : AnimPlayState.AnimPlayStateSet)
	{
		if (Element.AnimNodeIndex == NodeIndex)
		{
			bIsReplay = Element.bIsReplay;
			return Element.PlayState;
		}
	}

	return EGBWAnimPlayStateType::Deactivated;
}

bool UGBWAnimComponent::IsAnimStateIsConsistentWithServer() const
{
	return AnimPlayState.MainAnimPlayState.AnimAsset == AnimPlayState_Local.MainAnimPlayState.AnimAsset;
}

bool UGBWAnimComponent::GetStateMachineInfoByNodeIndex(int32 NodeIndex, FGBWAnimStateMachineNodeInfo& SMInfo) const
{
	for (const auto Element : AnimPlayState.StateMachineState)
	{
		if (Element.NodeIndex == NodeIndex)
		{
			SMInfo = Element;
			return true;
		}
	}

	return false;
}
