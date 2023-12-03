// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GBWNumericGeneratorScript.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GBWNGDataAsset.generated.h"

UCLASS(BlueprintType, meta = (DisplayName = "NGSetDA"))
class GBWNUMERICGENERATOR_API UGBWNGDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FString Description = "No Description";

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings", meta=(ForceInlineRow, TitleProperty="Description"))
	TMap<FString, FNG_Set> NGSet;
	
	UGBWNumericGenerator_Value_Base* GetNGO(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag);
	
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	bool GetValue_Bool(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  bool DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	float GetValue_Float(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  float DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FVector GetValue_Vector(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  FVector DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FRotator GetValue_Rotator(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  FRotator DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FTransform GetValue_Transform(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  FTransform DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FGameplayTag GetValue_GameplayTag(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  FGameplayTag DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FText GetValue_Text(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag, FText DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FString GetValue_String(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag, FString DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	FName GetValue_Name(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag, FName DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UCurveFloat* GetValue_CurveFloat(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UCurveFloat* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UCurveVector* GetValue_CurveVector(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UCurveVector* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UAnimMontage* GetValue_Montage(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimMontage* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UAnimSequence* GetValue_AnimSequence(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimSequence* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UBlendSpace* GetValue_AnimBlendSpace(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UBlendSpace* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UAnimBlueprint* GetValue_AnimBlueprint(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UAnimBlueprint* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	USkeletalMesh* GetValue_SkeletalMesh(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  USkeletalMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UStaticMesh* GetValue_StaticMesh(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UStaticMesh* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	UNiagaraSystem* GetValue_Niagara(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  UNiagaraSystem* DefaultValue);
	UFUNCTION(BlueprintCallable, Category = "GBW|NG|Structure", meta=(DefaultToSelf="OwnerActor"))
	USoundBase* GetValue_Sound(AActor* OwnerActor, FGameplayTag GroupTag, FGameplayTag ValueTag,  USoundBase* DefaultValue);
};

UENUM(BlueprintType)
enum class EGBWNGElementType :uint8
{
	DirectlyGenerator UMETA(DisplayName = "DirectlyValue"),
	GeneratorFromDA UMETA(DisplayName = "GeneratorFromDA")
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Bool"))
struct FNGV_Bool
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	bool DefaultValue = false;

	bool GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Bool(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Bool(): NGSetDA(nullptr){}
	FNGV_Bool(bool InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Float"))
struct FNGV_Float
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	float DefaultValue = 0.0f;

	float GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Float(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Float(): NGSetDA(nullptr){}
	FNGV_Float(float InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Vector"))
struct FNGV_Vector
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FVector DefaultValue = FVector::ZeroVector;

	FVector GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Vector(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Vector(): NGSetDA(nullptr){}
	FNGV_Vector(const FVector& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Rotator"))
struct FNGV_Rotator
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FRotator DefaultValue = FRotator::ZeroRotator;

	FRotator GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Rotator(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Rotator(): NGSetDA(nullptr){}
	FNGV_Rotator(const FRotator& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Transform"))
struct FNGV_Transform
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FTransform DefaultValue = FTransform();

	FTransform GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Transform(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Transform(): NGSetDA(nullptr){}
	FNGV_Transform(const FTransform& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_GameplayTag"))
struct FNGV_GameplayTag
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FGameplayTag DefaultValue = FGameplayTag();

	FGameplayTag GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_GameplayTag(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_GameplayTag(): NGSetDA(nullptr){}
	FNGV_GameplayTag(const FGameplayTag& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_String"))
struct FNGV_String
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FString DefaultValue = FString();

	FString GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_String(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_String(): NGSetDA(nullptr){}
	FNGV_String(const FString& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Text"))
struct FNGV_Text
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FText DefaultValue = FText();

	FText GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Text(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Text(): NGSetDA(nullptr){}
	FNGV_Text(const FText& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Name"))
struct FNGV_Name
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	FName DefaultValue = FName();

	FName GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Name(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
	FNGV_Name(): NGSetDA(nullptr){}
	FNGV_Name(const FName& InitValue): NGSetDA(nullptr) { DefaultValue = InitValue; }
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_CurveFloat"))
struct FNGV_CurveFloat
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UCurveFloat* DefaultValue = nullptr;

	UCurveFloat* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_CurveFloat(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_CurveVector"))
struct FNGV_CurveVector
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UCurveVector* DefaultValue = nullptr;

	UCurveVector* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_CurveVector(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Montage"))
struct FNGV_Montage
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UAnimMontage* DefaultValue = nullptr;

	UAnimMontage* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Montage(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_AnimSequence"))
struct FNGV_AnimSequence
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UAnimSequence* DefaultValue = nullptr;

	UAnimSequence* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_AnimSequence(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_AnimBlendSpace"))
struct FNGV_AnimBlendSpace
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UBlendSpace* DefaultValue = nullptr;

	UBlendSpace* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_AnimBlendSpace(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_AnimBlueprint"))
struct FNGV_AnimBlueprint
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UAnimBlueprint* DefaultValue = nullptr;

	UAnimBlueprint* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_AnimBlueprint(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_SkeletalMesh"))
struct FNGV_SkeletalMesh
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	USkeletalMesh* DefaultValue = nullptr;

	USkeletalMesh* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_SkeletalMesh(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_StaticMesh"))
struct FNGV_StaticMesh
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UStaticMesh* DefaultValue = nullptr;

	UStaticMesh* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_StaticMesh(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Niagara"))
struct FNGV_Niagara
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	UNiagaraSystem* DefaultValue = nullptr;

	UNiagaraSystem* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Niagara(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};
USTRUCT(BlueprintType, meta = (DisplayName = "NGV_Sound"))
struct FNGV_Sound
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	EGBWNGElementType Type = EGBWNGElementType::DirectlyGenerator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	UGBWNGDataAsset* NGSetDA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag GroupTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings",
		meta=(EditConditionHides, EditCondition="Type == EGBWNGElementType::GeneratorFromDA"))
	FGameplayTag ValueTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	USoundBase* DefaultValue = nullptr;

	USoundBase* GetValue(AActor* OwnerActor) const
	{
		if (Type == EGBWNGElementType::DirectlyGenerator)
		{
			return DefaultValue;
		}
		
		if (NGSetDA)
		{
			return NGSetDA->GetValue_Sound(OwnerActor,GroupTag,ValueTag,DefaultValue);
		}
		return DefaultValue;
	}
};