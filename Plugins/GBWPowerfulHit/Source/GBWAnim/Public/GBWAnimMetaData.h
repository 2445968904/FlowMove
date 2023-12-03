// Copyright GanBowen 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "GBWAnimMetaData.generated.h"

UCLASS(meta = (DisplayName = "GBW_MainAssetSetting"))
class GBWANIM_API UGBWAnimMetaData_MainAssetSetting : public UAnimMetaData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Settings")
	int MainAssetPriorityLevel = -1;
};
