// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "Expand/GBWFlowMoveObjectExpand.h"

#include "GBWFlowMoveComponent.h"
#include "GBWFlowMoveFuncLib.h"
#include "GBWFlowMoveInterface.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "RMS/GBWRMSBPFuncLib.h"
#include "DrawDebugHelpers.h"

UGBWFlowMovePerceptron_Default::UGBWFlowMovePerceptron_Default():Super()
{
	ThePerceptionSetting.TraceSetting.TraceObjectTypes = {
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	ThePerceptionSetting.TraceSetting.TraceChannels = {
		UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility)
	};
}

bool UGBWFlowMovePerceptron_Default::GetPerceptionResult_Implementation(FGBWFlowMoveScene& FlowMoveScene,
	EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent)
	{
		const bool bRes = FindWay(
			FlowMoveSceneCache,
			OwnerCharacter,
			FlowMoveComponent,
			ThePerceptionSetting
		);
		FlowMoveScene = FlowMoveSceneCache;
		return bRes;
	}
	return false;
}

bool UGBWFlowMovePerceptron_Default::FindWay(
	FGBWFlowMoveScene& FlowMoveScene,
	ACharacter* Character,
	UGBWFlowMoveComponent* FlowMoveComponent,
	FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const
{
	if (!Character || !FlowMoveComponent)
	{
		return false;
	}

	FName Res_SceneType = NAME_None;
	FVector Res_TargetLocation = FVector::ZeroVector;
	AActor* Res_TargetActor = nullptr;
	float Res_Slope = 0.0f;
	
	FVector MoveVector = FlowMoveComponent->TaskState.MoveVector;
	FVector ControlVector = FlowMoveComponent->GetControlVector();
	ControlVector.Z = 0.0f;
	ControlVector.Normalize();
	FVector TraceDirection = ControlVector.Rotation().RotateVector(MoveVector);
	TraceDirection.Normalize();
	if (TraceDirection.IsNearlyZero())
	{
		TraceDirection = Character->GetCapsuleComponent()->GetForwardVector();
	}
	
	FVector PerceptionDirection = (FMath::Abs(TraceDirection.Rotation().Yaw-ControlVector.Rotation().Yaw)<=30)? FlowMoveComponent->GetPerceptionVector() : FVector::Zero();
	PerceptionDirection.Normalize();

	bool bIsForwardHit = false;
	bool bIsFindWay = false;
	bool bIsLedgeDownHit = false;
	bool bIsFree = false;
	FHitResult InitHitResult;
	FHitResult WayHitResult;
	
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);
	
	FVector ForwardTrace_Start = Character->GetCapsuleComponent()->GetComponentLocation();
	FVector ForwardTrace_End =
		TraceDirection * PerceptionSetting.PrejudgeDistance
		+ ForwardTrace_Start;
	
	Res_SceneType = PerceptionSetting.FreeTag;
	Res_TargetLocation = ForwardTrace_End;

	FHitResult ForwardTraceHit;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		ForwardTraceHitArr,
		ForwardTrace_Start - TraceDirection * Character->GetCapsuleComponent()->GetScaledCapsuleRadius() * 0.5f,
		ForwardTrace_End,
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeCapsule(Character->GetCapsuleComponent()->GetScaledCapsuleRadius(), Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		CollisionQueryParams
	);
	bIsForwardHit = ForwardTraceHit.IsValidBlockingHit();
	InitHitResult = ForwardTraceHit;
	
	FVector LeftTrace_End =
		FRotator(0.0f,-90.0f,0.0f).RotateVector(TraceDirection) * PerceptionSetting.PrejudgeDistance
		+ ForwardTrace_Start;
	FHitResult LeftTraceHit;
	static const FName LeftTraceTag{FString::Format(TEXT("{0} (Left Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = LeftTraceTag;
	TArray<struct FHitResult> LeftTraceHitArr;
	LeftTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		LeftTraceHitArr,
		ForwardTrace_Start,
		LeftTrace_End,
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeCapsule(Character->GetCapsuleComponent()->GetScaledCapsuleRadius()/2.0f, Character->GetCapsuleComponent()->GetScaledCapsuleRadius()/2.0f),
		CollisionQueryParams
	);
	/*if (LeftTraceHit.IsValidBlockingHit())
	{
		DrawDebugDirectionalArrow(Character->GetWorld(),
			LeftTraceHit.ImpactPoint,
			LeftTraceHit.ImpactPoint + LeftTraceHit.ImpactNormal * 100.0f,
			5.0f,
			FColor::Purple,
			false,
			-1,
			0,
			3.0f);
	}*/
	
	FVector RightTrace_End =
		FRotator(0.0f,90.f,0.0f).RotateVector(TraceDirection) * PerceptionSetting.PrejudgeDistance
		+ ForwardTrace_Start;
	FHitResult RightTraceHit;
	static const FName RightTraceTag{FString::Format(TEXT("{0} (Right Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = RightTraceTag;
	TArray<struct FHitResult> RightTraceHitArr;
	RightTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		RightTraceHitArr,
		ForwardTrace_Start,
		RightTrace_End,
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeCapsule(Character->GetCapsuleComponent()->GetScaledCapsuleRadius()/2.0f, Character->GetCapsuleComponent()->GetScaledCapsuleRadius()/2.0f),
		CollisionQueryParams
	);
	/*if (RightTraceHit.IsValidBlockingHit())
	{
		DrawDebugDirectionalArrow(Character->GetWorld(),
			RightTraceHit.ImpactPoint,
			RightTraceHit.ImpactPoint + RightTraceHit.ImpactNormal * 100.0f,
			5.0f,
			FColor::Purple,
			false,
			-1,
			0,
			3.0f);
	}*/

	if (bIsForwardHit)
	{
		FTransform TraceBaseTransform = Character->GetCapsuleComponent()->GetComponentTransform();
		TraceBaseTransform.SetRotation(TraceDirection.Rotation().Quaternion());
		
		if (!Character->GetCharacterMovement()->IsWalkable(ForwardTraceHit))
		{
			TArray<FVector> InputVectorInterp = {};
			if (PerceptionSetting.bUseTraceDirection)
			{
				InputVectorInterp = GenerateCrawlerDirection(PerceptionSetting.DirectiveCoverageAngle,PerceptionDirection,PerceptionSetting.CrawlerCount);
			}
			else
			{
				InputVectorInterp = {FVector(0.0f,0.0f,1.0f)};
			}
			
			TArray<FHitResult> LastHitResultInterp;
			TArray<bool> IsTraceOutOfDistance;
			for (int i=0; i<InputVectorInterp.Num(); i++)
			{
				LastHitResultInterp.Add(InitHitResult);
				IsTraceOutOfDistance.Add(false);
			}
			
			for (int j=0; j<InputVectorInterp.Num(); j++)
			{
				bool bIsBreak = false;
				for (int i=0; i<PerceptionSetting.MaxCrawlerStep; i++)
				{
					if (!IsTraceOutOfDistance.Contains(false))
					{
						bIsBreak = true;
						break;
					}

					if ((LastHitResultInterp[j].TraceStart-InitHitResult.TraceStart).Length()>PerceptionSetting.CrawlerMaxTraceDistance)
					{
						IsTraceOutOfDistance[j] = true;
					}
					else
					{
						FVector Offset =
							UKismetMathLibrary::TransformDirection(
								TraceBaseTransform,
								FVector(0,InputVectorInterp[j].Y, InputVectorInterp[j].Z))
								* Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
						FHitResult TryHitResult;
						FHitResult SuccessHitResult;
						if (TestWay(
							TryHitResult,
							SuccessHitResult,
							Character,
							LastHitResultInterp[j].TraceStart + Offset,
							LastHitResultInterp[j].TraceEnd + Offset,
							PerceptionSetting.TraceDetectorHeight,
							PerceptionSetting
						))
						{
							bIsBreak = true;
							bIsFindWay = true;
							WayHitResult = SuccessHitResult;
							break;
						}
						else
						{
							LastHitResultInterp[j] = TryHitResult;
						}
					}
				}
				if (bIsBreak)
				{
					break;
				}
			}
		}
		else
		{
			FHitResult LastHitResult = InitHitResult;
			FVector Offset =
						UKismetMathLibrary::TransformDirection(
							TraceBaseTransform,
							FVector(0,0, 1))
							* Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		
			for (int i=0; i<PerceptionSetting.MaxCrawlerStep; i++)
			{
				if ((LastHitResult.TraceStart-InitHitResult.TraceStart).Length()>PerceptionSetting.CrawlerMaxTraceDistance)
				{
					break;
				}
				else
				{
					FHitResult TryHitResult;
					FHitResult SuccessHitResult;
					if (TestWay(
						TryHitResult,
						SuccessHitResult,
						Character,
						LastHitResult.TraceStart + Offset,
						LastHitResult.TraceEnd + Offset,
						PerceptionSetting.TraceDetectorHeight,
						PerceptionSetting
					))
					{
						bIsFindWay = true;
						WayHitResult = SuccessHitResult;
						break;
					}
					else
					{
						LastHitResult = TryHitResult;
					}
				}
			}
		}
	}
	else if (
	!bIsForwardHit
	|| (ForwardTraceHit.ImpactPoint-Character->GetCapsuleComponent()->GetComponentLocation()).Length() <= PerceptionSetting.BeginLedgeDownForwardTraceDistanceThreshold
	)
	{
		ForwardTrace_End =
			ForwardTrace_Start
			- FVector(
				0,0,
				Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - Character->GetCapsuleComponent()->GetScaledCapsuleRadius() + Character->GetCharacterMovement()->MaxStepHeight);
		ForwardTrace_Start =
			ForwardTrace_End
			+ (PerceptionSetting.PrejudgeDistance + Character->GetCapsuleComponent()->GetScaledCapsuleRadius()) * TraceDirection;

		ForwardTraceHitArr.Empty();
		ForwardTraceHit = UGBWFlowMoveFuncLib::LineTrace(
			Character->GetWorld(),
			ForwardTraceHitArr,
			Character->GetCapsuleComponent()->GetComponentLocation(),
			ForwardTrace_Start,
			PerceptionSetting.TraceSetting,
			CollisionQueryParams
		);

		if (!ForwardTraceHit.IsValidBlockingHit())
		{
			ForwardTraceHitArr.Empty();
			ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
				Character->GetWorld(),
				ForwardTraceHitArr,
				ForwardTrace_Start,
				ForwardTrace_End,
				FQuat::Identity,
				PerceptionSetting.TraceSetting,
				FCollisionShape::MakeSphere(Character->GetCapsuleComponent()->GetScaledCapsuleRadius()),
				CollisionQueryParams
			);

			bIsForwardHit = ForwardTraceHit.IsValidBlockingHit();

			if (bIsForwardHit)
			{
				bIsFindWay = true;
				WayHitResult = InitHitResult;
				InitHitResult = ForwardTraceHit;
				bIsLedgeDownHit = true;
			}
		
			InitHitResult = ForwardTraceHit;
		}
		else
		{
			bIsFree = true;
		}
	}
	else
	{
		bIsFree = true;
	}

	float Slope = 0.0f;
	if (bIsForwardHit && !bIsFree)
	{
		if (!bIsFindWay)
		{
			Res_SceneType = PerceptionSetting.HitWallTag;
			Res_TargetActor = InitHitResult.GetActor();
			Res_TargetLocation = InitHitResult.ImpactPoint;
		}
		else
		{
			TArray<FHitResult> WayHitResultDown;
			TArray<FHitResult> WayHitResultUp;
			float MaxHeightHitResult_High;
			FHitResult MaxHeightHitResult_HighResult;
			float MaxHeightHitResult_Low;
			FHitResult MaxHeightHitResult_LowResult;
			float MinHeightHitResult_Low;
			FHitResult MinHeightHitResult_LowResult;
			float WayMinHeight;
			FHitResult MinZoneHitResult_DownResult;
			FHitResult MinZoneHitResult_UpResult;
			
			FindBottleneck(
				Character,
				 WayHitResult.TraceStart,
				 WayHitResult.TraceEnd,
				 PerceptionSetting,
				WayHitResultDown, 
				WayHitResultUp,
				MaxHeightHitResult_High,
				MaxHeightHitResult_HighResult,
				MaxHeightHitResult_Low,
				MaxHeightHitResult_LowResult,
				MinHeightHitResult_Low,
				MinHeightHitResult_LowResult,
				WayMinHeight,
				MinZoneHitResult_DownResult,
				MinZoneHitResult_UpResult);
			
			if (WayMinHeight<=0 || WayMinHeight >= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2)
			{
				TArray<FHitResult> WayHitResultDownArr;

				FHitResult WayHitResultDown_Temp;
				UGBWFlowMoveFuncLib::GetTheLastValidHitResult(WayHitResultDown,WayHitResultDown_Temp);
				FHitResult WayHitResult_Temp;
				bool bIsBarrierFree = TestBarrierFree(
					Character,
					WayHitResultDown_Temp,
					WayHitResult_Temp,
					WayHitResultDownArr,
					PerceptionSetting.TraceDetectorHeight,
					PerceptionSetting);
				
				if (!bIsBarrierFree)
				{
					WayHitResultDownArr = WayHitResultDown;
				}
				else
				{
					WayHitResult = WayHitResult_Temp;
				}
				
				bool IsSlopeWalkable = false;
				Slope = GetSlope(Character,WayHitResultDownArr,IsSlopeWalkable);

				if (IsSlopeWalkable)
				{
					Res_SceneType = PerceptionSetting.FreeTag;
				}
				else
				{
					Res_SceneType = PerceptionSetting.LedgeTag;
				}

				if (bIsLedgeDownHit)
				{
					if (MinHeightHitResult_Low>=0)
					{
						Res_TargetActor = MinHeightHitResult_LowResult.GetActor();
					//	FlowMoveScene.TargetPrimitive = MinHeightHitResult_LowResult.GetComponent();
						Res_TargetLocation = MinHeightHitResult_LowResult.ImpactPoint + FVector(0,0,Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
					}
					else if (MaxHeightHitResult_Low >= 0)
					{
						Res_TargetActor = MaxHeightHitResult_LowResult.GetActor();
					//	FlowMoveScene.TargetPrimitive = MaxHeightHitResult_LowResult.GetComponent();
						Res_TargetLocation = MaxHeightHitResult_LowResult.ImpactPoint + FVector(0,0,Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
					}
				}
				else
				{
					if (MaxHeightHitResult_High>=0)
					{
						Res_TargetActor = MaxHeightHitResult_HighResult.GetActor();
					//	FlowMoveScene.TargetPrimitive = MaxHeightHitResult_HighResult.GetComponent();
						Res_TargetLocation = MaxHeightHitResult_HighResult.ImpactPoint + FVector(0,0,Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
					}
					else if (MaxHeightHitResult_Low >= 0)
					{
						Res_TargetActor = MaxHeightHitResult_LowResult.GetActor();
					//	FlowMoveScene.TargetPrimitive = MaxHeightHitResult_LowResult.GetComponent();
						Res_TargetLocation = MaxHeightHitResult_LowResult.ImpactPoint + FVector(0,0,Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
					}
				}
			}
			else
			{
				Res_SceneType = PerceptionSetting.HoleTag;
				Res_TargetActor = MinZoneHitResult_DownResult.GetActor();
			//	FlowMoveScene.TargetPrimitive = MinZoneHitResult_DownResult.GetComponent();
				Res_TargetLocation = (MinZoneHitResult_DownResult.ImpactPoint + MinZoneHitResult_UpResult.ImpactPoint) / 2.0f;
			}
		}
	}

	Res_Slope = UKismetMathLibrary::FInterpTo(
					FlowMoveScene.Slope,
					Slope,
					FlowMoveComponent->TaskState.FrameDeltaTime,
					ThePerceptionSetting.SlopeSensitivity);
	
	FlowMoveScene = FGBWFlowMoveScene(
		Character,
		Res_SceneType,
		Res_TargetActor,
		Res_Slope,
		Res_TargetLocation,
		LeftTraceHit.ImpactPoint,
		RightTraceHit.ImpactPoint,
		PerceptionSetting.TraceSetting,
		ThePerceptionSetting.FloorAndRoofLedgeTraceDistance);
	return true;
}

TArray<FVector> UGBWFlowMovePerceptron_Default::GenerateCrawlerDirection(
	const float TraceAngle,
	const FVector TraceVector,
	const int InterpCount)
{
	TArray<FVector> Result;
	
	if (TraceVector.IsNearlyZero())
	{
		Result.Add(FVector(0,0,1));
		const int LoopCount = 360.0f/TraceAngle*InterpCount;
		for (int i=1; i<=LoopCount; i++)
		{
			Result.Add(UKismetMathLibrary::RotateAngleAxis(
				FVector(0,0,1),
				TraceAngle/InterpCount*i,
				FVector(1,0,0)
			));
			Result.Add(UKismetMathLibrary::RotateAngleAxis(
				FVector(0,0,1),
				-TraceAngle/InterpCount*i,
				FVector(1,0,0)
			));
		}
	}
	else
	{
		Result.Add(TraceVector);
		for (int i=1; i<=InterpCount; i++)
		{
			Result.Add(UKismetMathLibrary::RotateAngleAxis(
				TraceVector,
				TraceAngle/2/i,
				FVector(1,0,0)
			));
			Result.Add(UKismetMathLibrary::RotateAngleAxis(
				TraceVector,
				-TraceAngle/2/i,
				FVector(1,0,0)
			));
		}
	}

	return Result;
}

bool UGBWFlowMovePerceptron_Default::TestWay(
	FHitResult& TryHitResult,
	FHitResult& SuccessHitResult,
	ACharacter* Character,
	FVector Start,
	FVector End,
	float TraceHeight,
	FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const
{
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);

	FHitResult ForwardTraceHit;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	ForwardTraceHit = UGBWFlowMoveFuncLib::LineTrace(
		Character->GetWorld(),
		ForwardTraceHitArr,
		Character->GetCapsuleComponent()->GetComponentLocation(),
		Start,
		PerceptionSetting.TraceSetting,
		CollisionQueryParams
	);

	if (ForwardTraceHit.IsValidBlockingHit())
	{
		return false;
	}

	ForwardTraceHitArr.Empty();
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		ForwardTraceHitArr,
		Start,
		End,
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeSphere(Character->GetCapsuleComponent()->GetScaledCapsuleRadius()),
		CollisionQueryParams
	);

	TryHitResult = ForwardTraceHit;
	if (ForwardTraceHit.bBlockingHit || ForwardTraceHit.bStartPenetrating)
	{
		return false;
	}

	float FinalTraceHalfHeight = TraceHeight>Character->GetCapsuleComponent()->GetScaledCapsuleRadius()*2? TraceHeight/2 : Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float FinalTraceZOffset = FinalTraceHalfHeight - Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	ForwardTraceHitArr.Empty();
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		ForwardTraceHitArr,
		Start + FVector(0,0, FinalTraceZOffset),
		End + FVector(0,0, FinalTraceZOffset),
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeCapsule(
			Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
			FinalTraceHalfHeight),
			CollisionQueryParams
	);

	if (ForwardTraceHit.bBlockingHit || ForwardTraceHit.bStartPenetrating)
	{
		TryHitResult = ForwardTraceHit;
		return false;
	}
	
	SuccessHitResult = ForwardTraceHit;
	return true;
}

void UGBWFlowMovePerceptron_Default::FindBottleneck(
	ACharacter* Character,
	FVector Start,
	FVector End,
	FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting,
	TArray<FHitResult>& WayHitResultDown, 
	TArray<FHitResult>& WayHitResultUp,
	float& MaxHeightHitResult_High,
	FHitResult& MaxHeightHitResult_HighResult,
	float& MaxHeightHitResult_Low,
	FHitResult& MaxHeightHitResult_LowResult,
	float& MinHeightHitResult_Low,
	FHitResult& MinHeightHitResult_LowResult,
	float& WayMinHeight,
	FHitResult& MinZoneHitResult_DownResult,
	FHitResult& MinZoneHitResult_UpResult) const
{
	WayMinHeight = -1.0f;
	MaxHeightHitResult_High = -1.0f;
	MaxHeightHitResult_Low = -1.0f;
	MinHeightHitResult_Low = -1.0f;

	float FootHeight = Character->GetCapsuleComponent()->GetComponentLocation().Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector TraceLocation;
	float StepCount = (Start-End).Length() / Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	for (int i=0; i<=StepCount; i++)
	{
		TraceLocation = UKismetMathLibrary::VLerp(Start, End, i/StepCount);
		bool Floor; FHitResult OutHit_Floor; bool Roof; FHitResult OutHit_Roof;
		FGBWFlowMoveFloorRoofScene FloorAndRoofRes;
		UGBWFlowMoveFuncLib::FindFloorAndRoof(Character, TraceLocation, 10000.0f, 10000.0f, false, 0.0f, PerceptionSetting.TraceSetting,
			Floor, OutHit_Floor, Roof, OutHit_Roof,FloorAndRoofRes);

		WayHitResultDown.Add(OutHit_Floor);
		WayHitResultUp.Add(OutHit_Roof);

		if (Floor)
		{
			float Height = OutHit_Floor.ImpactPoint.Z - FootHeight;
			if (Height >= 0)
			{
				if (Height >= MaxHeightHitResult_High - 5)
				{
					MaxHeightHitResult_High = Height;
					MaxHeightHitResult_HighResult = OutHit_Floor;
				}
			}
			else
			{
				if (-Height <= MaxHeightHitResult_Low+5.0f || MaxHeightHitResult_Low < 0)
				{
					MaxHeightHitResult_Low = -Height;
					MaxHeightHitResult_LowResult = OutHit_Floor;
				}

				if (-Height >= MinHeightHitResult_Low)
				{
					MinHeightHitResult_Low = -Height;
					MinHeightHitResult_LowResult = OutHit_Floor;
				}
			}
		}

		if (Floor && Roof)
		{
			float WayHeight = (OutHit_Roof.ImpactPoint-OutHit_Floor.ImpactPoint).Z;
			if (WayMinHeight <= 0 || (WayMinHeight>0 && WayMinHeight >= WayHeight))
			{
				WayMinHeight = WayHeight;
				MinZoneHitResult_DownResult = OutHit_Floor;
				MinZoneHitResult_UpResult = OutHit_Roof;
			}
		}
	}
}

float UGBWFlowMovePerceptron_Default::GetSlope(
	const ACharacter* Character,
	TArray<FHitResult> HitResultArr,
	bool& bIsWalkable)
{
	if (!Character || HitResultArr.IsEmpty())
	{
		bIsWalkable = false;
		return 0.0f;
	}
	
	bIsWalkable = true;

	FVector FootLocation = Character->GetCapsuleComponent()->GetComponentLocation();
	FootLocation.Z = FootLocation.Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float SlopeRes = 0.0f;
	float SlopeMax = 0.0f;
	float SlopeMin = 0.0f;
	
	FVector LastLocation = HitResultArr[0].ImpactPoint;

	float HeightDistance = FMath::Abs(LastLocation.Z - FootLocation.Z);
	float StandardDistance = (FVector(LastLocation.X, LastLocation.Y, 0) - FVector(FootLocation.X, FootLocation.Y, 0)).Length();
		
	if (HeightDistance > Character->GetCharacterMovement()->MaxStepHeight
		|| StandardDistance > Character->GetCharacterMovement()->MaxStepHeight*2)
	{
		bIsWalkable = false;
	}

	TArray<float> Slopes = {};
	for (int i=1; i<HitResultArr.Num()-1; i++)
	{
		FHitResult Hit = HitResultArr[i];
		if (!Character->GetCharacterMovement()->IsWalkable(Hit))
		{
			bIsWalkable = false;
		}

		if (Hit.IsValidBlockingHit())
		{
			FVector ImpactLocation_Temp = Hit.ImpactPoint;
			FVector LastLocation_Temp = LastLocation;
			HeightDistance = FMath::Abs((Hit.ImpactPoint - LastLocation).Z);
			StandardDistance =
				(FVector(ImpactLocation_Temp.X, ImpactLocation_Temp.Y, 0) - FVector(LastLocation_Temp.X, LastLocation_Temp.Y, 0)).Length();
		
			if (HeightDistance > Character->GetCharacterMovement()->MaxStepHeight
				|| StandardDistance > Character->GetCharacterMovement()->MaxStepHeight*2)
			{
				bIsWalkable = false;
			}
		
			const float Slope = (Hit.ImpactPoint - LastLocation).Rotation().Pitch;
			Slopes.Add(Slope);
			
			LastLocation = Hit.ImpactPoint;	
		}
	}
	
	UGBWFlowMoveFuncLib::Boxplot_Float(Slopes, Slopes);
	for (auto Slope : Slopes)
	{
		if (Slope > SlopeMax)
		{
			SlopeMax = Slope;
		}
		else if (Slope < SlopeMin)
		{
			SlopeMin = Slope;
		}
		SlopeRes += Slope;
	}

	/*if (bIsWalkable)
	{
		return SlopeRes>0? SlopeMax : SlopeMin;
	}
	
	return 0;*/
	return SlopeRes>0? SlopeMax : SlopeMin;
}

bool UGBWFlowMovePerceptron_Default::TestBarrierFree(
	ACharacter* Character,
	const FHitResult TargetHitResultDown,
	FHitResult& WayHitResult,
	TArray<FHitResult>& WayHitResultDownArr,
	float TraceHeight,
	FGBWFlowMoveDefaultPerceptionSetting PerceptionSetting) const
{
	FVector End = TargetHitResultDown.TraceStart;
	FVector Start = Character->GetCapsuleComponent()->GetComponentLocation();
	Start.Z = End.Z;

	FHitResult TryHitResult;
	FHitResult SuccessHitResult;
	if (!TestWay(TryHitResult,
		SuccessHitResult,
		Character,
		Start,
		End,
		TraceHeight,
		PerceptionSetting))
	{
		return false;
	}

	WayHitResult = SuccessHitResult;

	FVector TraceLocation;
	float StepCount = (Start-End).Length() / Character->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.0f;
	for (int i=0; i<=StepCount; i++)
	{
		TraceLocation = UKismetMathLibrary::VLerp(Start, End, i/StepCount);
		bool Floor; FHitResult OutHit_Floor; bool Roof; FHitResult OutHit_Roof;
		FGBWFlowMoveFloorRoofScene FloorAndRoofRes;
		UGBWFlowMoveFuncLib::FindFloorAndRoof(Character, TraceLocation, 10000.0f, 10000.0f, false, 0.0f, PerceptionSetting.TraceSetting,
			Floor, OutHit_Floor, Roof, OutHit_Roof,FloorAndRoofRes);

		if (Floor)
		{
			WayHitResultDownArr.Add(OutHit_Floor);
		}
	}
	
	return !WayHitResultDownArr.IsEmpty();
}

FGBWFlowMoveFindTheBestFootholdPerceptionSetting::FGBWFlowMoveFindTheBestFootholdPerceptionSetting()
{
	
}

bool FGBWFlowMoveFindTheBestFootholdPerceptionSetting::GetSceneTagByActor(
		AActor* InActor,
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		FName& SceneTag,
		FVector& TargetPoint)
{
	SceneTag = FName();
	if (!InActor)
	{
		return false;
	}
	
	if (InActor->GetClass()->ImplementsInterface(UGBWFlowMoveInteractiveActorInterface::StaticClass()))
	{
		bool CanInteractive = false;
		IGBWFlowMoveInteractiveActorInterface::Execute_CanInteractive(
			InActor,
			FromCharacter,
			FromFlowMoveComponent,
			CanInteractive);

		if (!CanInteractive)
		{
			return false;
		}

		bool bIsGet = false;
		IGBWFlowMoveInteractiveActorInterface::Execute_GetTargetPoint(
			InActor,
			FromCharacter,
			FromFlowMoveComponent,
			bIsGet,
			TargetPoint,
			SceneTag);

		return bIsGet;
	}

	return false;
}

UGBWFlowMovePerceptron_FindTheBestFoothold::UGBWFlowMovePerceptron_FindTheBestFoothold()
{
	ThePerceptionSetting.TraceSetting.TraceObjectTypes = {
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	ThePerceptionSetting.TraceSetting.TraceChannels = {
		UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility)
	};
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::GetPerceptionResult_Implementation(FGBWFlowMoveScene& FlowMoveScene,
	EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent)
	{
		if (TryGetBestFootholdSceneTargetFromActor(
			FlowMoveScene,
			OwnerCharacter,
			ThePerceptionSetting,
			FlowMoveComponent))
		{
			return true;
		}

		if (!ThePerceptionSetting.bIsOnlyGetTargetFromActorWithComponentTag)
		{
			return FindWay(
				FlowMoveScene,
				OwnerCharacter,
				FlowMoveComponent,
				ThePerceptionSetting
			);
		}
	}
	return false;
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::FindWay(
	FGBWFlowMoveScene& FlowMoveScene,
	ACharacter* Character,
	UGBWFlowMoveComponent* FlowMoveComponent,
	FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting) const
{
	if (!Character || !FlowMoveComponent)
	{
		return false;
	}

	FName Res_SceneType = NAME_None;
	FVector Res_TargetLocation = FVector::ZeroVector;
	AActor* Res_TargetActor = nullptr;
	float Res_Slope = 0.0f;
	
	FVector MoveVector = FlowMoveComponent->TaskState.MoveVector;
	FVector ControlVector = FlowMoveComponent->GetControlVector();
	ControlVector.Z = 0.0f;
	ControlVector.Normalize();
	FVector TraceDirection = ControlVector.Rotation().RotateVector(MoveVector);
	TraceDirection.Normalize();
	if (TraceDirection.IsNearlyZero())
	{
		TraceDirection = ControlVector;
	}
	if (TraceDirection.IsNearlyZero())
	{
		TraceDirection = Character->GetCapsuleComponent()->GetForwardVector();
	}
	
	FVector UpTrace_Start = Character->GetCapsuleComponent()->GetComponentLocation();
	FVector UpTrace_End;

	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);

	FHitResult UpTraceHit;
	static const FName UpTraceTag{FString::Format(TEXT("{0} (Up Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = UpTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	UpTraceHit = UGBWFlowMoveFuncLib::LineTrace(
		Character->GetWorld(),
		ForwardTraceHitArr,
		UpTrace_Start,
		UpTrace_Start + FVector(0,0,PerceptionSetting.PrejudgeDistance),
		PerceptionSetting.TraceSetting,
		CollisionQueryParams
	);

	if (UpTraceHit.IsValidBlockingHit())
	{
		UpTrace_End = UpTraceHit.ImpactPoint;
	}
	else
	{
		UpTrace_End = UpTrace_Start + FVector(0,0,PerceptionSetting.PrejudgeDistance);
	}
	
	TArray<FVector> TraceVectorInterp = GenerateCrawlerDirection(TraceDirection,PerceptionSetting.CrawlerCount);
	TArray<FVector> LastHitPointInterp;
	TArray<FVector> AlternatePointArr_Target;
	TArray<FVector> AlternatePointArr_Prepare;
	float MaxDistance = -1;
	int MaxDistanceIndex = 0;
	TArray<AActor*> AlternatePointArr_TargetActor;
	TArray<UPrimitiveComponent*> AlternatePointArr_TargetPrimitive;
	for (int i=0; i<TraceVectorInterp.Num(); i++)
	{
		LastHitPointInterp.Add(UpTrace_End);
	}

	FVector Offset{0,0,Character->GetCapsuleComponent()->GetScaledCapsuleRadius()};
	
	for (int j=0; j<TraceVectorInterp.Num(); j++)
	{
		FVector TraceStart = FVector(UpTrace_Start.X,UpTrace_Start.Y,LastHitPointInterp[j].Z) - Offset;
		FVector TraceEnd = TraceStart + TraceVectorInterp[j] * PerceptionSetting.CrawlerMaxTraceDistance;
		FHitResult TryHitResult;
		if (!TestWay(
			TryHitResult,
			Character,
			TraceStart,
			TraceEnd,
			PerceptionSetting
		))
		{
			break;
		}
			
		if (TryHitResult.IsValidBlockingHit())
		{
			LastHitPointInterp[j] = TryHitResult.ImpactPoint;
		}
		else
		{
			LastHitPointInterp[j] = TraceEnd - Offset;
		}

		TArray<FHitResult> WayHitResultDown;
		TArray<FHitResult> WayHitResultUp;
		float MaxHeightHitResult_High;
		FHitResult MaxHeightHitResult_HighResult;
		float MaxHeightHitResult_Low;
		FHitResult MaxHeightHitResult_LowResult;
		float MinHeightHitResult_Low;
		FHitResult MinHeightHitResult_LowResult;
		float WayMinHeight;
		FHitResult MinZoneHitResult_DownResult;
		FHitResult MinZoneHitResult_UpResult;
			
		FindBottleneck(
			Character,
			TraceStart,
			LastHitPointInterp[j],
			PerceptionSetting,
			WayHitResultDown, 
			WayHitResultUp,
			MaxHeightHitResult_High,
			MaxHeightHitResult_HighResult,
			MaxHeightHitResult_Low,
			MaxHeightHitResult_LowResult,
			MinHeightHitResult_Low,
			MinHeightHitResult_LowResult,
			WayMinHeight,
			MinZoneHitResult_DownResult,
			MinZoneHitResult_UpResult);

		if (WayMinHeight<=0 || WayMinHeight >= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2)
		{
			TArray<FHitResult> WayHitResultDown_Valid;
			if (MaxHeightHitResult_HighResult.IsValidBlockingHit()
					&& Character->GetCharacterMovement()->IsWalkable(MaxHeightHitResult_HighResult)
					&& (MaxHeightHitResult_HighResult.ImpactPoint-UpTrace_Start).Length()<=PerceptionSetting.TargetPointMaxDistance)
			{
				WayHitResultDown_Valid.Add(MaxHeightHitResult_HighResult);
			}
			else
			{
				for (auto Hit:WayHitResultDown)
                {
                	if (Hit.IsValidBlockingHit()
                		&& Character->GetCharacterMovement()->IsWalkable(Hit)
                		&& (Hit.ImpactPoint-UpTrace_Start).Length()<=PerceptionSetting.TargetPointMaxDistance)
                	{
                		WayHitResultDown_Valid.Add(Hit);
                	}
                }
			}
			
			if (!WayHitResultDown_Valid.IsEmpty())
			{
				FHitResult WayHitResult = WayHitResultDown_Valid[WayHitResultDown_Valid.Num()/2];
				AlternatePointArr_Target.Add(WayHitResult.ImpactPoint + FVector(0,0,Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
				AlternatePointArr_Prepare.Add(TraceStart);
				AlternatePointArr_TargetActor.Add(WayHitResult.GetActor());
				AlternatePointArr_TargetPrimitive.Add(WayHitResult.GetComponent());

				float Distance = (AlternatePointArr_Target.Last()-UpTrace_Start).Z;
				if (Distance>=MaxDistance)
				{
					MaxDistance = Distance;
					MaxDistanceIndex = AlternatePointArr_Target.Num()-1;
				}
			}
		}
	}
	
	if (!AlternatePointArr_Target.IsEmpty())
	{
		Res_SceneType = PerceptionSetting.SuccessTag;
		Res_TargetLocation = AlternatePointArr_Target[MaxDistanceIndex];
		Res_TargetActor = AlternatePointArr_TargetActor[MaxDistanceIndex];
		//FlowMoveScene.TargetPrimitive = AlternatePointArr_TargetPrimitive[MaxDistanceIndex];
		
		FlowMoveScene = FGBWFlowMoveScene(
			Character,
			Res_SceneType,
			Res_TargetActor,
			Res_Slope,
			Res_TargetLocation,
			FVector::ZeroVector,
			FVector::ZeroVector,
			PerceptionSetting.TraceSetting,
			PerceptionSetting.FloorAndRoofLedgeTraceDistance);

		if (PerceptionSetting.SimpleDebug)
		{
			DrawDebugSphere(Character->GetWorld(),
				Res_TargetLocation,
				Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
				3,FColor::Green,false,0,0,5);
		}
					
		return true;
	}

	Res_SceneType = PerceptionSetting.FailedTag;
	Res_TargetLocation = Character->GetActorLocation();
	FlowMoveScene = FGBWFlowMoveScene(
			Character,
			Res_SceneType,
			Res_TargetActor,
			Res_Slope,
			Res_TargetLocation,
			FVector::ZeroVector,
			FVector::ZeroVector,
			PerceptionSetting.TraceSetting,
			PerceptionSetting.FloorAndRoofLedgeTraceDistance);
	return true;
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::TryGetBestFootholdSceneTargetFromActor(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting,
		UGBWFlowMoveComponent* FlowMoveComponent) const
{
	FName Res_SceneType = NAME_None;
	FVector Res_TargetLocation = FVector::ZeroVector;
	AActor* Res_TargetActor = nullptr;
	float Res_Slope = 0.0f;
	
	Res_SceneType = ThePerceptionSetting.FailedTag;
	
	if (!Character)
	{
		return false;
	}
	
	//Reusable parameters - start
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);
	//Reusable parameters - end

	const auto ActorLocation{Character->GetActorLocation()};
	
	FVector ForwardVector = FlowMoveComponent->GetControlVector();
	FVector ViewForwardVector;
	FVector ViewLocation;
	FRotator ViewRotation;
	UGBWRMSBPFuncLib::GBWRMS_GetCharacterViewInfo(Character,ViewLocation, ViewRotation, ViewForwardVector);
	ForwardVector = ViewForwardVector;
	
	ForwardVector.Z = 0.0f;
	if (!FlowMoveComponent->GetMoveVector().IsNearlyZero())
	{
		ForwardVector = ForwardVector.Rotation().RotateVector(FlowMoveComponent->GetMoveVector());
	}

	Res_TargetLocation = Character->GetActorLocation();

	const auto* Capsule{Character->GetCapsuleComponent()};
	
	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - CapsuleHalfHeight};

	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};

	// Get Target Point
	bool bIsGetTarget = false;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	
	FHitResult ForwardTraceHit = FHitResult();
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
			Character->GetWorld(),
			ForwardTraceHitArr,
			Character->GetActorLocation(),
			Character->GetActorLocation(),
			FQuat::Identity,
			ThePerceptionSetting.TraceSetting,
			FCollisionShape::MakeSphere(ThePerceptionSetting.PrejudgeDistance),
			CollisionQueryParams
		);

	FVector TargetPoint = FVector::ZeroVector;
	float AngleCache = 180.0f;
	AActor* TargetActor = nullptr;
	FVector TouchPoint = FVector::ZeroVector;
	FName SceneTag = FName();
	for (auto Hit:ForwardTraceHitArr)
	{
		if (TestVisibility(Character,Hit.GetActor(),ThePerceptionSetting.TestVisibilityMinDistance,ThePerceptionSetting.TraceSetting, TouchPoint))
		{
			FName TheSceneTag = FName();
			if (TryGetBestFootholdTargetFromActor(Hit.GetActor(), Character, FlowMoveComponent, TheSceneTag,TouchPoint))
			{
				SceneTag = TheSceneTag;
				FVector DirectionVector = TouchPoint-ViewLocation;
				DirectionVector.Normalize();
				float Angle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(DirectionVector, ViewForwardVector));
				if (Angle<=45.0f && Angle <= AngleCache)
				{
					AngleCache = Angle;
					TargetPoint = TouchPoint;
					TargetActor = Hit.GetActor();
				}
			}
		}
	}

	if (TargetActor)
	{
		Res_SceneType = !SceneTag.IsNone()? SceneTag : ThePerceptionSetting.SuccessTag;
		Res_TargetLocation = TargetPoint;
		Res_TargetActor = TargetActor;
    
		bIsGetTarget = true;
	}

	FlowMoveScene = FGBWFlowMoveScene(
			Character,
			Res_SceneType,
			Res_TargetActor,
			Res_Slope,
			Res_TargetLocation,
			FVector::ZeroVector,
			FVector::ZeroVector,
			PerceptionSetting.TraceSetting,
			PerceptionSetting.FloorAndRoofLedgeTraceDistance);

	return bIsGetTarget;
}

TArray<FVector> UGBWFlowMovePerceptron_FindTheBestFoothold::GenerateCrawlerDirection(FVector BeginTraceVector, int InterpCount)
{
	TArray<FVector> Result;
    	
	for (int i=0; i < InterpCount/2; i++)
	{
		Result.Add(UKismetMathLibrary::RotateAngleAxis(
			BeginTraceVector,
			360 / InterpCount * i,
			FVector(0,0,1)
		));
		Result.Add(UKismetMathLibrary::RotateAngleAxis(
			BeginTraceVector,
			i==0? 180 : -360 / InterpCount * i,
			FVector(0,0,1)
		));
	}
    
	return Result;
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::TestWay(FHitResult& TryHitResult, ACharacter* Character, FVector Start,
	FVector End, FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting) const
{
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);

	FHitResult ForwardTraceHit;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	ForwardTraceHit = UGBWFlowMoveFuncLib::LineTrace(
		Character->GetWorld(),
		ForwardTraceHitArr,
		Character->GetCapsuleComponent()->GetComponentLocation(),
		Start,
		PerceptionSetting.TraceSetting,
		CollisionQueryParams
	);

	if (ForwardTraceHit.IsValidBlockingHit())
	{
		return false;
	}

	float FinalTraceZOffset = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	ForwardTraceHitArr.Empty();
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		ForwardTraceHitArr,
		Start + FVector(0,0, FinalTraceZOffset),
		End + FVector(0,0, FinalTraceZOffset),
		FQuat::Identity,
		PerceptionSetting.TraceSetting,
		FCollisionShape::MakeCapsule(
			Character->GetCapsuleComponent()->GetScaledCapsuleRadius(),
			Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
			CollisionQueryParams
	);

	TryHitResult = ForwardTraceHit;
	return true;
}

void UGBWFlowMovePerceptron_FindTheBestFoothold::FindBottleneck(ACharacter* Character, FVector Start, FVector End,
	FGBWFlowMoveFindTheBestFootholdPerceptionSetting PerceptionSetting, TArray<FHitResult>& WayHitResultDown,
	TArray<FHitResult>& WayHitResultUp, float& MaxHeightHitResult_High, FHitResult& MaxHeightHitResult_HighResult,
	float& MaxHeightHitResult_Low, FHitResult& MaxHeightHitResult_LowResult, float& MinHeightHitResult_Low,
	FHitResult& MinHeightHitResult_LowResult, float& WayMinHeight, FHitResult& MinZoneHitResult_DownResult,
	FHitResult& MinZoneHitResult_UpResult) const
{
	WayMinHeight = -1.0f;
	MaxHeightHitResult_High = -1.0f;
	MaxHeightHitResult_Low = -1.0f;
	MinHeightHitResult_Low = -1.0f;

	float FootHeight = Character->GetCapsuleComponent()->GetComponentLocation().Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector TraceLocation;
	float StepCount = (Start-End).Length() / Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	for (int i=0; i<=StepCount; i++)
	{
		TraceLocation = UKismetMathLibrary::VLerp(Start, End, i/StepCount);
		bool Floor; FHitResult OutHit_Floor; bool Roof; FHitResult OutHit_Roof;
		FGBWFlowMoveFloorRoofScene FloorAndRoofRes;
		UGBWFlowMoveFuncLib::FindFloorAndRoof(Character, TraceLocation, 10000.0f, 10000.0f, false,0.0f, PerceptionSetting.TraceSetting,
			Floor, OutHit_Floor, Roof, OutHit_Roof,FloorAndRoofRes);

		WayHitResultDown.Add(OutHit_Floor);
		WayHitResultUp.Add(OutHit_Roof);

		if (Floor)
		{
			float Height = OutHit_Floor.ImpactPoint.Z - FootHeight;
			if (Height >= 0)
			{
				if (Height >= MaxHeightHitResult_High - 5)
				{
					MaxHeightHitResult_High = Height;
					MaxHeightHitResult_HighResult = OutHit_Floor;
				}
			}
			else
			{
				if (-Height <= MaxHeightHitResult_Low+5.0f || MaxHeightHitResult_Low < 0)
				{
					MaxHeightHitResult_Low = -Height;
					MaxHeightHitResult_LowResult = OutHit_Floor;
				}

				if (-Height >= MinHeightHitResult_Low)
				{
					MinHeightHitResult_Low = -Height;
					MinHeightHitResult_LowResult = OutHit_Floor;
				}
			}
		}

		if (Floor && Roof)
		{
			float WayHeight = (OutHit_Roof.ImpactPoint-OutHit_Floor.ImpactPoint).Z;
			if (WayMinHeight <= 0 || (WayMinHeight>0 && WayMinHeight >= WayHeight))
			{
				WayMinHeight = WayHeight;
				MinZoneHitResult_DownResult = OutHit_Floor;
				MinZoneHitResult_UpResult = OutHit_Roof;
			}
		}
	}
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::TryGetBestFootholdTargetFromActor(
		AActor* InActor,
		ACharacter* FromCharacter,
		UGBWFlowMoveComponent* FromFlowMoveComponent,
		FName& SceneTag,
		FVector& TargetPoint) const
{
	bool bIsGet = false;
	if (InActor && FromCharacter)
	{
		bIsGet = ThePerceptionSetting.GetSceneTagByActor(InActor,FromCharacter,FromFlowMoveComponent,SceneTag,TargetPoint);
	}

	return bIsGet;
}

bool UGBWFlowMovePerceptron_FindTheBestFoothold::TestVisibility(AActor* StartActor, AActor* EndActor,float MinDistanceForTest,
	FGBWFlowMoveTraceSetting TraceSetting, FVector& TouchPoint)
{
	if (!StartActor || !EndActor)
	{
		return false;
	}

	if ((StartActor->GetActorLocation() - EndActor->GetActorLocation()).Length() <= MinDistanceForTest)
	{
		return true;
	}

	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(StartActor);
	
	TArray<FHitResult> OutHits;
	FHitResult HitRes = UGBWFlowMoveFuncLib::LineTrace(
		StartActor->GetWorld(),
		OutHits,
		StartActor->GetActorLocation(),
		EndActor->GetActorLocation(),
		TraceSetting,
		CollisionQueryParams);

	if (HitRes.IsValidBlockingHit())
	{
		if (HitRes.GetActor() == EndActor)
		{
			TouchPoint = HitRes.ImpactPoint;
			return true;
		}
	}

	return false;
}

UGBWFlowMovePerceptron_GetTargetPointByProjectile::UGBWFlowMovePerceptron_GetTargetPointByProjectile()
{
	ThePerceptionSetting.TraceSetting.TraceObjectTypes = {
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	ThePerceptionSetting.TraceSetting.TraceChannels = {
		UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility)
	};
}

bool UGBWFlowMovePerceptron_GetTargetPointByProjectile::GetPerceptionResult_Implementation(
	FGBWFlowMoveScene& FlowMoveScene, EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter,
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent)
	{
		return GetFlowScene(
			FlowMoveScene,
			EventType,
			OwnerCharacter,
			ThePerceptionSetting,
			FlowMoveComponent->TaskState.MoveVector,
			FlowMoveComponent->TaskState.ControlVector,
			FlowMoveComponent);
	}
	return false;
}

bool UGBWFlowMovePerceptron_GetTargetPointByProjectile::GetFlowScene(
	FGBWFlowMoveScene& FlowMoveScene,
	EGBWFlowMoveEventType EventType,
	ACharacter* Character,
	FGBWFlowMoveGetTargetPointByProjectilePerceptionSetting PerceptionSetting,
	FVector InputVector,
	FVector ControlVector,
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	FName Res_SceneType = NAME_None;
	FVector Res_TargetLocation = FVector::ZeroVector;
	AActor* Res_TargetActor = nullptr;
	float Res_Slope = 0.0f;
	
	Res_SceneType = ThePerceptionSetting.FailedTag;
	
	if (!Character)
	{
		return false;
	}
	
	FGBWFlowMoveScene TheFlowMoveSceneCache;
	switch (EventType)
	{
	case EGBWFlowMoveEventType::Init:
		bIsFinished = false;
		ProjectileActorGUID = FGuid::NewGuid();
		ResetState(FlowMoveComponent);
		break;
	case EGBWFlowMoveEventType::Update:

		break;
	case EGBWFlowMoveEventType::End:
		bIsFinished = true;
		UpdateProjectileActor(FlowMoveComponent->TaskState.FrameDeltaTime, FlowMoveComponent);
		
		if (ThePerceptionSetting.GetLastProjectileLocationAsTargetWhenNotHit)
		{
			const FVector ActorLocation = Character->GetActorLocation();
			const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
			
			Res_SceneType = ThePerceptionSetting.SuccessTag;
			Res_TargetLocation = LastProjectileLocation;
			FlowMoveScene = FGBWFlowMoveScene(
				Character,
				Res_SceneType,
				Res_TargetActor,
				Res_Slope,
				Res_TargetLocation,
				FVector::ZeroVector,
				FVector::ZeroVector,
				PerceptionSetting.TraceSetting,
				0.0f);
			ResetState(FlowMoveComponent);
			SetCache(FlowMoveScene);
			return true;
		}
		ResetState(FlowMoveComponent);
		return false;
	}

	if(!bIsActive)
	{
		bIsActive = true;
	    FlyDirection = ControlVector;
	    LastProjectileLocation = GetLaunchLocation(Character);
	}
	else if (!bIsFinished)
	{
		Timer += FlowMoveComponent->TaskState.FrameDeltaTime;

		UpdateProjectileActor(FlowMoveComponent->TaskState.FrameDeltaTime, FlowMoveComponent);
		
		if (Timer < ThePerceptionSetting.DelayLaunch)
		{
			FlyDirection = ControlVector;
			LastProjectileLocation = GetLaunchLocation(Character);
			return false;
		}

		if (FlyDistance>=ThePerceptionSetting.ProjectileMaxFlyDistance)
		{
			bIsFinished = true;
			UpdateProjectileActor(FlowMoveComponent->TaskState.FrameDeltaTime, FlowMoveComponent);
			if (ThePerceptionSetting.GetLastProjectileLocationAsTargetWhenNotHit)
			{
				const FVector ActorLocation = Character->GetActorLocation();
				const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
			
				Res_SceneType = ThePerceptionSetting.SuccessTag;
				Res_TargetLocation = LastProjectileLocation;
				FlowMoveScene = FGBWFlowMoveScene(
					Character,
					Res_SceneType,
					Res_TargetActor,
					Res_Slope,
					Res_TargetLocation,
					FVector::ZeroVector,
					FVector::ZeroVector,
					PerceptionSetting.TraceSetting,
					0.0f);
				SetCache(FlowMoveScene);
				return true;
			}
			return false;
		}
		
		//Reusable parameters - start
		FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
		CollisionQueryParams.bTraceComplex = false;
		CollisionQueryParams.AddIgnoredActor(Character);
		//Reusable parameters - end
		
		FVector NewLocation = LastProjectileLocation + FlyDirection * ThePerceptionSetting.ProjectileSpeed * FlowMoveComponent->TaskState.FrameDeltaTime;

		static const FName ProjectileTraceTag{FString::Format(TEXT("{0} (Projectile Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
		FHitResult ProjectileTraceHit;
		CollisionQueryParams.TraceTag = ProjectileTraceTag;
		TArray<struct FHitResult> ProjectileTraceHitArr;
		UGBWFlowMoveFuncLib::SweepMulti(
			Character->GetWorld(),
			ProjectileTraceHitArr,
			LastProjectileLocation,
			NewLocation,
			FQuat::Identity,
			ThePerceptionSetting.TraceSetting,
			FCollisionShape::MakeSphere(ThePerceptionSetting.ProjectileRadius),
			CollisionQueryParams
		);
		
		if (ThePerceptionSetting.TraceSetting.isDebug)
		{
			DrawDebugSphere(Character->GetWorld(),LastProjectileLocation,ThePerceptionSetting.ProjectileRadius,
				5,FColor::Purple,false,0.0f,0,1.0f);
			DrawDebugSphere(Character->GetWorld(),NewLocation,ThePerceptionSetting.ProjectileRadius,
				5,FColor::Red,false,0.0f,0,1.0f);
			DrawDebugLine(Character->GetWorld(),LastProjectileLocation,NewLocation,
				FColor::Red,false,0.0f,0,2.0f);
			DrawDebugString(Character->GetWorld(),NewLocation,
				FString::Printf(TEXT("Time:%.2fs, Distance:%.2fcm"),
					Timer
					,FlyDistance),
				nullptr,FColor::Green,0.0f,true,1.5f);
		}

		bool bIsHit = false;
		for (auto Hit:ProjectileTraceHitArr)
		{
			if (Hit.IsValidBlockingHit())
			{
				ProjectileTraceHit = Hit;
				bIsHit = true;
				break;
			}
		}
		if (bIsHit)
		{
			const FVector ActorLocation = Character->GetActorLocation();
			const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
			
			FlyDistance += (ProjectileTraceHit.ImpactPoint - LastProjectileLocation).Length();
			Res_SceneType = ThePerceptionSetting.SuccessTag;
			Res_TargetLocation = ProjectileTraceHit.ImpactPoint;
			
			bIsFinished = true;
			UpdateProjectileActor(FlowMoveComponent->TaskState.FrameDeltaTime, FlowMoveComponent);
			ResetState(FlowMoveComponent);
			
			if (ThePerceptionSetting.TraceSetting.isDebug)
			{
				DrawDebugSphere(Character->GetWorld(),ProjectileTraceHit.ImpactPoint,ThePerceptionSetting.ProjectileRadius,
					5,FColor::Red,false,1.0f,0,1.0f);
			}

			FlowMoveScene = FGBWFlowMoveScene(
				Character,
				Res_SceneType,
				Res_TargetActor,
				Res_Slope,
				Res_TargetLocation,
				FVector::ZeroVector,
				FVector::ZeroVector,
				PerceptionSetting.TraceSetting,
				0.0f);
			SetCache(FlowMoveScene);
			return true;
		}
		
		FlyDistance += (NewLocation - LastProjectileLocation).Length();
		LastProjectileLocation = NewLocation;
	}

	return false;
}

void UGBWFlowMovePerceptron_GetTargetPointByProjectile::ResetState(UGBWFlowMoveComponent* FlowMoveComponent)
{
	FlowMoveComponent->DestroyNetWorkActor(ProjectileActorGUID);
	bIsActive = false;
	LastProjectileLocation = FVector::ZeroVector;
	FlyDirection = FVector::ZeroVector;
	Timer = 0.0f;
	FlyDistance = 0.0f;
	bIsProjectileActorInited = false;
}

FVector UGBWFlowMovePerceptron_GetTargetPointByProjectile::GetLaunchLocation(const ACharacter* Character) const
{
	FVector Result = FVector::ZeroVector;
	if (Character)
	{
		const USceneComponent* LaunchComponent = nullptr;
		if (!ThePerceptionSetting.LaunchFromComponentTag.IsNone())
		{
			TArray<UActorComponent*> C =  Character->GetComponentsByTag(ThePerceptionSetting.LaunchFromComponentClass,ThePerceptionSetting.LaunchFromComponentTag);
			for (const auto Item:C)
			{
				if (const auto c = Cast<USceneComponent>(Item))
				{
					LaunchComponent = c;
					break;
				}
			}
		}

		if (LaunchComponent)
		{
			Result = LaunchComponent->GetSocketLocation(ThePerceptionSetting.LaunchFromComponentSocketName)
			+ UKismetMathLibrary::TransformDirection(
				LaunchComponent->GetSocketTransform(ThePerceptionSetting.LaunchFromComponentSocketName),
				ThePerceptionSetting.LaunchFromComponentOffset.GetLocation());
			
		}
		else
		{
			Result = Character->GetActorLocation()
			+ UKismetMathLibrary::TransformDirection(
				Character->GetTransform(),
				ThePerceptionSetting.LaunchFromComponentOffset.GetLocation());
		}
	}

	return Result;
}

void UGBWFlowMovePerceptron_GetTargetPointByProjectile::UpdateProjectileActor(float DeltaTime, UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (!FlowMoveComponent->IsInServer())
	{
		return;
	}
	
	if (Timer >= ThePerceptionSetting.DelayLaunch && !bIsFinished)
	{
		if (!bIsProjectileActorInited)
		{
			bIsProjectileActorInited = true;
			ProjectileActorGUID = FGuid::NewGuid();
			FTransform T;
			T.SetLocation(LastProjectileLocation + ThePerceptionSetting.LaunchFromComponentOffset.GetLocation());
			T.SetRotation(ThePerceptionSetting.LaunchFromComponentOffset.GetRotation());
			T.SetScale3D(ThePerceptionSetting.LaunchFromComponentOffset.GetScale3D());
			
			FlowMoveComponent->SpawnNetWorkActor(
				ProjectileActorGUID,
				ThePerceptionSetting.ProjectileActorClass,
				T
			);
		}

		FlowMoveComponent->SetNetWorkActorLocation(ProjectileActorGUID, LastProjectileLocation);
	}
	else if (bIsFinished)
	{
		FlowMoveComponent->DestroyNetWorkActor(ProjectileActorGUID);
	}
}

UGBWFlowMovePerceptron_FindTargetPointByActor::UGBWFlowMovePerceptron_FindTargetPointByActor()
{
	ThePerceptionSetting.TraceSetting.TraceObjectTypes = {
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldStatic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_WorldDynamic),
		UCollisionProfile::Get()->ConvertToObjectType(ECC_Destructible)
	};

	ThePerceptionSetting.TraceSetting.TraceChannels = {
		UCollisionProfile::Get()->ConvertToTraceType(ECC_Visibility)
	};
}

bool UGBWFlowMovePerceptron_FindTargetPointByActor::GetPerceptionResult_Implementation(FGBWFlowMoveScene& FlowMoveScene,
	EGBWFlowMoveEventType EventType, ACharacter* OwnerCharacter, UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent)
	{
		return GetFlowScene(
			FlowMoveScene,
			OwnerCharacter,
			ThePerceptionSetting,
			FlowMoveComponent);
	}
	return false;
}

void UGBWFlowMovePerceptron_FindTargetPointByActor::OnUpdate_Implementation(FGameplayTag PerceptionKey, EGBWFlowMoveEventType EventType, UGBWFlowMoveComponent* FlowMoveComponent)
{
	CheckWidget(PerceptionKey,EventType,FlowMoveComponent);
}

bool UGBWFlowMovePerceptron_FindTargetPointByActor::GetFlowScene(
		FGBWFlowMoveScene& FlowMoveScene,
		ACharacter* Character,
		FGBWFlowMoveFindTargetPointByActorPerceptionSetting PerceptionSetting,
		UGBWFlowMoveComponent* FlowMoveComponent)
{
	FName Res_SceneType = NAME_None;
	FVector Res_TargetLocation = FVector::ZeroVector;
	AActor* Res_TargetActor = nullptr;
	float Res_Slope = 0.0f;
	
	Res_SceneType = ThePerceptionSetting.FailedTag;
	
	if (!Character)
	{
		return false;
	}

	//Reusable parameters - start
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(Character);
	//Reusable parameters - end

	const auto ActorLocation{Character->GetActorLocation()};
	const auto ActorYawAngle{UE_REAL_TO_FLOAT(FRotator::NormalizeAxis(Character->GetActorRotation().Yaw))};
	
	FVector ForwardVector = FlowMoveComponent->GetControlVector();
	FVector ViewForwardVector;
	FVector ViewLocation;
	FRotator ViewRotation;
	UGBWRMSBPFuncLib::GBWRMS_GetCharacterViewInfo(Character,ViewLocation, ViewRotation, ViewForwardVector);
	ForwardVector = ViewForwardVector;
	
	ForwardVector.Z = 0.0f;
	if (!FlowMoveComponent->GetMoveVector().IsNearlyZero())
	{
		ForwardVector = ForwardVector.Rotation().RotateVector(FlowMoveComponent->GetMoveVector());
	}

	Res_TargetLocation = Character->GetActorLocation();
	
	const auto VelocityYawAngle = UGBWFlowMoveFuncLib::GetVelocityYawAngle(ForwardVector);
	
	float ForwardTraceAngle = VelocityYawAngle;

	const auto ForwardTraceDeltaAngle{FRotator3f::NormalizeAxis(ForwardTraceAngle - ActorYawAngle)};
	if (FMath::Abs(ForwardTraceDeltaAngle) > PerceptionSetting.TraceAngleThreshold)
	{
		return false;
	}
	
	const auto ForwardTraceDirection{FlowMoveComponent->GetControlVector()};

	const auto* Capsule{Character->GetCapsuleComponent()};

	const auto CapsuleScale{Capsule->GetComponentScale().Z};
	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto CapsuleHalfHeight{Capsule->GetScaledCapsuleHalfHeight()};

	const FVector CapsuleBottomLocation{ActorLocation.X, ActorLocation.Y, ActorLocation.Z - CapsuleHalfHeight};

	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};

	// Get Target Point
	bool bIsGetTarget = false;
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	
	auto ForwardTraceStart{ViewLocation+ForwardTraceDirection*CapsuleRadius*2.0f};
	auto ForwardTraceEnd{ForwardTraceStart + ForwardTraceDirection * (CapsuleRadius + (PerceptionSetting.PrejudgeDistance + 1.0f) * CapsuleScale)};

	FHitResult ForwardTraceHit;
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		Character->GetWorld(),
		ForwardTraceHitArr,
		ForwardTraceStart,
		ForwardTraceEnd,
		FQuat::Identity,
		ThePerceptionSetting.TraceSetting,
		FCollisionShape::MakeSphere(TraceCapsuleRadius),
		CollisionQueryParams
	);

	if (ForwardTraceHit.IsValidBlockingHit())
	{
		if (ForwardTraceHit.GetActor()->GetClass()->ImplementsInterface(UGBWFlowMoveInteractiveActorInterface::StaticClass()))
		{
			bool CanInteractive = false;
			IGBWFlowMoveInteractiveActorInterface::Execute_CanInteractive(
				ForwardTraceHit.GetActor(),
				Character,
				FlowMoveComponent,
				CanInteractive);

			if (CanInteractive)
			{
				if (FVector TargetPoint = FVector::ZeroVector;
				TestVisibility(Character,ForwardTraceHit.GetActor(),ThePerceptionSetting.TraceSetting,TargetPoint))
				{
					Res_SceneType = ThePerceptionSetting.SuccessTag;
					Res_TargetLocation = ForwardTraceHit.ImpactPoint;
					Res_TargetActor = ForwardTraceHit.GetActor();
    
					bIsGetTarget = true;
				}
			}
		}
	}

	if (!bIsGetTarget)
	{
		ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
			Character->GetWorld(),
			ForwardTraceHitArr,
			Character->GetActorLocation(),
			Character->GetActorLocation(),
			FQuat::Identity,
			ThePerceptionSetting.TraceSetting,
			FCollisionShape::MakeSphere(ThePerceptionSetting.PrejudgeDistance),
			CollisionQueryParams
		);

		FVector TargetPoint = FVector::ZeroVector;
		float AngleCache = 180.0f;
		AActor* TargetActor = nullptr;
		FVector TouchPoint = FVector::ZeroVector;
		for (auto Hit:ForwardTraceHitArr)
		{
			if (Hit.GetActor()->GetClass()->ImplementsInterface(UGBWFlowMoveInteractiveActorInterface::StaticClass()))
			{
				bool CanInteractive = false;
				IGBWFlowMoveInteractiveActorInterface::Execute_CanInteractive(
					Hit.GetActor(),
					Character,
					FlowMoveComponent,
					CanInteractive);

				if (CanInteractive)
				{
					if (TestVisibility(Character,Hit.GetActor(),ThePerceptionSetting.TraceSetting, TouchPoint))
					{
						FVector DirectionVector = TouchPoint-ViewLocation;
						DirectionVector.Normalize();
						float Angle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(DirectionVector, ViewForwardVector));
						if (Angle<=ThePerceptionSetting.FuzzyQueryAngleRestrictions && Angle <= AngleCache)
						{
							AngleCache = Angle;
							TargetPoint = TouchPoint;
							TargetActor = Hit.GetActor();
						}
					}
				}
			}
		}

		if (TargetActor)
		{
			Res_SceneType = ThePerceptionSetting.SuccessTag;
			Res_TargetLocation = TargetPoint;
			Res_TargetActor = TargetActor;
    
			bIsGetTarget = true;
		}
	}

	FlowMoveScene = FGBWFlowMoveScene(
			Character,
			Res_SceneType,
			Res_TargetActor,
			Res_Slope,
			Res_TargetLocation,
			FVector::ZeroVector,
			FVector::ZeroVector,
			PerceptionSetting.TraceSetting,
			0.0f);
	return bIsGetTarget;
}

bool UGBWFlowMovePerceptron_FindTargetPointByActor::TestVisibility(
	AActor* StartActor,
	AActor* EndActor,
	FGBWFlowMoveTraceSetting TraceSetting,
	FVector& TouchPoint)
{
	if (!StartActor || !EndActor)
	{
		return false;
	}

	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(StartActor);
	
	TArray<FHitResult> OutHits;
	FHitResult HitRes = UGBWFlowMoveFuncLib::LineTrace(
		StartActor->GetWorld(),
		OutHits,
		StartActor->GetActorLocation(),
		EndActor->GetActorLocation(),
		TraceSetting,
		CollisionQueryParams);

	if (HitRes.IsValidBlockingHit())
	{
		if (HitRes.GetActor() == EndActor)
		{
			TouchPoint = HitRes.ImpactPoint;
			return true;
		}
	}

	return false;
}

void UGBWFlowMovePerceptron_FindTargetPointByActor::CheckWidget(
	FGameplayTag PerceptionKey,
	EGBWFlowMoveEventType EventType,
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (FlowMoveComponent && FlowMoveComponent->IsLocalPlayer())
	{
		if (EventType != EGBWFlowMoveEventType::End)
		{
			if (!SightWidget_NoTarget && IsValid(ThePerceptionSetting.SightWidgetClass_NoTarget))
			{
				SightWidget_NoTarget = CreateWidget(FlowMoveComponent->TaskState.OwnerCharacter->GetWorld(), ThePerceptionSetting.SightWidgetClass_NoTarget);
			}
			if (!SightWidget_GetTarget && IsValid(ThePerceptionSetting.SightWidgetClass_GetTarget))
			{
				SightWidget_GetTarget = CreateWidget(FlowMoveComponent->TaskState.OwnerCharacter->GetWorld(), ThePerceptionSetting.SightWidgetClass_GetTarget);
			}
			if (!TargetWidget && IsValid(ThePerceptionSetting.TargetWidgetClass))
			{
				TargetWidget = CreateWidget(FlowMoveComponent->TaskState.OwnerCharacter->GetWorld(), ThePerceptionSetting.TargetWidgetClass);
			}
			
			if (FGBWFlowMoveScene Scene; FlowMoveComponent->GetScene(Scene,PerceptionKey,false))
			{
				if (Scene.SceneType == ThePerceptionSetting.SuccessTag)
				{
					if (SightWidget_NoTarget && SightWidget_NoTarget->IsInViewport())
					{
						SightWidget_NoTarget->RemoveFromParent();
					}
					if (SightWidget_GetTarget && !SightWidget_GetTarget->IsInViewport())
					{
						SightWidget_GetTarget->AddToViewport(1);
					}
					if (TargetActorCache != Scene.TargetActor)
					{
						TargetActorCache = Scene.TargetActor;
						if (ThePerceptionSetting.Sound_GetTarget)
						{
							UGameplayStatics::PlaySound2D(FlowMoveComponent,ThePerceptionSetting.Sound_GetTarget);
						}
					}
					
					if (TargetWidget && !TargetWidget->IsInViewport())
					{
						TargetWidget->AddToViewport();
					}

					if (const auto PC = UGameplayStatics::GetPlayerController(FlowMoveComponent,0))
					{
						if (FVector2D Translation;
							TargetActorCache && 
							UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC,TargetActorCache->GetActorLocation(),Translation,false))
						{
							if (TargetWidget)
							{
								TargetWidget->SetRenderTranslation(Translation);
							}
						}
					}
				}
				else
				{
					if (SightWidget_NoTarget && !SightWidget_NoTarget->IsInViewport())
					{
						SightWidget_NoTarget->AddToViewport(1);
					}
					if (SightWidget_GetTarget && SightWidget_GetTarget->IsInViewport())
					{
						SightWidget_GetTarget->RemoveFromParent();
					}
					if (TargetWidget && TargetWidget->IsInViewport())
					{
						TargetWidget->RemoveFromParent();
					}
				}
			}
		}
		else
		{
			if (SightWidget_NoTarget && SightWidget_NoTarget->IsInViewport())
			{
				SightWidget_NoTarget->RemoveFromParent();
			}
			if (SightWidget_GetTarget && SightWidget_GetTarget->IsInViewport())
			{
				SightWidget_GetTarget->RemoveFromParent();
			}
			if (TargetWidget && TargetWidget->IsInViewport())
			{
				TargetWidget->RemoveFromParent();
			}
		}
	}
}

USceneComponent* UGBWFlowMoveGetFocusActor_GetFocusActor_Default::OnGetFocusActor_Implementation(
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	USceneComponent* Res = nullptr;
	
	if (!FlowMoveComponent)
	{
		return Res;
	}
	
	//Reusable parameters - start
	FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.AddIgnoredActor(FlowMoveComponent->TaskState.OwnerCharacter);
	//Reusable parameters - end

	FVector ForwardVector = FlowMoveComponent->GetControlVector();
	FVector ViewForwardVector;
	FVector ViewLocation;
	FRotator ViewRotation;
	UGBWRMSBPFuncLib::GBWRMS_GetCharacterViewInfo(FlowMoveComponent->TaskState.OwnerCharacter,ViewLocation, ViewRotation, ViewForwardVector);
	ForwardVector = ViewForwardVector;
	
	ForwardVector.Z = 0.0f;
	if (!FlowMoveComponent->GetMoveVector(false).IsNearlyZero())
	{
		ForwardVector = ForwardVector.Rotation().RotateVector(FlowMoveComponent->GetMoveVector(false));
	}
	
	const auto ForwardTraceDirection{FlowMoveComponent->GetControlVector()};

	const auto* Capsule{FlowMoveComponent->TaskState.OwnerCharacter->GetCapsuleComponent()};

	const auto CapsuleScale{Capsule->GetComponentScale().Z};
	const auto CapsuleRadius{Capsule->GetScaledCapsuleRadius()};
	const auto TraceCapsuleRadius{CapsuleRadius - 1.0f};
	
	// Get Target Point
	static const FName ForwardTraceTag{FString::Format(TEXT("{0} (Forward Trace)"), {ANSI_TO_TCHAR(__FUNCTION__)})};
	
	auto ForwardTraceStart{ViewLocation+ForwardTraceDirection*CapsuleRadius*2.0f};
	auto ForwardTraceEnd{ForwardTraceStart + ForwardTraceDirection * (CapsuleRadius + (PrejudgeDistance + 1.0f) * CapsuleScale)};

	FHitResult ForwardTraceHit;
	CollisionQueryParams.TraceTag = ForwardTraceTag;
	TArray<struct FHitResult> ForwardTraceHitArr;
	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
		FlowMoveComponent->TaskState.OwnerCharacter->GetWorld(),
		ForwardTraceHitArr,
		ForwardTraceStart,
		ForwardTraceEnd,
		FQuat::Identity,
		PerceptionSetting,
		FCollisionShape::MakeSphere(TraceCapsuleRadius),
		CollisionQueryParams
	);

	if (ForwardTraceHit.IsValidBlockingHit())
	{
		if (FVector TargetPoint = FVector::ZeroVector;
				TestVisibility(FlowMoveComponent->TaskState.OwnerCharacter,
					ForwardTraceHit.GetActor(),PerceptionSetting,TargetPoint))
		{
			AActor* TargetActor = ForwardTraceHit.GetActor();
			if (ComponentWithTags.IsEmpty())
			{
				return TargetActor->GetRootComponent();
			}
			
			for (auto Tag:ComponentWithTags)
			{
				TArray<UActorComponent*> ACs = TargetActor->GetComponentsByTag(USceneComponent::StaticClass(), Tag);
				if (!ACs.IsEmpty())
				{
					Res = Cast<USceneComponent>(ACs[0]);
				}
				if (Res)
				{
					return Res;
				}
			}
		}
	}

	ForwardTraceHit = UGBWFlowMoveFuncLib::SweepMulti(
			FlowMoveComponent->TaskState.OwnerCharacter->GetWorld(),
			ForwardTraceHitArr,
			FlowMoveComponent->TaskState.OwnerCharacter->GetActorLocation(),
			FlowMoveComponent->TaskState.OwnerCharacter->GetActorLocation(),
			FQuat::Identity,
			PerceptionSetting,
			FCollisionShape::MakeSphere(PrejudgeDistance),
			CollisionQueryParams
		);
	
	float AngleCache = 180.0f;
	AActor* TargetActor = nullptr;
	FVector TouchPoint = FVector::ZeroVector;
	for (auto Hit:ForwardTraceHitArr)
	{
		if (TestVisibility(FlowMoveComponent->TaskState.OwnerCharacter,
			Hit.GetActor(),PerceptionSetting, TouchPoint))
		{
			FVector DirectionVector = TouchPoint-ViewLocation;
			DirectionVector.Normalize();
			float Angle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(DirectionVector, ViewForwardVector));
			if (Angle<=FuzzyQueryAngleRestrictions && Angle <= AngleCache)
			{
				AngleCache = Angle;
				TargetActor = Hit.GetActor();
			}
		}
	}

	if (TargetActor)
	{
		if (ComponentWithTags.IsEmpty())
		{
			return TargetActor->GetRootComponent();
		}
			
		for (auto Tag:ComponentWithTags)
		{
			TArray<UActorComponent*> ACs = TargetActor->GetComponentsByTag(USceneComponent::StaticClass(), Tag);
			if (!ACs.IsEmpty())
			{
				Res = Cast<USceneComponent>(ACs[0]);
			}
			if (Res)
			{
				return Res;
			}
		}
	}

	return nullptr;
}

void UGBWFlowMoveScript_Debug::OnUpdate_Implementation(EGBWFlowMoveEventType EventType,EFlowMoveNetworkLocationType NetworkLocation, bool bIsLocalOwn,
                                                       UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (bIsLocalOwn && FlowMoveComponent)
	{
		FlowMoveDebug(FlowMoveComponent->TaskState);
	}
}

void UGBWFlowMoveScript_Debug::FlowMoveDebug(FGBWFlowMoveState TaskState) const
{
	if (bIsDebug)
	{
		int KeyBase = TaskState.FlowMoveComponent->IsInServer()? 0:100;
		const FVector StartPoint = TaskState.OwnerCharacter->GetActorLocation() + FVector(0,0,TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		if (FlowMoveState)
		{
			FGBWAnimPlayStateSet AnimPlayStateSet = FGBWAnimPlayStateSet();
			UGBWAnimFuncLib::GBWGetAnimPlayState(
					TaskState.OwnerCharacter,
					AnimPlayStateSet
					);
			
			GEngine->AddOnScreenDebugMessage(0+KeyBase,1,FColor::Orange,
			FString::Printf(
				TEXT(">>>>>>>FlowMoveState(%.2f)<<<<<<< "
				"\n IsActive : %s "
				"\n DefaultViewMode : %s "
				"\n ViewMode : %s "
				"\n OwnerCharacter : %s "
				"\n FocusActor : %s "
				"\n FrameDeltaTime : %.4f "
				"\n MoveVector : %s "
				"\n ControlVector : %s "
				"\n PerceptionVector : %s "
				"\n IsStopping : %s "
				"\n Speed : %.2f ; RealSpeed : %.2f"
				"\n HorizontalSpeed : %.2f ; RealHorizontalSpeed : %.2f"
				"\n ActionToLock (%s) : %s "
				"\n CurrentFlowMoveAction : (%.2f)%s "
				"\n    >>>>>>>MoveControll<<<<<<< "
				"\n       ActionGuid : %s "
				"\n       ActionTag : %s "
				"\n       TargetSceneTag : %s "
				"\n       TargetPointType : %s "
				"\n       RMSNow.Location : %s "
				"\n       RMSNow.Rotation : %s "
				"\n       LostSpeed : %f "
				"\n       "
				"\n       AnimPlayRateAdjust : %s (%.2f)"
				"\n          AnimPlayRateAdjustReferenceSpeed : %f"
				"\n          AdjustAnimPlayRateScale : %f"
				"\n          Min_PlayRateRange : %f to %f"
				"\n       ToTargetRate : %f <%f>"
				"\n       MaxSpeed<%s> : %.2f"
				"\n       ConstraintMoveToPlane<%s> : %.2f"
				"\n       Gravity<to %.2f by %.2f> : %.2f <%.2f> <%s>"
				"\n       RotationLockToMoveDirectionYaw : %.2f <%.2f> <%s> LockTo<%s>"
				"\n       MoveSpeed<to %.2f by %.2f> : %.2f <%.2f> <%s>"
				"\n       MoveToDirectionSmoothSpeed : %.2f <%.2f> <%s>"
				"\n       CompensateLostSpeedToCurrentSpeedDirection : %s"
				"\n       PathOffset : %s <%s>"
				"\n       RotationOffset : %s <%s>"
				"\n    >>>>>>>>>>>>>><<<<<<<<<<<< "
				"\n    >>>>>>>AnimPlayState<<<<<<< "
				"\n %s"
				"\n    >>>>>>>>>>>>>><<<<<<<<<<<< "
				"\n >>>>>>>FlowMoveState<<<<<<<"),
				TaskState.Timer,
				*(TaskState.bIsActive? FString("True"):FString("False")),
				*(StaticEnum<EFlowMoveCharacterViewMode>()->GetNameStringByIndex( ( int32 ) TaskState.FlowMoveComponent->DefaultViewMode)),
				*(StaticEnum<EFlowMoveCharacterViewMode>()->GetNameStringByIndex( ( int32 ) TaskState.ViewMode)),
				*TaskState.OwnerCharacter->GetName(),
				*(TaskState.FocusActorComponent? TaskState.FocusActorComponent->GetOwner()->GetName() + " -> " + TaskState.FocusActorComponent->GetName() : FString("None")),
				TaskState.FrameDeltaTime,
				*TaskState.MoveVector.ToString(),
				*TaskState.ControlVector.ToString(),
				*TaskState.PerceptionVector.ToString(),
				*(TaskState.bIsStopping? FString("True"):FString("False")),
				TaskState.OwnerCharacter->GetVelocity().Length(), TaskState.RealVelocity.Length(),
				(FVector(TaskState.OwnerCharacter->GetVelocity().X,TaskState.OwnerCharacter->GetVelocity().Y,0.0f)).Length(),
				(FVector(TaskState.RealVelocity.X,TaskState.RealVelocity.Y,0.0f)).Length(),
				*(TaskState.ActionLockState.bHasLock? FString("True") : FString("False")),
				*TaskState.ActionLockState.ActionToLock.GetDescription(),
				TaskState.CurrentFlowMoveExecutedTime,
				*TaskState.CurrentFlowMoveActionTag.ToString(),
				*TaskState.MoveControlParam.ActionGuid.ToString(),
				*TaskState.MoveControlParam.ActionTag.ToString(),
				*TaskState.MoveControlParam.ActionTargetScene.ToString(),
				*(StaticEnum<EGBWFlowMoveScenePointType>()->GetNameStringByIndex( ( int32 ) TaskState.MoveControlParam.ActionTargetPoint)),
				*TaskState.RMSParamNow.GetLocation().ToString(),
				*TaskState.RMSParamNow.GetRotation().Rotator().ToString(),
				TaskState.LostSpeed,
				*(TaskState.MoveControlParam.AnimPlayRateAdjust.bUseSpeedAdjustAnimPlayRate? FString("True"):FString("False")),
				TaskState.MoveControlParam.AnimPlayRateNow,
				TaskState.MoveControlParam.AnimPlayRateAdjust.AnimPlayRateAdjustReferenceSpeed,TaskState.MoveControlParam.AnimPlayRateAdjust.AdjustAnimPlayRateScale,
				TaskState.MoveControlParam.AnimPlayRateAdjust.Min_AnimPlayRate,TaskState.MoveControlParam.AnimPlayRateAdjust.Max_AnimPlayRate,
				TaskState.MoveControlParam.ToTargetPointRate,TaskState.MoveControlParam.DeltaToTargetPointRate,
				*(TaskState.MoveControlParam.bIsLock_MaxSpeed? FString("Limitation"):FString("NoLimit")),
				TaskState.MoveControlParam.MaxMoveSpeed,
				*(TaskState.MoveControlParam.bIsLock_ConstrainMoveToTargetPlane? FString("True"):FString("False")),
				TaskState.MoveControlParam.ConstrainMoveTime,
				TaskState.MoveControlParam.TargetGravity,TaskState.MoveControlParam.GravitySmoothSpeed,
				TaskState.MoveControlParam.Gravity,TaskState.MoveControlParam.DeltaGravity,
				*(TaskState.MoveControlParam.bIsLock_Gravity? FString("Lock"):FString("FromAnimCurve")),
				TaskState.MoveControlParam.RotationLockToMoveDirectionYaw,TaskState.MoveControlParam.DeltaRotationLockToMoveDirectionYaw,
				*(TaskState.MoveControlParam.bIsLock_RotationLockToMoveDirectionYaw? FString("Lock"):FString("FromAnimCurve")),
				*(StaticEnum<EFlowMoveDirectionType>()->GetNameStringByIndex( ( int32 ) TaskState.MoveControlParam.RotationLockTo)),
				TaskState.MoveControlParam.TargetMoveSpeed,TaskState.MoveControlParam.MoveSpeedSmoothSpeed,
				TaskState.MoveControlParam.MoveSpeed,TaskState.MoveControlParam.DeltaMoveSpeed,
				*(TaskState.MoveControlParam.bIsLock_MoveSpeed? FString("Lock"):FString("FromAnimCurve")),
				TaskState.MoveControlParam.MoveToDirectionSmoothSpeed,TaskState.MoveControlParam.DeltaMoveToDirectionSmoothSpeed,
				*(TaskState.MoveControlParam.bIsLock_MoveToDirectionSmoothSpeed? FString("Lock"):FString("FromAnimCurve")),
				*(TaskState.MoveControlParam.CompensateLostSpeedToCurrentSpeedDirection? FString("True"):FString("False")),
				*TaskState.MoveControlParam.PathOffset.ToString(),*TaskState.MoveControlParam.DeltaPathOffset.ToString(),
				*TaskState.MoveControlParam.RotationOffset.ToString(),*TaskState.MoveControlParam.DeltaRotationOffset.ToString(),
				*UGBWAnimFuncLib::GetDebugStringFromAnimPlayState(AnimPlayStateSet)
			));
		}

		if (FlowMovePerceptron)
		{
			TArray<FGameplayTag> InOutGameplayTags = {};
			FString PerceptronReadyStr = "";
			FString PerceptronNowStr = "";
			FString PerceptronToEndStr = "";
			FString PerceptronInvalid = "";

			TaskState.PerceptronReady.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				PerceptronReadyStr += GT.ToString() + " \n   ";
			}
			TaskState.PerceptronNow.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				PerceptronNowStr += GT.ToString() + " \n   ";
			}
			TaskState.PerceptronToEnd.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				PerceptronToEndStr += GT.ToString() + " \n   ";
			}
			TaskState.PerceptronInvalid.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				PerceptronInvalid += GT.ToString() + " \n   ";
			}
			GEngine->AddOnScreenDebugMessage(1+KeyBase,1,FColor::Green,
			FString::Printf(
				TEXT(">>>>>>>FlowMovePerceptron<<<<<<< "
				"\nPerceptronReady :\n   %s "
				"\nPerceptronNow :\n   %s "
				"\nPerceptronToEnd :\n   %s "
				"\nPerceptronInvalid :\n   %s "
				"\n >>>>>>>FlowMovePerceptron<<<<<<<"),
				*PerceptronReadyStr,
				*PerceptronNowStr,
				*PerceptronToEndStr,
				*PerceptronInvalid
			));
		}

		if (FlowMoveScript)
		{
			TArray<FGameplayTag> InOutGameplayTags = {};
			FString ScriptReadyStr = "";
			FString ScriptNowStr = "";
			FString ScriptToEndStr = "";
			FString ScriptInvalid = "";

			TaskState.ScriptReady.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				ScriptReadyStr += GT.ToString() + " \n   ";
			}
			TaskState.ScriptNow.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				ScriptNowStr += GT.ToString() + " \n   ";
			}
			TaskState.ScriptToEnd.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				ScriptToEndStr += GT.ToString() + " \n   ";
			}
			TaskState.ScriptInvalid.GetGameplayTagArray(InOutGameplayTags);
			for (auto GT : InOutGameplayTags)
			{
				ScriptInvalid += GT.ToString() + " \n   ";
			}
			GEngine->AddOnScreenDebugMessage(2+KeyBase,1,FColor::Purple,
			FString::Printf(
				TEXT(">>>>>>>FlowMoveScript<<<<<<< "
				"\nScriptReady :\n   %s "
				"\nScriptNow :\n   %s "
				"\nScriptToEnd :\n   %s "
				"\nScriptInvalid :\n   %s "
				"\n >>>>>>>FlowMoveScript<<<<<<<"),
				*ScriptReadyStr,
				*ScriptNowStr,
				*ScriptToEndStr,
				*ScriptInvalid
			));
		}

		if (FlowMoveAction)
		{
			GEngine->AddOnScreenDebugMessage(3+KeyBase,1,FColor::Green,
			FString::Printf(
				TEXT(">>>>>>>FlowMoveAction<<<<<<< "
			   "\n LastFlowMoveAction : %s "
			   "\n CurrentFlowMoveAction : (%.2f)%s "
			   "\n ActionToLock (%s) : %s "
			   "\n >>>>>>>FlowMoveAction<<<<<<<"),
				*TaskState.LastFlowMoveActionTag.ToString(),
				TaskState.CurrentFlowMoveExecutedTime,
				*TaskState.CurrentFlowMoveActionTag.ToString(),
				*(TaskState.ActionLockState.bHasLock? FString("True") : FString("False")),
				*TaskState.ActionLockState.ActionToLock.GetDescription()
			));
		}
		
		int i = 1;
		if (FlowMoveScene)
		{
			FString SupplementaryFlowMoveSceneStr = "";
			
			SupplementaryFlowMoveSceneStr = SupplementaryFlowMoveSceneStr + "\n"
					+ FString::Printf(
				TEXT(">>>>>>>TargetFlowMoveScene (%s)(%s) <<<<<<<"
					"\n BeginVelocity : %s "
					"\n SceneType : %s "
					"\n TargetActor : %s "
					"\n Slope : %f"
					"\n ActorPoint : HeightFromGround:%.2f Ledge: F<%s>(%.2f) B<%s>(%.2f)"
					"\n              HeightFromRoof:%.2f Ledge: F<%s>(%.2f) B<%s>(%.2f)"
					"\n TargetPoint : Height:%.2f"
					"\n               Right:%.2f"
					"\n               Forward:%.2f"
					"\n               Angle:%.2f"
					"\n               Direction<%s>:%.2f"
					"\n               HeightFromGround:%.2f"
					"\n               HeightFromRoof:%.2f"
					"\n LeftTargetPoint <%.2f> : %s"
					"\n RightTargetPoint<%.2f> : %s"
					"\n >>>>>>>TargetFlowMoveScene<<<<<<<"),
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.bIsValid? FString("Valid") : FString("Invalid")),
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.ActionTargetScene.ToString()),
						*TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginVelocity.ToString(),
						*TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.SceneType.ToString(),
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetActor? TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetActor->GetName() : FString("None")),
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.Slope,
						
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.HeightFromGround,
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward.IsZero()? FString("False") : FString("ture")),
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward
							- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length(),
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward.IsZero()? FString("False") : FString("ture")),
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward
							- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor).Length(),
						
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.HeightFromRoof,
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward.IsZero()? FString("False") : FString("ture")),
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward
							- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length(),
						*(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward.IsZero()? FString("False") : FString("ture")),
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward
							- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof).Length(),

						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Height,
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Right,
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Forward,
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Angle,
						*(StaticEnum<EGBWFlowMoveDirectionType>()->GetNameStringByIndex( static_cast<int32>(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.SceneDirectionMark))),
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Direction,
						TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.FloorAndRoof.HeightFromGround,TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.FloorAndRoof.HeightFromRoof,
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.LeftLocationScene.Point
						- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.Point).Length(),
						*TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.LeftLocationScene.Point.ToString(),
						(TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.RightLocationScene.Point
						- TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.Point).Length(),
						*TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.RightLocationScene.Point.ToString()
					);
			
			for (auto SScene:TaskState.SupplementaryFlowMoveScene)
			{
				if (FlowMoveSceneTags.Contains(SScene.SceneSlot))
				{
					SupplementaryFlowMoveSceneStr = SupplementaryFlowMoveSceneStr + "\n"
					+ FString::Printf(
				TEXT(">>>>>>>FlowMoveScene (%s) <<<<<<<"
					"\n SceneType : %s "
					"\n TargetActor : %s "
					"\n Slope : %f"
					"\n ActorPoint : HeightFromGround:%.2f Ledge: F<%s>(%.2f) B<%s>(%.2f)"
					"\n              HeightFromRoof:%.2f Ledge: F<%s>(%.2f) B<%s>(%.2f)"
					"\n TargetPoint : Height:%.2f"
					"\n               Right:%.2f"
					"\n               Forward:%.2f"
					"\n               Angle:%.2f"
					"\n               Direction<%s>:%.2f"
					"\n               HeightFromGround:%.2f"
					"\n               HeightFromRoof:%.2f"
					"\n LeftTargetPoint <%.2f> : %s"
					"\n RightTargetPoint<%.2f> : %s"
					"\n >>>>>>>FlowMoveScene<<<<<<<"),
						*SScene.SceneSlot.ToString(),
						*SScene.Scene.SceneType.ToString(),
						*(SScene.Scene.TargetActor? SScene.Scene.TargetActor->GetName() : FString("None")),
						SScene.Scene.Slope,
						
						SScene.Scene.ActorLocationScene.FloorAndRoof.HeightFromGround,
						*(SScene.Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward.IsZero()? FString("False") : FString("ture")),
						(SScene.Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward - SScene.Scene.ActorLocationScene.FloorAndRoof.Floor).Length(),
						*(SScene.Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward.IsZero()? FString("False") : FString("ture")),
						(SScene.Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward - SScene.Scene.ActorLocationScene.FloorAndRoof.Floor).Length(),
						
						SScene.Scene.ActorLocationScene.FloorAndRoof.HeightFromRoof,
						*(SScene.Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward.IsZero()? FString("False") : FString("ture")),
						(SScene.Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward - SScene.Scene.ActorLocationScene.FloorAndRoof.Roof).Length(),
						*(SScene.Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward.IsZero()? FString("False") : FString("ture")),
						(SScene.Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward - SScene.Scene.ActorLocationScene.FloorAndRoof.Roof).Length(),
						
						SScene.Scene.TargetLocationScene.Height,
						SScene.Scene.TargetLocationScene.Right,
						SScene.Scene.TargetLocationScene.Forward,
						SScene.Scene.TargetLocationScene.Angle,
						*(StaticEnum<EGBWFlowMoveDirectionType>()->GetNameStringByIndex( static_cast<int32>(SScene.Scene.TargetLocationScene.SceneDirectionMark))),
						SScene.Scene.TargetLocationScene.Direction,
						SScene.Scene.TargetLocationScene.FloorAndRoof.HeightFromGround,SScene.Scene.TargetLocationScene.FloorAndRoof.HeightFromRoof,
						(SScene.Scene.LeftLocationScene.Point
						- SScene.Scene.ActorLocationScene.Point).Length(),
						*SScene.Scene.LeftLocationScene.Point.ToString(),
						(SScene.Scene.RightLocationScene.Point
						- SScene.Scene.ActorLocationScene.Point).Length(),
						*SScene.Scene.RightLocationScene.Point.ToString()
					);
				}
			}
			GEngine->AddOnScreenDebugMessage(6+i+KeyBase,1,FColor::Orange,SupplementaryFlowMoveSceneStr);
		}

		if (FlowMoveScene && DrawGraphFlowMoveSceneTag.IsValid())
		{
			FGBWFlowMoveScene Scene;
			if (TaskState.GetSupplementaryScene(DrawGraphFlowMoveSceneTag,Scene))
			{
				DrawDebugSphere(
					TaskState.OwnerCharacter->GetWorld(),
					Scene.TargetLocationScene.Point,
					20.0f,
					2,
					FColor::Purple,
					false,
					0.0f,
					0,
					3.0f
				);
				DrawDebugString(
					TaskState.OwnerCharacter->GetWorld(),
					Scene.TargetLocationScene.Point+FVector(0,0,10.0f),
					FString::Printf(TEXT("TargetPoint")),
					nullptr,
					FColor::Purple,
					0.0f,
					true,
					1.2f
					);

				if (!Scene.ActorLocationScene.FloorAndRoof.Floor.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.Floor,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}
				if (!Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}
				if (!Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}

				if (!Scene.ActorLocationScene.FloorAndRoof.Roof.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.Roof,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}
				if (!Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}
				if (!Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward.IsZero())
				{
					DrawDebugSphere(this->GetWorld(),
					Scene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward,
					30.0f,
					10,
					FColor::Purple,false,-1,0,3);
				}
			}
		}
		
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + TaskState.OwnerCharacter->GetCapsuleComponent()->GetForwardVector() *100.0f,
			50.0f,
			FColor::Silver,
			false,
			0.0f,
			0,
			3.0f
		);
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + TaskState.ForwardVector *100.0f,
			50.0f,
			FColor::Orange,
			false,
			0.0f,
			0,
			3.0f
		);
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + TaskState.ControlVector *100.0f,
			50.0f,
			FColor::Blue,
			false,
			0.0f,
			0,
			3.0f
		);
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + FVector(TaskState.ControlVector.X,TaskState.ControlVector.Y,0)*100.0f,
			50.0f,
			FColor::Red,
			false,
			0.0f,
			0,
			3.0f
		);
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + FVector(TaskState.ControlVector.X,TaskState.ControlVector.Y,0).Rotation().RotateVector(TaskState.MoveVector)*100.0f,
			50.0f,
			FColor::Yellow,
			false,
			0.0f,
			0,
			3.0f
		);
		
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + (UKismetMathLibrary::TransformDirection(TaskState.OwnerCharacter->GetTransform(),TaskState.PerceptionVector))*100.0f,
			50.0f,
			FColor::Green,
			false,
			0.0f,
			0,
			3.0f
		);

		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + TaskState.MoveControlParam.MoveToDirection*200.0f,
			50.0f,
			FColor::Magenta,
			false,
			0.0f,
			0,
			3.0f
		);

		FVector VelocityVector = TaskState.OwnerCharacter->GetVelocity();
		VelocityVector.Normalize();
		DrawDebugDirectionalArrow(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint,
			StartPoint + VelocityVector*200.0f,
			50.0f,
			FColor::Blue,
			false,
			0.0f,
			0,
			3.0f
		);
		DrawDebugString(
			TaskState.OwnerCharacter->GetWorld(),
			StartPoint + VelocityVector*200.0f,
			FString::Printf(TEXT("Velocity: %.2f"), TaskState.OwnerCharacter->GetVelocity().Length()),
			nullptr,
			FColor::Blue,
			0.0f,
			true,
			1.2f
		);
		
		DrawDebugCapsule(
			TaskState.OwnerCharacter->GetWorld(),
			TaskState.OwnerCharacter->GetCapsuleComponent()->GetComponentLocation(),
			TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
			TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius(),
			TaskState.OwnerCharacter->GetCapsuleComponent()->GetComponentRotation().Quaternion(),
			FColor::Orange,
			false,
			0.0f,
			0,
			3.0f
		);

		if (TaskState.OwnerCharacter->GetMovementComponent()->bConstrainToPlane)
		{
			DrawDebugSolidPlane(
				TaskState.OwnerCharacter->GetWorld(),
				FPlane(TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintOrigin(),TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintNormal()),
				TaskState.OwnerCharacter->GetMovementComponent()->GetPlaneConstraintOrigin(),
				200.0f,
				FColor::Silver,
				false,
				-1,
				0
			);
		}

		if (TaskState.FlowMoveComponent->IsActionNow())
		{
			DrawDebugSphere(this->GetWorld(),
					TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.TargetLocationScene.Point,
					30.0f,
					10,
					FColor::Orange,false,-1,0,3);
			DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.Point,
				30.0f,
				10,
				FColor::Red,false,-1,0,3);
			
			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Floor,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}
			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Forward,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}
			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.FloorLedge_Backward,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}

			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.Roof,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}
			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Forward,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}
			if (!TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward.IsZero())
			{
				DrawDebugSphere(this->GetWorld(),
				TaskState.FlowMoveComponent->
				CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene.ActorLocationScene.FloorAndRoof.RoofLedge_Backward,
				30.0f,
				10,
				FColor::Purple,false,-1,0,3);
			}
		}
	}
}

void UGBWFlowMoveScript_AdjustCapsule::OnUpdate_Implementation(
	EGBWFlowMoveEventType EventType,
	EFlowMoveNetworkLocationType NetworkLocation,
	const bool bIsLocalOwn,
	UGBWFlowMoveComponent* FlowMoveComponent)
{
	if (NetworkLocation == EFlowMoveNetworkLocationType::Server && FlowMoveComponent)
	{
		if (EventType == EGBWFlowMoveEventType::End)
		{
			LastTargetHeight = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
			FlowMoveComponent->ResetCapsuleAndMesh();
			return;
		}

		if (EventType == EGBWFlowMoveEventType::Init)
		{
			LastTargetHeight = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
		}

		FGBWFlowMoveScene TargetScene;
		bool bIsGetScene = false;
		if (FlowMoveComponent->IsActionNow() && FlowMoveComponent->GetMoveControlParam().ToTargetPointRate > 0)
		{
			TargetScene = FlowMoveComponent->CurrentActionTargetSceneInfo.BeginFlowMoveState.CurrentActionTargetScene;
			bIsGetScene = true;
		}
		else if (FlowMoveComponent->GetScene(TargetScene, DefaultTargetSceneSlot,false))
		{
			bIsGetScene = true;
		}

		if (!bIsGetScene)
		{
			LastTargetHeight = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
			FlowMoveComponent->ResetCapsuleAndMesh();
			return;
		}
		
		float TargetHeight = AdjustTo;

		if (Type == EFlowMoveAdjustCapsuleType::AdjustToValidSpace)
		{
			float TargetHeight_Current = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
			float TargetHeight_Target = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
			const float TargetHeight_InAction = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
		
			if (ByCurrentLocation)
			{
				if (!TargetScene.ActorLocationScene.FloorAndRoof.Floor.IsNearlyZero() && !TargetScene.ActorLocationScene.FloorAndRoof.Roof.IsNearlyZero())
				{
					TargetHeight_Current = (TargetScene.ActorLocationScene.FloorAndRoof.Roof - TargetScene.ActorLocationScene.FloorAndRoof.Floor).Z * AdjustToValidSpaceFraction/100.0f;
				}
			}

			if (ByTargetLocation)
			{
				if (FlowMoveComponent->TaskState.CurrentFlowMoveActionTag.IsValid())
				{
					if (!TargetScene.TargetLocationScene.FloorAndRoof.Floor.IsNearlyZero() && !TargetScene.TargetLocationScene.FloorAndRoof.Roof.IsNearlyZero())
					{
						TargetHeight_Current = (TargetScene.TargetLocationScene.FloorAndRoof.Roof - TargetScene.TargetLocationScene.FloorAndRoof.Floor).Z * AdjustToValidSpaceFraction/100.0f;
					}
				}
			}
		
			TargetHeight = FMath::Min(FMath::Min(TargetHeight_Current, TargetHeight_Target),TargetHeight_InAction);
		}
		
		TargetHeight = FMath::Max(TargetHeight, MinHeightCanAdjustTo);
		if (TargetHeight >= FlowMoveComponent->CapsuleHalfHeightCache * 2.0f
			&& TargetHeight>FlowMoveComponent->TaskState.OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2)
		{
			LastTargetHeight = FlowMoveComponent->CapsuleHalfHeightCache * 2.0f;
			FlowMoveComponent->ResetCapsuleAndMesh();
		}
		else
		{
			LastTargetHeight = UKismetMathLibrary::FInterpTo(LastTargetHeight,TargetHeight,FlowMoveComponent->TaskState.FrameDeltaTime,InterpSpeed);
			FlowMoveComponent->SetCapsuleAndKeepMesh(LastTargetHeight);
		}
	}
}