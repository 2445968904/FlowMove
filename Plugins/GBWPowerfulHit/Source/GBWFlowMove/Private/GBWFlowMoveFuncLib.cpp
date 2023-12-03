// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "GBWFlowMoveFuncLib.h"

#include "GBWFlowMoveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"

bool UGBWFlowMoveFuncLib::GetSKMeshByTag(
	USkeletalMeshComponent*& SKMeshComp,
	ACharacter* Character,
	FName MeshTag,
	TArray<FName> ExcludedMeshTag,
	UAnimationAsset* TargetAnimationAsset)
{
	SKMeshComp = nullptr;
	if (Character && TargetAnimationAsset)
	{
		auto SKMeshes = Character->GetComponentsByTag(USkeletalMeshComponent::StaticClass(),MeshTag);
		if (SKMeshes.IsEmpty())
		{
			Character->GetComponents(USkeletalMeshComponent::StaticClass(),SKMeshes);
		}

		for (const auto SKMeshItem:SKMeshes)
		{
			bool bIsExcluded = false;
			for (const auto ETag:ExcludedMeshTag)
			{
				if (SKMeshItem->ComponentHasTag(ETag))
				{
					bIsExcluded = true;
					break;
				}
			}

			if (!bIsExcluded)
			{
				USkeletalMeshComponent* SKMC = Cast<USkeletalMeshComponent>(SKMeshItem);
				USkeletalMesh* SKMS = nullptr;
				if (SKMC)
				{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
					SKMS = SKMC->GetSkeletalMeshAsset();
#else
					SKMS = SKMC->SkeletalMesh;
#endif
				}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 0
				//if (SKMS->GetSkeleton()->IsCompatible(TargetAnimationAsset->GetSkeleton()))
#else
				if (SKMS->GetSkeleton()->IsCompatible(TargetAnimationAsset->GetSkeleton()))
#endif
				{
					if (SKMS->GetSkeleton()->IsCompatible(TargetAnimationAsset->GetSkeleton()))
					{
						SKMeshComp = SKMC;
						return  true;
					}
				}
			}
		}
	}
	
	return false;
}

bool UGBWFlowMoveFuncLib::IsLocalOwn(AActor* Actor)
{
	bool bRes;

	if (Actor)
	{
		const ENetRole LocalRole = Actor->GetLocalRole();
		const ENetRole RemoteRole = Actor->GetRemoteRole();

		if (LocalRole == ENetRole::ROLE_Authority)
		{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 3
			if (RemoteRole != ENetRole::ROLE_AutonomousProxy)
			{
				bRes = true;
			}
			else
			{
				bRes = false;
			}
#else
			bRes = true;
#endif
		}
		else
		{
			if (LocalRole == ENetRole::ROLE_AutonomousProxy)
			{
				bRes = true;
			}
			else
			{
				bRes = false;
			}
		}
	}
	else
	{
		bRes = false;
	}
	
	return bRes;
}

bool UGBWFlowMoveFuncLib::IsAIPlayer(AActor* Actor)
{
	bool bRes;

	if (Actor)
	{
		const ENetRole LocalRole = Actor->GetLocalRole();
		const ENetRole RemoteRole = Actor->GetRemoteRole();
		
		bRes = LocalRole == ENetRole::ROLE_Authority
		&& RemoteRole != ENetRole::ROLE_AutonomousProxy
		&& Actor != UGameplayStatics::GetPlayerCharacter(Actor, 0);
	}
	else
	{
		bRes = false;
	}
	
	return bRes;
}

FHitResult UGBWFlowMoveFuncLib::SweepMulti(
	const UWorld* World,
	TArray<FHitResult>& OutHits,
	const FVector& Start,
	const FVector& End,
	const FQuat& Rot,
	FGBWFlowMoveTraceSetting TraceSetting,
	const FCollisionShape& CollisionShape,
	const FCollisionQueryParams& Params)
{
	FHitResult Res = FHitResult();
	if (!World)
	{
		return Res;
	}

	if (TraceSetting.TraceObjectTypes.IsEmpty() && TraceSetting.TraceChannels.IsEmpty())
	{
		return Res;
	}
	
	TArray<FHitResult> ObjectTypeOutHits;
	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : TraceSetting.TraceObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}
	World->SweepMultiByObjectType(
		ObjectTypeOutHits,
		Start,
		End,
		Rot,
		ObjectQueryParameters,
		CollisionShape,
		Params);
	
	TArray<FHitResult> ChannelOutHits;
	for (const auto Channel:TraceSetting.TraceChannels)
	{
		TArray<FHitResult> TempChannelOutHits;
        	World->SweepMultiByChannel(
        	TempChannelOutHits,
        	Start,
        	End,
        	Rot,
        	UEngineTypes::ConvertToCollisionChannel(Channel),
        	CollisionShape,
        	Params
        	);

		ChannelOutHits.Append(TempChannelOutHits);
	}

	TArray<AActor*> ResultOutHits;
	OutHits.Empty();
	for (auto Item:ObjectTypeOutHits)
	{
   		if (!ResultOutHits.Contains(Item.GetActor()))
		{
			OutHits.Add(Item);
			ResultOutHits.Add(Item.GetActor());
		}
	}
	for (auto Item:ChannelOutHits)
	{
		if (Item.IsValidBlockingHit() && !ResultOutHits.Contains(Item.GetActor()))
		{
			OutHits.Add(Item);
			ResultOutHits.Add(Item.GetActor());
		}
	}

	if (TraceSetting.isDebug && TraceSetting.DebugTime>=0)
	{
		if (TraceSetting.DrawTraceShape)
		{
			DrawDebugShape(World,Start,FQuat::Identity,CollisionShape,
            			!OutHits.IsEmpty()? FColor::Red : FColor::White,
            			TraceSetting.DebugTime,1);
			DrawDebugShape(World,End,FQuat::Identity,CollisionShape,
						!OutHits.IsEmpty()? FColor::Red : FColor::White,
						TraceSetting.DebugTime,1);
		}
		
		if (!OutHits.IsEmpty())
		{
			DrawDebugSphere(World,OutHits[0].ImpactPoint,5,8,
				FColor::Green,
				false,TraceSetting.DebugTime,0,10);
		}

		if (TraceSetting.DrawTraceLine)
		{
			DrawDebugLine(World, Start, End,
			              !OutHits.IsEmpty()? FColor::Red : FColor::White,
			              false,TraceSetting.DebugTime,0,1);
		}
	}

	if (!OutHits.IsEmpty())
	{
		Res = OutHits[0];
	}
	else
	{
		Res.TraceStart = Start;
		Res.TraceEnd = End;
	}
	for (auto Hit:OutHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			Res = Hit;
			break;
		}
	}
	
	if (Res.GetActor() && !TraceSetting.IsMetScreeningConditions(Res.GetActor()))
	{
		Res = FHitResult();
	}
	
	return Res;
}

bool UGBWFlowMoveFuncLib::OverlapAnyTest(
	const UWorld* World,
	const FVector& Pos,
	const FQuat& Rot,
	FGBWFlowMoveTraceSetting TraceSetting,
	const FCollisionShape& CollisionShape,
	const FCollisionQueryParams& Params)
{
	if (!World)
	{
		return false;
	}

	if (TraceSetting.TraceObjectTypes.IsEmpty() && TraceSetting.TraceChannels.IsEmpty())
	{
		return false;
	}
	
	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : TraceSetting.TraceObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}
	if (World->OverlapAnyTestByObjectType(
		Pos,
		Rot,
		ObjectQueryParameters,
		CollisionShape,
		Params))
	{
		return true;
	}

	for (auto Item:TraceSetting.TraceChannels)
	{
		if (World->OverlapAnyTestByChannel(
			Pos,
			Rot,
			UEngineTypes::ConvertToCollisionChannel(Item),
			CollisionShape,
			Params
		))
		{
			return true;
		}
	}

	return false;
}

FHitResult UGBWFlowMoveFuncLib::LineTrace(
		const UWorld* World,
		TArray<struct FHitResult>& OutHits,
		const FVector& Start,
		const FVector& End,
		FGBWFlowMoveTraceSetting TraceSetting,
		const FCollisionQueryParams& Params)
{
	FHitResult Res = FHitResult();
	if (!World)
	{
		return Res;
	}

	if (TraceSetting.TraceObjectTypes.IsEmpty() && TraceSetting.TraceChannels.IsEmpty())
	{
		return Res;
	}

	TArray<FHitResult> ObjectTypeOutHits;
	FCollisionObjectQueryParams ObjectQueryParameters;
	for (const auto ObjectType : TraceSetting.TraceObjectTypes)
	{
		ObjectQueryParameters.AddObjectTypesToQuery(UCollisionProfile::Get()->ConvertToCollisionChannel(false, ObjectType));
	}
	World->LineTraceMultiByObjectType(
		ObjectTypeOutHits,
		Start,
		End,
		ObjectQueryParameters,
		Params
	);
	
	TArray<FHitResult> ChannelOutHits;
	for (const auto Channel:TraceSetting.TraceChannels)
	{
		TArray<FHitResult> TempChannelOutHits;
		World->LineTraceMultiByChannel(
		ChannelOutHits,
		Start,
		End,
		UEngineTypes::ConvertToCollisionChannel(Channel),
		Params
		);

		ChannelOutHits.Append(TempChannelOutHits);
	}

	TArray<AActor*> ResultOutHits;
	OutHits.Empty();
	for (auto Item:ObjectTypeOutHits)
	{
		if (!ResultOutHits.Contains(Item.GetActor()))
		{
			OutHits.Add(Item);
			ResultOutHits.Add(Item.GetActor());
		}
	}
	for (auto Item:ChannelOutHits)
	{
		if (Item.IsValidBlockingHit() && !ResultOutHits.Contains(Item.GetActor()))
		{
			OutHits.Add(Item);
			ResultOutHits.Add(Item.GetActor());
		}
	}

	if (!OutHits.IsEmpty())
	{
		Res = OutHits[0];
	}
	else
	{
		Res.TraceStart = Start;
		Res.TraceEnd = End;
	}
	for (auto Hit:OutHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			Res = Hit;
			break;
		}
	}

	if (!TraceSetting.IsMetScreeningConditions(Res.GetActor()))
	{
		Res = FHitResult();
	}

	if (TraceSetting.isDebug && TraceSetting.DebugTime>=0)
	{
		DrawDebugLine(World,Start,End,FColor::Orange,false,TraceSetting.DebugTime,0,2);
		if (!OutHits.IsEmpty())
		{
			DrawDebugSphere(World,OutHits[0].ImpactPoint,5,8,FColor::Green,false,TraceSetting.DebugTime,0,10);
		}
	}
	
	return Res;
}

bool UGBWFlowMoveFuncLib::GetTheLastValidHitResult(TArray<FHitResult> HitResultArr, FHitResult& Result)
{
	for (int i=HitResultArr.Num()-1; i>=0; i--)
	{
		if (HitResultArr[i].IsValidBlockingHit())
		{
			Result = HitResultArr[i];
			return true;
		}
	}
	return false;
}

void UGBWFlowMoveFuncLib::FindFloorAndRoof(
	ACharacter* Character,
	FVector FromLocation,
	float FloorTraceDistance,
	float RoofTraceDistance,
	bool bFindLedge, 
	float LedgeTraceDistance,
	FGBWFlowMoveTraceSetting TraceSetting,
	bool& Floor, FHitResult& OutHit_Floor, bool& Roof, FHitResult& OutHit_Roof,
	FGBWFlowMoveFloorRoofScene& FloorAndRoofRes)
{
	FloorAndRoofRes = FGBWFlowMoveFloorRoofScene();
	Floor = false;
	Roof = false;
	OutHit_Floor = FHitResult();
	OutHit_Roof = FHitResult();
	if (Character)
	{
		FVector ForwardVector = Character->GetActorForwardVector();
		ForwardVector.Z = 0.0f;
		ForwardVector.Normalize();

		float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		
		FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
		CollisionQueryParams.bTraceComplex = false;
		CollisionQueryParams.AddIgnoredActor(Character);

		FHitResult TraceHit;
		static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
		CollisionQueryParams.TraceTag = ForwardTraceTag;
		TArray<struct FHitResult> TraceHitArr;
		TraceHit = SweepMulti(
			Character->GetWorld(),
			TraceHitArr,
			FromLocation,
			FromLocation - FVector(0,0, FloorTraceDistance),
			FQuat::Identity,
			TraceSetting,
			FCollisionShape::MakeSphere(CapsuleRadius),
			CollisionQueryParams
		);
		if (TraceHit.IsValidBlockingHit())
		{
			Floor = true;
			OutHit_Floor = TraceHit;
			FloorAndRoofRes.Floor = TraceHit.ImpactPoint;

			if (bFindLedge)
			{
				//Forward Floor Ledge Trace
				float TraceDistance = 0.0f;
				for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
				{
					TraceDistance = (i+1) * CapsuleRadius;
					FVector StartLocation = FromLocation + ForwardVector * TraceDistance;
					TraceHitArr.Empty();
					TraceHit = SweepMulti(
						Character->GetWorld(),
						TraceHitArr,
						StartLocation,
						StartLocation - FVector(0,0, FloorTraceDistance),
						FQuat::Identity,
						TraceSetting,
						FCollisionShape::MakeSphere(CapsuleRadius),
						CollisionQueryParams
					);
					if ((TraceHit.IsValidBlockingHit()
						&& TraceHit.ImpactPoint.Z < OutHit_Floor.ImpactPoint.Z - Character->GetCharacterMovement()->MaxStepHeight)
						|| !TraceHit.IsValidBlockingHit())
					{
						FVector LedgePoint = FVector(
								StartLocation.X,
								StartLocation.Y,
								OutHit_Floor.ImpactPoint.Z
							);

						TraceHitArr.Empty();
						TraceHit = LineTrace(
							Character->GetWorld(),
							TraceHitArr,
							FromLocation,
							LedgePoint,
							TraceSetting,
							CollisionQueryParams
						);
						if (!TraceHit.IsValidBlockingHit())
						{
							FloorAndRoofRes.FloorLedge_Forward = LedgePoint;
                            break;
						}
						else
						{
							break;
						}
					}
				}

				//Backward Floor Ledge Trace
				TraceDistance = 0.0f;
				for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
				{
					TraceDistance = (i+1) * CapsuleRadius;
					FVector StartLocation = FromLocation - ForwardVector * TraceDistance;
					TraceHitArr.Empty();
					TraceHit = SweepMulti(
						Character->GetWorld(),
						TraceHitArr,
						StartLocation,
						StartLocation - FVector(0,0, FloorTraceDistance),
						FQuat::Identity,
						TraceSetting,
						FCollisionShape::MakeSphere(CapsuleRadius),
						CollisionQueryParams
					);
					if ((TraceHit.IsValidBlockingHit()
						&& TraceHit.ImpactPoint.Z < OutHit_Floor.ImpactPoint.Z - Character->GetCharacterMovement()->MaxStepHeight)
						|| !TraceHit.IsValidBlockingHit())
					{
						FVector LedgePoint = FVector(
								StartLocation.X,
								StartLocation.Y,
								OutHit_Floor.ImpactPoint.Z
							);

						TraceHitArr.Empty();
						TraceHit = LineTrace(
							Character->GetWorld(),
							TraceHitArr,
							FromLocation,
							LedgePoint,
							TraceSetting,
							CollisionQueryParams
						);
						if (!TraceHit.IsValidBlockingHit())
						{
							FloorAndRoofRes.FloorLedge_Backward = LedgePoint;
							break;
						}
						else
						{
							break;
						}
					}
				}	
			}
		}
		else
		{
			Floor = false;
		}

		TraceHitArr.Empty();
		TraceHit = SweepMulti(
			Character->GetWorld(),
			TraceHitArr,
			FromLocation,
			FromLocation + FVector(0,0, RoofTraceDistance),
			FQuat::Identity,
			TraceSetting,
			FCollisionShape::MakeSphere(CapsuleRadius),
			CollisionQueryParams
		);
		if (TraceHit.IsValidBlockingHit())
		{
			Roof = true;
			OutHit_Roof = TraceHit;
			FloorAndRoofRes.Roof = TraceHit.ImpactPoint;

			if (bFindLedge)
			{
				//Forward Roof Ledge Trace
				float TraceDistance = 0.0f;
				for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
				{
					TraceDistance = (i+1) * CapsuleRadius;
					FVector StartLocation = FromLocation + ForwardVector * TraceDistance;
					TraceHitArr.Empty();
					TraceHit = SweepMulti(
						Character->GetWorld(),
						TraceHitArr,
						StartLocation,
						StartLocation + FVector(0,0, RoofTraceDistance),
						FQuat::Identity,
						TraceSetting,
						FCollisionShape::MakeSphere(CapsuleRadius),
						CollisionQueryParams
					);
					if ((TraceHit.IsValidBlockingHit()
						&& TraceHit.ImpactPoint.Z > OutHit_Roof.ImpactPoint.Z + CapsuleRadius * 4.0f)
						|| !TraceHit.IsValidBlockingHit())
					{
						FVector LedgePoint = FVector(
								StartLocation.X,
								StartLocation.Y,
								OutHit_Roof.ImpactPoint.Z
							);

						TraceHitArr.Empty();
						TraceHit = LineTrace(
							Character->GetWorld(),
							TraceHitArr,
							FromLocation,
							LedgePoint,
							TraceSetting,
							CollisionQueryParams
						);
						if (!TraceHit.IsValidBlockingHit())
						{
							FloorAndRoofRes.RoofLedge_Forward = LedgePoint;
							break;
						}
						else
						{
							break;
						}
					}
				}

				//Backward Roof Ledge Trace
				TraceDistance = 0.0f;
				for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
				{
					TraceDistance = (i+1) * CapsuleRadius;
					FVector StartLocation = FromLocation - ForwardVector * TraceDistance;
					TraceHitArr.Empty();
					TraceHit = SweepMulti(
						Character->GetWorld(),
						TraceHitArr,
						StartLocation,
						StartLocation + FVector(0,0, RoofTraceDistance),
						FQuat::Identity,
						TraceSetting,
						FCollisionShape::MakeSphere(CapsuleRadius),
						CollisionQueryParams
					);
					if ((TraceHit.IsValidBlockingHit()
						&& TraceHit.ImpactPoint.Z > OutHit_Roof.ImpactPoint.Z + CapsuleRadius * 4.0f)
						|| !TraceHit.IsValidBlockingHit())
					{
						FVector LedgePoint = FVector(
								StartLocation.X,
								StartLocation.Y,
								OutHit_Roof.ImpactPoint.Z
							);

						TraceHitArr.Empty();
						TraceHit = LineTrace(
							Character->GetWorld(),
							TraceHitArr,
							FromLocation,
							LedgePoint,
							TraceSetting,
							CollisionQueryParams
						);
						if (!TraceHit.IsValidBlockingHit())
						{
							FloorAndRoofRes.RoofLedge_Backward = LedgePoint;
							break;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		else
		{
			Roof = false;
		}
	}
}

void UGBWFlowMoveFuncLib::FindBlockWall(
	const ACharacter* Character,
	FVector TraceVector,
	float WallTraceDistance,
	bool bFindLedge,
	float LedgeTraceDistance,
	FGBWFlowMoveTraceSetting TraceSetting,
	FGBWFlowMoveBlockWallScene& WallRes)
{
	WallRes = FGBWFlowMoveBlockWallScene();
	if (!Character)
	{
		return;
	}

	FVector ForwardVector = TraceVector;
	ForwardVector.Z = 0.0f;
	ForwardVector.Normalize();

	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector FromLocation = Character->GetCapsuleComponent()->GetComponentLocation();
	
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);

	FHitResult TraceHit;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> TraceHitArr;
	TraceHit = SweepMulti(
		Character->GetWorld(),
		TraceHitArr,
		FromLocation,
		FromLocation + ForwardVector * WallTraceDistance,
		FQuat::Identity,
		TraceSetting,
		FCollisionShape::MakeSphere(CapsuleRadius),
		CollisionQueryParams
	);
	if (TraceHit.IsValidBlockingHit())
	{
		FHitResult WallTraceHit = TraceHit;
		WallRes.WallPoint = TraceHit.ImpactPoint;
		WallRes.DistanceFromActor = (WallTraceHit.ImpactPoint - WallTraceHit.TraceStart).Length();

		ForwardVector = -WallTraceHit.ImpactNormal;
		ForwardVector.Z = 0.0;
		ForwardVector.Normalize();
		
		if (bFindLedge)
		{
			FromLocation = FVector(FromLocation.X, FromLocation.Y, WallTraceHit.ImpactPoint.Z);
			//Up Ledge Trace
			float TraceDistance = 0.0f;
			for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
			{
				TraceDistance = (i+1) * CapsuleRadius;
				FVector StartLocation = FromLocation + FVector(0.0f,0.0f,1.0f) * TraceDistance;
				TraceHitArr.Empty();
				TraceHit = SweepMulti(
					Character->GetWorld(),
					TraceHitArr,
					StartLocation,
					StartLocation + ForwardVector * WallTraceDistance,
					FQuat::Identity,
					TraceSetting,
					FCollisionShape::MakeSphere(CapsuleRadius),
					CollisionQueryParams
				);
				if ((TraceHit.IsValidBlockingHit()
					&& (TraceHit.ImpactPoint - TraceHit.TraceStart).Length() >= WallRes.DistanceFromActor + CapsuleRadius)
					|| !TraceHit.IsValidBlockingHit())
				{
					FVector LedgePoint = FVector(
							WallTraceHit.ImpactPoint.X,
							WallTraceHit.ImpactPoint.Y,
							StartLocation.Z
						);

					TraceHitArr.Empty();
					TraceHit = LineTrace(
						Character->GetWorld(),
						TraceHitArr,
						FromLocation,
						LedgePoint,
						TraceSetting,
						CollisionQueryParams
					);
					if (!TraceHit.IsValidBlockingHit())
					{
						WallRes.WallLedge_Up = LedgePoint;
						break;
					}
					else
					{
						break;
					}
				}
			}

			//Down Ledge Trace
			TraceDistance = 0.0f;
			for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
			{
				TraceDistance = (i+1) * CapsuleRadius;
				FVector StartLocation = FromLocation - FVector(0.0f,0.0f,1.0f) * TraceDistance;
				TraceHitArr.Empty();
				TraceHit = SweepMulti(
					Character->GetWorld(),
					TraceHitArr,
					StartLocation,
					StartLocation + ForwardVector * WallTraceDistance,
					FQuat::Identity,
					TraceSetting,
					FCollisionShape::MakeSphere(CapsuleRadius),
					CollisionQueryParams
				);
				if ((TraceHit.IsValidBlockingHit()
					&& (TraceHit.ImpactPoint - TraceHit.TraceStart).Length() >= WallRes.DistanceFromActor + CapsuleRadius)
					|| !TraceHit.IsValidBlockingHit())
				{
					FVector LedgePoint = FVector(
							WallTraceHit.ImpactPoint.X,
							WallTraceHit.ImpactPoint.Y,
							StartLocation.Z
						);

					TraceHitArr.Empty();
					TraceHit = LineTrace(
						Character->GetWorld(),
						TraceHitArr,
						FromLocation,
						LedgePoint,
						TraceSetting,
						CollisionQueryParams
					);
					if (!TraceHit.IsValidBlockingHit())
					{
						WallRes.WallLedge_Down = LedgePoint;
						break;
					}
					else
					{
						break;
					}
				}
			}

			//Left Ledge Trace
			TraceDistance = 0.0f;
			for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
			{
				TraceDistance = (i+1) * CapsuleRadius;
				FVector StartLocation = FromLocation - FVector(-ForwardVector.Y, ForwardVector.X, 0.0f) * TraceDistance;
				TraceHitArr.Empty();
				TraceHit = SweepMulti(
					Character->GetWorld(),
					TraceHitArr,
					StartLocation,
					StartLocation + ForwardVector * WallTraceDistance,
					FQuat::Identity,
					TraceSetting,
					FCollisionShape::MakeSphere(CapsuleRadius),
					CollisionQueryParams
				);
				if ((TraceHit.IsValidBlockingHit()
					&& (TraceHit.ImpactPoint - TraceHit.TraceStart).Length() >= WallRes.DistanceFromActor + CapsuleRadius)
					|| !TraceHit.IsValidBlockingHit())
				{
					FVector LedgePoint = GetPointToLineFoot(TraceHit.TraceStart, TraceHit.TraceEnd, WallTraceHit.ImpactPoint);

					TraceHitArr.Empty();
					TraceHit = LineTrace(
						Character->GetWorld(),
						TraceHitArr,
						FromLocation,
						LedgePoint,
						TraceSetting,
						CollisionQueryParams
					);
					if (!TraceHit.IsValidBlockingHit())
					{
						WallRes.WallLedge_Left = LedgePoint;
						break;
					}
					else
					{
						break;
					}
				}
			}

			//Right Ledge Trace
			TraceDistance = 0.0f;
			for (int i = 0; TraceDistance <= LedgeTraceDistance; i++)
			{
				TraceDistance = (i+1) * CapsuleRadius;
				FVector StartLocation = FromLocation + FVector(-ForwardVector.Y, ForwardVector.X, 0.0f) * TraceDistance;
				TraceHitArr.Empty();
				TraceHit = SweepMulti(
					Character->GetWorld(),
					TraceHitArr,
					StartLocation,
					StartLocation + ForwardVector * WallTraceDistance,
					FQuat::Identity,
					TraceSetting,
					FCollisionShape::MakeSphere(CapsuleRadius),
					CollisionQueryParams
				);
				if ((TraceHit.IsValidBlockingHit()
					&& (TraceHit.ImpactPoint - TraceHit.TraceStart).Length() >= WallRes.DistanceFromActor + CapsuleRadius)
					|| !TraceHit.IsValidBlockingHit())
				{
					FVector LedgePoint = GetPointToLineFoot(TraceHit.TraceStart, TraceHit.TraceEnd, WallTraceHit.ImpactPoint);

					TraceHitArr.Empty();
					TraceHit = LineTrace(
						Character->GetWorld(),
						TraceHitArr,
						FromLocation,
						LedgePoint,
						TraceSetting,
						CollisionQueryParams
					);
					if (!TraceHit.IsValidBlockingHit())
					{
						WallRes.WallLedge_Right = LedgePoint;
						break;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	if (WallRes.IsWall())
	{
		DrawDebugSphere(Character->GetWorld(),WallRes.WallPoint,30,8,FColor::Orange,false,TraceSetting.DebugTime,0,10);
	}
	if (WallRes.IsLedge_Down())
	{
		DrawDebugSphere(Character->GetWorld(),WallRes.WallLedge_Down,30,8,FColor::Orange,false,TraceSetting.DebugTime,0,10);
	}
	if (WallRes.IsLedge_Up())
	{
		DrawDebugSphere(Character->GetWorld(),WallRes.WallLedge_Up,30,8,FColor::Orange,false,TraceSetting.DebugTime,0,10);
	}
	if (WallRes.IsLedge_Left())
	{
		DrawDebugSphere(Character->GetWorld(),WallRes.WallLedge_Left,30,8,FColor::Orange,false,TraceSetting.DebugTime,0,10);
	}
	if (WallRes.IsLedge_Right())
	{
		DrawDebugSphere(Character->GetWorld(),WallRes.WallLedge_Right,30,8,FColor::Orange,false,TraceSetting.DebugTime,0,10);
	}
}

void UGBWFlowMoveFuncLib::AddBool(FGBWPerceptionResult PerceptionResult, FName Key, bool Value)
{
	PerceptionResult.AddBool(Key,Value);
}

void UGBWFlowMoveFuncLib::AddFloat(FGBWPerceptionResult PerceptionResult, FName Key, float Value)
{
	PerceptionResult.AddFloat(Key,Value);
}

void UGBWFlowMoveFuncLib::AddVector(FGBWPerceptionResult PerceptionResult, FName Key, FVector Value)
{
	PerceptionResult.AddVector(Key,Value);
}

void UGBWFlowMoveFuncLib::AddTransform(FGBWPerceptionResult PerceptionResult, FName Key, FTransform Value)
{
	PerceptionResult.AddTransform(Key,Value);
}

void UGBWFlowMoveFuncLib::AddString(FGBWPerceptionResult PerceptionResult, FName Key, FString Value)
{
	PerceptionResult.AddString(Key,Value);
}

void UGBWFlowMoveFuncLib::AddGameplayTag(FGBWPerceptionResult PerceptionResult, FName Key, FGameplayTag Value)
{
	PerceptionResult.AddGameplayTag(Key,Value);
}

void UGBWFlowMoveFuncLib::AddActor(FGBWPerceptionResult PerceptionResult, FName Key, AActor* Value)
{
	PerceptionResult.AddActor(Key,Value);
}

void UGBWFlowMoveFuncLib::AddObject(FGBWPerceptionResult PerceptionResult, FName Key, UObject* Value)
{
	PerceptionResult.AddObject(Key,Value);
}

void UGBWFlowMoveFuncLib::AddMovementMode(FGBWPerceptionResult PerceptionResult, FName Key, EMovementMode Value)
{
	PerceptionResult.AddMovementMode(Key,Value);
}

void UGBWFlowMoveFuncLib::GetBool(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, bool& Value)
{
	PerceptionResult.GetBool(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetFloat(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, float& Value)
{
	PerceptionResult.GetFloat(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetVector(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FVector& Value)
{
	PerceptionResult.GetVector(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetTransform(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet,
	FTransform& Value)
{
	PerceptionResult.GetTransform(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetString(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, FString& Value)
{
	PerceptionResult.GetString(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetGameplayTag(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet,
	FGameplayTag& Value)
{
	PerceptionResult.GetGameplayTag(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetActor(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, AActor*& Value)
{
	PerceptionResult.GetActor(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetObject(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet, UObject*& Value)
{
	PerceptionResult.GetObject(Key,IsGet,Value);
}

void UGBWFlowMoveFuncLib::GetMovementMode(FGBWPerceptionResult PerceptionResult, FName Key, bool& IsGet,
	TEnumAsByte<EMovementMode>& Value)
{
	PerceptionResult.GetMovementMode(Key,IsGet,Value);
}

FGBWFlowMovePointScene UGBWFlowMoveFuncLib::MakeFlowMovePointScene(ACharacter* InCharacter, const FVector& InPoint, bool bFindLedge, float LedgeTraceDistance, 
                                                                   const FGBWFlowMoveTraceSetting& TraceSetting)
{
	return FGBWFlowMovePointScene(InCharacter,InPoint,bFindLedge,LedgeTraceDistance,TraceSetting);
}

FVector UGBWFlowMoveFuncLib::RadianToDirectionXY(float Radian)
{
	float Sin, Cos;
	FMath::SinCos(&Sin, &Cos, Radian);

	return {Cos, Sin, 0.0f};
}

FVector UGBWFlowMoveFuncLib::AngleToDirectionXY(float Angle)
{
	return RadianToDirectionXY(FMath::DegreesToRadians(Angle));
}

float UGBWFlowMoveFuncLib::GetVelocityYawAngle(FVector Velocity)
{
	return UE_REAL_TO_FLOAT(FMath::RadiansToDegrees(FMath::Atan2(Velocity.Y, Velocity.X)));
}

float UGBWFlowMoveFuncLib::GetInputYawAngle(FVector InputDirection)
{
	const FVector_NetQuantizeNormal TheInputDirection = InputDirection.GetSafeNormal();
	return UE_REAL_TO_FLOAT(FMath::RadiansToDegrees(FMath::Atan2(TheInputDirection.Y, TheInputDirection.X)));
}

float UGBWFlowMoveFuncLib::GetPointToLineDistance(const FVector LineStart, const FVector LineEnd, const FVector Point)
{
	const float A = (LineStart - LineEnd).Length();
	const float B = (LineStart - Point).Length();
	const float C = (LineEnd - Point).Length();

	return FMath::Sqrt((A+B+C)*(A+B-C)*(A+C-B)*(B+C-A))/2/A;
}

FVector UGBWFlowMoveFuncLib::GetPointToLineFoot(const FVector& LineStart, const FVector& LineEnd, const FVector& Point)
{
	FVector retVal;
	double dx = LineStart.X - LineEnd.X;
	double dy = LineStart.Y - LineEnd.Y;
	double dz = LineStart.Z - LineEnd.Z;
	if(abs(dx) < 0.00000001 && abs(dy) < 0.00000001 && abs(dz) < 0.00000001 )
		{ retVal = LineStart; return retVal; }
	double u = (Point.X - LineStart.X)*(LineStart.X - LineEnd.X)
			+ (Point.Y - LineStart.Y)*(LineStart.Y - LineEnd.Y)
			+ (Point.Z - LineStart.Z)*(LineStart.Z - LineEnd.Z);
	u = u/((dx*dx)+(dy*dy)+(dz*dz));
	retVal.X = LineStart.X + u*dx;
	retVal.Y = LineStart.Y + u*dy;
	retVal.Z = LineStart.Z + u*dz;
	return retVal;
}

float UGBWFlowMoveFuncLib::GetMoveDirection(const ACharacter* OwnerCharacter, FVector DirectionVector, bool bIsUseActorSpeedVector)
{
	if (!OwnerCharacter)
	{
		return 0.0f;
	}

	FTransform T = OwnerCharacter->GetActorTransform();
	FVector Velocity = OwnerCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Velocity.Normalize();
	if (bIsUseActorSpeedVector)
	{
		T.SetRotation(Velocity.Rotation().Quaternion());
	}
	
	FVector A = UKismetMathLibrary::ProjectVectorOnToPlane(
		DirectionVector,
		OwnerCharacter->GetActorUpVector());
	A.Normalize();
	FVector B = UKismetMathLibrary::ProjectVectorOnToPlane(
		bIsUseActorSpeedVector? Velocity:OwnerCharacter->GetCapsuleComponent()->GetForwardVector(),
		OwnerCharacter->GetActorUpVector());
	B.Normalize();
	const float DirectionAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(A,B));

	const FVector A2 = UKismetMathLibrary::InverseTransformDirection(T,A);
	const FVector B2 = UKismetMathLibrary::InverseTransformDirection(T,B);
	const bool bIsRight = UKismetMathLibrary::CrossProduct2D(
		FVector2D(A2.X, A2.Y),
		FVector2D(B2.X, B2.Y)
	)<=0;

	const float SceneDirection = DirectionAngle * (bIsRight? 1 : -1);

	return SceneDirection;
}

FVector UGBWFlowMoveFuncLib::GetMoveVectorFromCharacterInputVector(const ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter)
    {
        return FVector::ZeroVector;
    }

    FTransform T = OwnerCharacter->GetActorTransform();
    FRotator CRotator = OwnerCharacter->GetControlRotation();
	CRotator.Roll = 0.0f;
	CRotator.Pitch = 0.0f;
    T.SetRotation(CRotator.Quaternion());

    return UKismetMathLibrary::InverseTransformDirection(T, OwnerCharacter->GetLastMovementInputVector());
}

void UGBWFlowMoveFuncLib::Boxplot_Float(const TArray<float>& InDataArr, TArray<float>& OutDataArr)
{
	if (InDataArr.Num() <= 0)
	{
		return;
	}
	TArray<float> DataArr = InDataArr;
	DataArr.Sort();
	const float Q1 = DataArr[DataArr.Num()*0.25f];
	//const float Q2 = DataArr[DataArr.Num()*0.50f];
	const float Q3 = DataArr[DataArr.Num()*0.75f];
	const float Iqr = Q3 - Q1;

	const float Outlier_Min = Q1 - 1.5 * Iqr;
	const float Outlier_Max = Q3 + 1.5 * Iqr;

	OutDataArr.Empty();
	for (auto Data : DataArr)
	{
		if (Outlier_Min <= Data && Data <= Outlier_Max)
		{
			OutDataArr.Add(Data);
		}
	}
}

UGBWFlowMoveComponent* UGBWFlowMoveFuncLib::FM_GetFlowMoveComponent(const AActor* InActor)
{
	if (!InActor)
	{
		return nullptr;
	}

	UActorComponent* AComp = InActor->GetComponentByClass(UGBWFlowMoveComponent::StaticClass());
	if (!AComp)
	{
		return nullptr;
	}

	if (UGBWFlowMoveComponent* FMComp = Cast<UGBWFlowMoveComponent>(AComp))
	{
		return FMComp;
	}

	return nullptr;
}

void UGBWFlowMoveFuncLib::FM_ActiveFlowMove(const AActor* InActor)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->ActiveFlowMove();
	}
}

void UGBWFlowMoveFuncLib::FM_StopFlowMove(const AActor* InActor, bool WaitForCurrentActionFinished)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->StopFlowMove(WaitForCurrentActionFinished);
	}
}

bool UGBWFlowMoveFuncLib::FM_SetMoveVector(const AActor* InActor, FVector NewMoveVector,
	float ZeroFaultToleranceDuration)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetMoveVector(NewMoveVector,ZeroFaultToleranceDuration);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_SetControlVector(const AActor* InActor, FVector NewControlVector)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetControlVector(NewControlVector);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_SetPerceptionVector(const AActor* InActor, FVector NewPerceptionVector)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetPerceptionVector(NewPerceptionVector);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_SetFocusActor(AActor* InActor, USceneComponent* FocusActorComponent, bool bSetViewModeAsActorLock)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetFocusActor(FocusActorComponent, bSetViewModeAsActorLock);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_SetViewMode(AActor* InActor, EFlowMoveCharacterViewMode NewViewMode, bool bAsDefaultViewMode)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetFlowMoveViewMode(NewViewMode, bAsDefaultViewMode);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_TryLockActor(AActor* InActor, FGameplayTag GetTargetFunctionTag)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->TryLockActor(GetTargetFunctionTag);
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneType(
	AActor* InActor,
	FGameplayTag TargetScene,
	FFlowMoveSceneTypeCondition SceneTypeCondition,
	bool SetAsCurrentTargetSceneWhenMet)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		if (!SceneTypeCondition.SceneTypeCondition.IsEmpty())
		{
			FGBWFlowMoveScene TheTargetScene;
			if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
			{
				return false;
			}
			if (const bool bHasSceneType = SceneTypeCondition.SceneTypeCondition.Contains(TheTargetScene.SceneType);
				(SceneTypeCondition.ConditionType == EInclusivityType::Include_ && !bHasSceneType)
				||(SceneTypeCondition.ConditionType == EInclusivityType::NotInclude_ && bHasSceneType))
			{
				return false;
			}
		}
		if (SetAsCurrentTargetSceneWhenMet)
		{
			FMComp->SetCurrentTargetScene(TargetScene);
		}
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneType(
	const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	const FFlowMoveSceneTypeCondition& SceneTypeCondition,
	bool SetAsCurrentTargetSceneWhenMet)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneType(
			AnimInstance->GetOwningActor(),
			TargetScene,
			SceneTypeCondition,
			SetAsCurrentTargetSceneWhenMet);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneType_Simple(AActor* InActor, FGameplayTag TargetScene,
TEnumAsByte<EInclusivityType> ConditionType, FName SceneType, bool SetAsCurrentTargetSceneWhenMet)
{
	FFlowMoveSceneTypeCondition SceneTypeCondition = FFlowMoveSceneTypeCondition();
	SceneTypeCondition.ConditionType = ConditionType;
	SceneTypeCondition.SceneTypeCondition.Add(SceneType);
	return UGBWFlowMoveFuncLib::FM_IsSceneType(
		InActor,
		TargetScene,
		SceneTypeCondition,
		SetAsCurrentTargetSceneWhenMet);
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneType_Simple(const UAnimInstance* AnimInstance,
	FGameplayTag TargetScene,
	TEnumAsByte<EInclusivityType> ConditionType, FName SceneType,
	bool SetAsCurrentTargetSceneWhenMet)
{
	FFlowMoveSceneTypeCondition SceneTypeCondition = FFlowMoveSceneTypeCondition();
	SceneTypeCondition.ConditionType = ConditionType;
	SceneTypeCondition.SceneTypeCondition.Add(SceneType);
	return FM_Pure_IsSceneType(AnimInstance, TargetScene, SceneTypeCondition, SetAsCurrentTargetSceneWhenMet);
}

bool UGBWFlowMoveFuncLib::FM_IsActionLock(AActor* InActor, FGameplayTagContainer ActionToLock)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return FMComp->IsActionLock(ActionToLock);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsActionLock(const UAnimInstance* AnimInstance, const FGameplayTagContainer& ActionToLock)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsActionLock(
			AnimInstance->GetOwningActor(),
			ActionToLock);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsActionLock_Simple(AActor* InActor, FGameplayTag ActionToLock)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return FMComp->IsActionLock(ActionToLock.GetSingleTagContainer());
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsActionLock_Simple(const UAnimInstance* AnimInstance, FGameplayTag ActionToLock)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsActionLock_Simple(
			AnimInstance->GetOwningActor(),
			ActionToLock);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsAnyActionLock(AActor* InActor)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return FMComp->IsAnyActionLock();
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsAnyActionLock(const UAnimInstance* AnimInstance)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsAnyActionLock(
			AnimInstance->GetOwningActor());
	}

	return false;
}

void UGBWFlowMoveFuncLib::FM_IsSide(AActor* InActor, FGameplayTag TargetScene, bool& IsLeft, bool& IsRight,
float& LeftDistance, float& RightDistance,bool& IsLeftCloser,
	bool& IsRightCloser)
{
	IsLeft = false;
	IsRight = false;
	LeftDistance = 0.0f;
	RightDistance = 0.0f;
	IsLeftCloser = false;
	IsRightCloser = false;
	
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene Scene;
		if (FMComp->TaskState.GetSupplementaryScene(TargetScene,Scene))
		{
			IsLeft = !Scene.LeftLocationScene.Point.IsZero();
			IsRight = !Scene.RightLocationScene.Point.IsZero();
			if (IsLeft)
			{
				LeftDistance = (Scene.LeftLocationScene.Point - Scene.ActorLocationScene.Point).Length();
				IsLeftCloser = true;
			}
			if (IsRight)
			{
				RightDistance = (Scene.RightLocationScene.Point - Scene.ActorLocationScene.Point).Length();
				IsRightCloser = true;
			}

			if (IsLeft && IsRight)
			{
				if (LeftDistance > RightDistance)
				{
					IsLeftCloser = true;
					IsRightCloser = false;
				}
				else
				{
					IsLeftCloser = false;
					IsRightCloser = true;
				}
			}
		}
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_IsSide(const UAnimInstance* AnimInstance, FGameplayTag TargetScene, bool& IsLeft,
bool& IsRight, float& LeftDistance, float& RightDistance,bool& IsLeftCloser,
	bool& IsRightCloser)
{
	IsLeft = false;
	IsRight = false;
	LeftDistance = 0.0f;
	RightDistance = 0.0f;
	IsLeftCloser = false;
	IsRightCloser = false;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWFlowMoveFuncLib::FM_IsSide(
			AnimInstance->GetOwningActor(),
			TargetScene,
			IsLeft,
			IsRight,
			LeftDistance,
			RightDistance,
			IsLeftCloser,
			IsRightCloser);
	}
}

bool UGBWFlowMoveFuncLib::FM_IsSceneHeight(
	AActor* InActor,
	FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SceneHeightRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return SceneHeightRange.InScope(TheTargetScene.TargetLocationScene.Height);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneHeight(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SceneHeightRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneHeight(
			AnimInstance->GetOwningActor(),TargetScene,SceneHeightRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneRight(AActor* InActor, FGameplayTag TargetScene,
                                          FGBWFlowMoveFloatScope SceneRightRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return SceneRightRange.InScope(TheTargetScene.TargetLocationScene.Right);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneRight(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SceneRightRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneRight(
			AnimInstance->GetOwningActor(),TargetScene,SceneRightRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneForward(AActor* InActor, FGameplayTag TargetScene,
                                            FGBWFlowMoveFloatScope SceneForwardRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return SceneForwardRange.InScope(TheTargetScene.TargetLocationScene.Forward);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneForward(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SceneForwardRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneForward(
			AnimInstance->GetOwningActor(),TargetScene,SceneForwardRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneAngle(AActor* InActor, FGameplayTag TargetScene,
                                          FGBWFlowMoveFloatScope SceneAngleRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return SceneAngleRange.InScope(TheTargetScene.TargetLocationScene.Angle);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneAngle(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SceneAngleRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneAngle(
			AnimInstance->GetOwningActor(),TargetScene,SceneAngleRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSceneDirection(AActor* InActor, FGameplayTag TargetScene,
                                              FFlowMoveSceneDirectionCondition SceneDirectionCondition)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		return SceneDirectionCondition.IsMet(TheTargetScene);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSceneDirection(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
                                                   const FFlowMoveSceneDirectionCondition& SceneDirectionCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSceneDirection(
			AnimInstance->GetOwningActor(),TargetScene,SceneDirectionCondition);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsMovementMode(AActor* InActor,
                                            FFlowMoveMovementModeCondition MovementModeCondition)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		if (!MovementModeCondition.MovementMode.IsEmpty())
		{
			if (const bool bHasMovementMode = MovementModeCondition.MovementMode.Contains(FMComp->TaskState.MovementMode);
				(MovementModeCondition.ConditionType == EInclusivityType::Include_ && !bHasMovementMode)
				||(MovementModeCondition.ConditionType == EInclusivityType::NotInclude_ && bHasMovementMode))
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsMovementMode(const UAnimInstance* AnimInstance,
                                                 const FFlowMoveMovementModeCondition& MovementModeCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsMovementMode(
			AnimInstance->GetOwningActor(),MovementModeCondition);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsHeightFromGround(AActor* InActor, FGameplayTag TargetScene,
                                                FGBWFlowMoveFloatScope HeightFromGroundRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return HeightFromGroundRange.InScope(TheTargetScene.ActorLocationScene.FloorAndRoof.HeightFromGround);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsHeightFromGround(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope HeightFromGroundRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsHeightFromGround(
			AnimInstance->GetOwningActor(),TargetScene,HeightFromGroundRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsHeightFromRoof(AActor* InActor, FGameplayTag TargetScene,
                                              FGBWFlowMoveFloatScope HeightFromRoofRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return HeightFromRoofRange.InScope(TheTargetScene.ActorLocationScene.FloorAndRoof.HeightFromRoof);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsHeightFromRoof(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope HeightFromRoofRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsHeightFromRoof(
			AnimInstance->GetOwningActor(),TargetScene,HeightFromRoofRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsFloor(AActor* InActor, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope HeightFromFloorRange, bool bIsAnyRequireLedge, FGBWFlowMoveFloatScope FloorLedgeRange,
	bool bIsRequireForwardLedge, FGBWFlowMoveFloatScope ForwardFloorLedgeRange, bool bIsRequireBackwardLedge,
	FGBWFlowMoveFloatScope BackwardFloorLedgeRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}

		if (!HeightFromFloorRange.InScope(TheTargetScene.ActorLocationScene.FloorAndRoof.HeightFromGround))
		{
			return false;
		}
		if (bIsAnyRequireLedge
			&& (!FloorLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward - TheTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length())
				&& !FloorLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward - TheTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length())))
		{
			return false;
		}
		if (bIsRequireForwardLedge
			&& !ForwardFloorLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward - TheTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length()))
		{
			return false;
		}
		if (bIsRequireBackwardLedge
			&& !BackwardFloorLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward - TheTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length()))
		{
			return false;
		}
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsFloor(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope HeightFromFloorRange, bool bIsAnyRequireLedge, FGBWFlowMoveFloatScope FloorLedgeRange,
	bool bIsRequireForwardLedge, FGBWFlowMoveFloatScope ForwardFloorLedgeRange, bool bIsRequireBackwardLedge,
	FGBWFlowMoveFloatScope BackwardFloorLedgeRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsFloor(AnimInstance->GetOwningActor(),
			TargetScene,
			HeightFromFloorRange,
			bIsAnyRequireLedge,
			FloorLedgeRange,
			bIsRequireForwardLedge,
			ForwardFloorLedgeRange,
			bIsRequireBackwardLedge,
			BackwardFloorLedgeRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsRoof(AActor* InActor, FGameplayTag TargetScene,
                                    FGBWFlowMoveFloatScope HeightFromRoofRange, bool bIsAnyRequireLedge, FGBWFlowMoveFloatScope RoofLedgeRange,
                                    bool bIsRequireForwardLedge, FGBWFlowMoveFloatScope ForwardRoofLedgeRange, bool bIsRequireBackwardLedge,
                                    FGBWFlowMoveFloatScope BackwardRoofLedgeRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}

		if (!HeightFromRoofRange.InScope(TheTargetScene.ActorLocationScene.FloorAndRoof.HeightFromRoof))
		{
			return false;
		}
		if (bIsAnyRequireLedge
			&& (!RoofLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward - TheTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length())
				&& !RoofLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward - TheTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length())))
		{
			return false;
		}
		if (bIsRequireForwardLedge
			&& !ForwardRoofLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward - TheTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length()))
		{
			return false;
		}
		if (bIsRequireBackwardLedge
			&& !BackwardRoofLedgeRange.InScope((TheTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward - TheTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length()))
		{
			return false;
		}
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsRoof(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope HeightFromRoofRange, bool bIsAnyRequireLedge, FGBWFlowMoveFloatScope RoofLedgeRange,
	bool bIsRequireForwardLedge, FGBWFlowMoveFloatScope ForwardRoofLedgeRange, bool bIsRequireBackwardLedge,
	FGBWFlowMoveFloatScope BackwardRoofLedgeRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsRoof(AnimInstance->GetOwningActor(),
			TargetScene,
			HeightFromRoofRange,
			bIsAnyRequireLedge,
			RoofLedgeRange,
			bIsRequireForwardLedge,
			ForwardRoofLedgeRange,
			bIsRequireBackwardLedge,
			BackwardRoofLedgeRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsSlope(AActor* InActor, FGameplayTag TargetScene,
                                     FGBWFlowMoveFloatScope SlopeRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FGBWFlowMoveScene TheTargetScene;
		if (!FMComp->GetScene(TheTargetScene,TargetScene,false))
		{
			return false;
		}
		
		return SlopeRange.InScope(TheTargetScene.Slope);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsSlope(const UAnimInstance* AnimInstance, FGameplayTag TargetScene,
	FGBWFlowMoveFloatScope SlopeRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsSlope(
			AnimInstance->GetOwningActor(),TargetScene,SlopeRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsMoveVector(AActor* InActor,
                                          FFlowMoveMoveVectorCondition MoveVectorRange)
{
	if (const UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return MoveVectorRange.IsMet(FMComp->TaskState.MoveVector);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsMoveVector(const UAnimInstance* AnimInstance,
                                               const FFlowMoveMoveVectorCondition& MoveVectorRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsMoveVector(
			AnimInstance->GetOwningActor(),MoveVectorRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsHasMoveVector(AActor* InActor)
{
	FFlowMoveMoveVectorCondition MoveVectorRange = FFlowMoveMoveVectorCondition();
	MoveVectorRange.ConditionType = EInputValueConditionType::HasValue;
	return UGBWFlowMoveFuncLib::FM_IsMoveVector(InActor,MoveVectorRange);
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsHasMoveVector(const UAnimInstance* AnimInstance)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsHasMoveVector(
			AnimInstance->GetOwningActor());
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsLastActionTag(AActor* InActor, FGameplayTagQuery LastActionCondition)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return FMComp->TaskState.CurrentFlowMoveActionTag.IsValid()
				&& LastActionCondition.Matches(FMComp->TaskState.LastFlowMoveActionTag.GetSingleTagContainer());
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsLastActionTag(const UAnimInstance* AnimInstance,
                                                  const FGameplayTagQuery& LastActionCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsLastActionTag(
			AnimInstance->GetOwningActor(),LastActionCondition);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsCurrentActionTag(AActor* InActor,
                                                FGameplayTagQuery CurrentActionCondition,
                                                float MinTimeCondition,
                                                float MaxTimeCondition)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		if (!FMComp->TaskState.CurrentFlowMoveActionTag.IsValid())
		{
			return false;
		}

		if (!CurrentActionCondition.Matches(FMComp->TaskState.CurrentFlowMoveActionTag.GetSingleTagContainer()))
		{
			return false;
		}

		if (FMComp->TaskState.CurrentFlowMoveExecutedTime <  MinTimeCondition)
		{
			return false;
		}

		if (MaxTimeCondition > 0.0f
			&& FMComp->TaskState.CurrentFlowMoveExecutedTime > MaxTimeCondition)
		{
			return false;
		}
		
		return true;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsCurrentActionTag(
	const UAnimInstance* AnimInstance,
	const FGameplayTagQuery& CurrentActionCondition,
	float MinTimeCondition,
	float MaxTimeCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsCurrentActionTag(
			AnimInstance->GetOwningActor(),CurrentActionCondition,MinTimeCondition,MaxTimeCondition);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsCurrentActionTagAndActionLock(AActor* InActor, FGameplayTagQuery CurrentActionCondition,
float MinTimeCondition, float MaxTimeCondition, bool IsRequireNoActionLock,
	bool IsNegateActionLock,
	FGameplayTagContainer ActionLockCondition)
{
	bool Res = UGBWFlowMoveFuncLib::FM_IsCurrentActionTag(InActor,
												CurrentActionCondition,
												MinTimeCondition,
												MaxTimeCondition);
	if (Res)
	{
		if(IsRequireNoActionLock)
		{
			Res = !UGBWFlowMoveFuncLib::FM_IsAnyActionLock(InActor);
		}
		else
		{
			Res = UGBWFlowMoveFuncLib::FM_IsActionLock(InActor,ActionLockCondition);
			if (IsNegateActionLock)
			{
				Res = !Res;
			}
		}
	}

	return Res;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsCurrentActionTagAndActionLock(const UAnimInstance* AnimInstance,
	const FGameplayTagQuery& CurrentActionCondition, float MinTimeCondition, float MaxTimeCondition,
	bool IsRequireNoActionLock, bool IsNegateActionLock, FGameplayTagContainer ActionLockCondition)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsCurrentActionTagAndActionLock(
			AnimInstance->GetOwningActor(),
			CurrentActionCondition,
			MinTimeCondition,
			MaxTimeCondition,
			IsRequireNoActionLock,
			IsNegateActionLock,
			ActionLockCondition);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsFlowMoveSystemEvent(AActor* InActor, FFlowMoveEvent FlowMoveSystemEvent)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		return FMComp->TaskState.IsHasFlowMoveEvent(FlowMoveSystemEvent);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsCharacterSpeed(
	AActor* InActor,
	const bool OnlyForHorizontalSpeed,
	const bool OnlyForMoveToDirectionSpeed,
	FGBWFlowMoveFloatScope CharacterSpeedRange)
{
	if (const UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FVector Velocity = FMComp->TaskState.RealVelocity;
		if (OnlyForHorizontalSpeed)
		{
			Velocity.Z = 0.0f;
		}
		float Speed = Velocity.Length();
		if (OnlyForMoveToDirectionSpeed)
		{
			Speed = UKismetMathLibrary::Dot_VectorVector(Velocity,FMComp->TaskState.MoveControlParam.MoveToDirection);
		}
		return CharacterSpeedRange.InScope(Speed);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsCharacterSpeed(
	const UAnimInstance* AnimInstance,
	bool OnlyForHorizontalSpeed,
	bool OnlyForMoveToDirectionSpeed,
	FGBWFlowMoveFloatScope CharacterSpeedRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsCharacterSpeed(
			AnimInstance->GetOwningActor(),
			OnlyForHorizontalSpeed,
			OnlyForMoveToDirectionSpeed,
			CharacterSpeedRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsViewMode(AActor* InActor, TEnumAsByte<EInclusivityType> ConditionType,
                                        TSet<TEnumAsByte<EFlowMoveCharacterViewMode>> ViewModeSet)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		if (ConditionType == EInclusivityType::Include_)
		{
			return ViewModeSet.Contains(FMComp->TaskState.ViewMode);
		}
		else if (ConditionType == EInclusivityType::NotInclude_)
		{
			return !ViewModeSet.Contains(FMComp->TaskState.ViewMode);
		}

		return false;
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsViewMode(const UAnimInstance* AnimInstance,
	TEnumAsByte<EInclusivityType> ConditionType, TSet<TEnumAsByte<EFlowMoveCharacterViewMode>> ViewModeSet)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsViewMode(
			AnimInstance->GetOwningActor(),ConditionType,ViewModeSet);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsAngle(AActor* InActor,
                                     EFlowMoveDirectionType From,
                                     EFlowMoveDirectionType To,
                                     FGBWFlowMoveFloatScope AngleRange)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		const FVector FromVector = FMComp->GetDirectionVector(From);
		const FVector ToVector = FMComp->GetDirectionVector(To);
		
		FTransform T = InActor->GetActorTransform();
		T.SetRotation(FromVector.Rotation().Quaternion());
	
		FVector A = UKismetMathLibrary::ProjectVectorOnToPlane(
			ToVector,
			InActor->GetActorUpVector());
		A.Normalize();
		FVector B = UKismetMathLibrary::ProjectVectorOnToPlane(
			FromVector,
			InActor->GetActorUpVector());
		B.Normalize();
		const float DirectionAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(A,B));

		const FVector A2 = UKismetMathLibrary::InverseTransformDirection(T,A);
		const FVector B2 = UKismetMathLibrary::InverseTransformDirection(T,B);
		const bool bIsRight = UKismetMathLibrary::CrossProduct2D(
			FVector2D(A2.X, A2.Y),
			FVector2D(B2.X, B2.Y)
		)<=0;

		const float SceneDirection = DirectionAngle * (bIsRight? 1 : -1);

		return AngleRange.InScope(SceneDirection);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_Pure_IsAngle(const UAnimInstance* AnimInstance, EFlowMoveDirectionType From,
	EFlowMoveDirectionType To, FGBWFlowMoveFloatScope AngleRange)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		return UGBWFlowMoveFuncLib::FM_IsAngle(
			AnimInstance->GetOwningActor(),From,To,AngleRange);
	}

	return false;
}

bool UGBWFlowMoveFuncLib::FM_IsFlowMoveActive(AActor* InActor, bool& IsStopping)
{
	if (const UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
    {
        IsStopping = FMComp->IsFlowMoveStopping();
        return FMComp->IsFlowMoveActive();
    }

    return false;
}

void UGBWFlowMoveFuncLib::FM_Pure_IsFlowMoveActive(const UAnimInstance* AnimInstance, bool& IsActive, bool& IsStopping)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		IsActive = UGBWFlowMoveFuncLib::FM_IsFlowMoveActive(
			AnimInstance->GetOwningActor(),IsStopping);
		return;
	}

	IsActive = false;
}

bool UGBWFlowMoveFuncLib::FM_GetMoveControlParam(AActor* InActor, FGBWFlowMoveControlParam& MoveControlParam)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		MoveControlParam = FMComp->GetMoveControlParam();
		return true;
	}

	return false;
}

void UGBWFlowMoveFuncLib::FM_GetMoveParamForBlendSpace(
	AActor* InActor,
	UBlendSpace* ForBS, float& Direction,
	float& Speed, float& PlayRate)
{
	PlayRate = 1.0f;
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->GetMoveParamForBlendSpace(ForBS,Direction,Speed,PlayRate);
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_GetMoveParamForBlendSpace(
	const UAnimInstance* AnimInstance,
	UBlendSpace* ForBS,
	float& Direction,
	float& Speed,
	float& PlayRate)
{
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWFlowMoveFuncLib::FM_GetMoveParamForBlendSpace(
			AnimInstance->GetOwningActor(),
			ForBS,
			Direction,
			Speed,
			PlayRate);
	}
}

void UGBWFlowMoveFuncLib::FM_GetAnimExpectedPlayRate(const AActor* InActor, float& PlayRate)
{
	PlayRate = 1.0f;
	if (const UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		PlayRate = FMComp->TaskState.MoveControlParam.AnimPlayRateNow;;
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_GetAnimExpectedPlayRate(const UAnimInstance* AnimInstance, float& PlayRate)
{
	PlayRate = 1.0f;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		UGBWFlowMoveFuncLib::FM_GetAnimExpectedPlayRate(
			AnimInstance->GetOwningActor(),
			PlayRate);
	}
}

void UGBWFlowMoveFuncLib::FM_SetCurrentTargetScene(AActor* InActor, FGameplayTag ActionTargetScene)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetCurrentTargetScene(ActionTargetScene);
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_SetCurrentTargetScene(
	bool InCondition,
	bool ExecuteWhenNotMeetCondition,
	const UAnimInstance* AnimInstance,
	const FGameplayTag& ActionTargetScene,
	bool& OutCondition)
{
	OutCondition =InCondition;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		if ((!ExecuteWhenNotMeetCondition && InCondition)
			|| (ExecuteWhenNotMeetCondition && !InCondition))
		{
			UGBWFlowMoveFuncLib::FM_SetCurrentTargetScene(
			AnimInstance->GetOwningActor(),
			ActionTargetScene);
		}
	}
}

void UGBWFlowMoveFuncLib::FM_SetCurrentActionTag(AActor* InActor, FGameplayTag ActionTag)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetCurrentActionTag(ActionTag);
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_SetCurrentActionTag(bool InCondition, bool ExecuteWhenNotMeetCondition,
	const UAnimInstance* AnimInstance, const FGameplayTag& ActionTag, bool& OutCondition)
{
	OutCondition =InCondition;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		if ((!ExecuteWhenNotMeetCondition && InCondition)
			|| (ExecuteWhenNotMeetCondition && !InCondition))
		{
			UGBWFlowMoveFuncLib::FM_SetCurrentActionTag(
			AnimInstance->GetOwningActor(),
			ActionTag);
		}
	}
}

void UGBWFlowMoveFuncLib::FM_ClearCurrentActionTag(AActor* InActor)
{
	if (UGBWFlowMoveComponent* FMComp = FM_GetFlowMoveComponent(InActor))
	{
		FMComp->SetCurrentActionTag(FGameplayTag::EmptyTag);
	}
}

void UGBWFlowMoveFuncLib::FM_Pure_ClearCurrentActionTag(bool InCondition, bool ExecuteWhenNotMeetCondition,
	const UAnimInstance* AnimInstance, bool& OutCondition)
{
	OutCondition =InCondition;
	if (AnimInstance && AnimInstance->GetOwningActor())
	{
		if ((!ExecuteWhenNotMeetCondition && InCondition)
			|| (ExecuteWhenNotMeetCondition && !InCondition))
		{
			UGBWFlowMoveFuncLib::FM_ClearCurrentActionTag(
			AnimInstance->GetOwningActor());
		}
	}
}

void UGBWFlowMoveFuncLib::DrawDebugShape(
	const UObject* WorldContext,
	const FVector Location,
	const FQuat Rotation,
	const FCollisionShape CollisionShape,
	const FColor Color,
	float Duration, 
	float Thickness)
{
	if (CollisionShape.IsBox())
	{
		DrawDebugBox(
			WorldContext->GetWorld(),
			Location,
			CollisionShape.GetExtent(),
			Color,
			false,
			Duration,
			0,
			Thickness
		);
	}

	if (CollisionShape.IsCapsule())
	{
		DrawDebugCapsule(
			WorldContext->GetWorld(),
			Location,
			CollisionShape.GetCapsuleHalfHeight(),
			CollisionShape.GetCapsuleRadius(),
			Rotation,
			Color,
			false,
			Duration,
			0,
			Thickness
		);
	}

	if (CollisionShape.IsSphere())
	{
		DrawDebugSphere(
			WorldContext->GetWorld(),
			Location,
			CollisionShape.GetSphereRadius(),
			8,
			Color,
			false,
			Duration,
			0,
			Thickness
		);
	}
}

void UGBWFlowMoveFuncLib::PrintDebugString(TArray<FString> DebugStrArr, const FName Key, const FColor Color, const float Duration)
{
	uint8 Index = 0;
	for (auto S:DebugStrArr)
	{
		Index++;
		uint64 InnerKey = -1;
		if (Key != NAME_None)
		{
			InnerKey = GetTypeHash(FName(Key.ToString().Append(FString::SanitizeFloat(Index))));
		}
		GEngine->AddOnScreenDebugMessage(
			InnerKey,Duration,Color,S
		);
	}
}

void UGBWFlowMoveFuncLib::PrintDebugStringNet(
	const AActor* WorldActor, TArray<FString> DebugStrArr, FName Key,
	const FColor ServerColor, const FColor ClientColor, float Duration)
{
	if (!WorldActor)
	{
		return;
	}
	const bool bIsServer = WorldActor->HasAuthority();

	PrintDebugString(DebugStrArr,
	FName(Key.IsValid()?
	(bIsServer? Key.ToString().Append("_Server") : Key.ToString().Append("_Client"))
	: (bIsServer? FString("_Server") : FString("_Client"))),
		bIsServer? ServerColor : ClientColor,
		Duration
		);
}
