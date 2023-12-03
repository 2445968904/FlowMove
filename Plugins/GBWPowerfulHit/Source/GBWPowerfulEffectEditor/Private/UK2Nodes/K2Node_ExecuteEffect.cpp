// Copyright GanBowen 2022-2023. All Rights Reserved.


#include "UK2Nodes/K2Node_ExecuteEffect.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "UObject/UnrealType.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintCompilationManager.h"
#include "FindInBlueprintManager.h"
#include "K2Node_CallFunction.h"
#include "K2Node_MakeArray.h"
#include "KismetCompiler.h"
#include "KismetCompilerMisc.h"
#include "GBWPowerfulEffect/Public/GBWEffectActorBase.h"
#include "GBWPowerfulEffect/Public/GBWPowerfulEffectComponent.h"
#include "GBWPowerfulEffect/Public/GBWPowerfulEffectFuncLib.h"
#include "KismetCompilerMisc.h"

struct FK2Node_ExecuteEffectHelper
{
	static FName GetWorldContextPinName()
	{
		return TEXT("WorldContextObject");
	}

	static FName GetClassPinName()
	{
		return TEXT("EffectClass");
	}

	static FName GetOuterPinFriendlyName()
	{
		return TEXT("Outer");
	}

	static FName GetOuterPinName()
	{
		return UEdGraphSchema_K2::PN_Self;
	}

	static FName GetIsReplicatePinName()
	{
		return FName("IsReplicate");
	}

	static FName GetIsOnlyTriggerFromServerPinName()
	{
		return FName("IsOnlyTriggerFromServer");
	}

	static FName GetOwnerActorPinName()
	{
		return FName("OwnerActor");
	}

	static FName GetSpawnSettingsPinName()
	{
		return FName("SpawnSettings");
	}

	static FName GetEffectGuidPinName()
	{
		return FName("EffectGuid");
	}
};

#define LOCTEXT_NAMESPACE "UK2Node_ExecuteEffect"

UK2Node_ExecuteEffect::UK2Node_ExecuteEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Execute A GBW Effect");
}

UClass* UK2Node_ExecuteEffect::GetClassPinBaseClass() const
{
	return AGBWEffectActorBase::StaticClass();
}

bool UK2Node_ExecuteEffect::UseWorldContext() const
{
	const UBlueprint* BP = GetBlueprint();
	const UClass* ParentClass = BP ? BP->ParentClass : nullptr;
	return ParentClass ? ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin) != nullptr : false;
}

void UK2Node_ExecuteEffect::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// If required add the world context pin
	if (UseWorldContext())
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), FK2Node_ExecuteEffectHelper::GetWorldContextPinName());
	}

	// Add blueprint pin
	UEdGraphPin* ClassPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, GetClassPinBaseClass(), FK2Node_ExecuteEffectHelper::GetClassPinName());
	
	// EffectGuid Pin
	UScriptStruct* EffectGuidStruct = TBaseStructure<FGuid>::Get();
	UEdGraphPin* EffectGuidPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, EffectGuidStruct, FK2Node_ExecuteEffectHelper::GetEffectGuidPinName());
	
	if (UseOuter())
	{
		UEdGraphPin* OuterPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), UEdGraphSchema_K2::PN_Self);
		OuterPin->PinFriendlyName = FText::FromName(FK2Node_ExecuteEffectHelper::GetOuterPinFriendlyName());
	}

	//Replicated pin
	UEdGraphPin* IsReplicatedPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FK2Node_ExecuteEffectHelper::GetIsReplicatePinName());
	UEdGraphPin* IsOnlyTriggerFromServerPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, FK2Node_ExecuteEffectHelper::GetIsOnlyTriggerFromServerPinName());
	
	// Owner Actor pin
	UEdGraphPin* OwnerPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), FK2Node_ExecuteEffectHelper::GetOwnerActorPinName());
	
	// SpawnSettings pin
	UScriptStruct* SpawnStruct = TBaseStructure<FGBWSpawnRules>::Get();
	UEdGraphPin* SpawnSettingsPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, SpawnStruct, FK2Node_ExecuteEffectHelper::GetSpawnSettingsPinName());


	Super::AllocateDefaultPins();
}

UEdGraphPin* UK2Node_ExecuteEffect::GetOuterPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetOuterPinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetIsReplicatedPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetIsReplicatePinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetIsOnlyTriggerFromServerPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetIsOnlyTriggerFromServerPinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetOwnerActorPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetOwnerActorPinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetSpawnSettingsPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetSpawnSettingsPinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetEffectGuidPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetEffectGuidPinName());
	ensure(nullptr == Pin || Pin->Direction == EGPD_Output);
	return Pin;
}

FString UK2Node_ExecuteEffect::GetSpawnAssignmentParameters(const FKismetCompilerContext& CompilerContext, UEdGraphNode* SpawnNode, const UClass* ForClass) const
{
	FString Res = "";
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	// Create 'set var by name' nodes and hook them up
	for (int32 PinIdx = 0; PinIdx < SpawnNode->Pins.Num(); PinIdx++)
	{
		// Only create 'set param by name' node if this pin is linked to something
		UEdGraphPin* OrgPin = SpawnNode->Pins[PinIdx];
		const bool bHasDefaultValue = !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;
		if (OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue)
		{
			if( OrgPin->LinkedTo.Num() == 0 )
			{
				FProperty* Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
				// NULL property indicates that this pin was part of the original node, not the 
				// class we're assigning to:
				if( !Property )
				{
					continue;
				}

				// We don't want to generate an assignment node unless the default value 
				// differs from the value in the CDO:
				FString DefaultValueAsString;
					
				if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
				{
					if (Schema->DoesDefaultValueMatch(*OrgPin, DefaultValueAsString))
					{
						continue;
					}
				}
				else if(ForClass->ClassDefaultObject)
				{
					FBlueprintEditorUtils::PropertyValueToString(Property, (uint8*)ForClass->ClassDefaultObject, DefaultValueAsString);

					if (DefaultValueAsString == OrgPin->GetDefaultAsString())
					{
						continue;
					}
				}
			}
			
			if (Res != "")
			{
				Res = Res + " ; ";
			}
			
			Res = Res + FString::Printf(TEXT("%s : %s : %s")
			,*OrgPin->PinName.ToString()
			,*OrgPin->PinType.PinCategory.ToString()
			,*OrgPin->GetDefaultAsString()
			);
		}
	}

	return Res;
}

void UK2Node_ExecuteEffect::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

void UK2Node_ExecuteEffect::CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins)
{
	check(InClass);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	const UObject* const ClassDefaultObject = InClass->GetDefaultObject(false);

	for (TFieldIterator<FProperty> PropertyIt(InClass, EFieldIteratorFlags::IncludeSuper); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		const bool bIsDelegate = Property->IsA(FMulticastDelegateProperty::StaticClass());
		const bool bIsExposedToSpawn = UEdGraphSchema_K2::IsPropertyExposedOnSpawn(Property);
		const bool bIsSettableExternally = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);

		if(	bIsExposedToSpawn &&
			!Property->HasAnyPropertyFlags(CPF_Parm) && 
			bIsSettableExternally &&
			Property->HasAllPropertyFlags(CPF_BlueprintVisible) &&
			!bIsDelegate &&
			(nullptr == FindPin(Property->GetFName()) ) &&
			FBlueprintEditorUtils::PropertyStillExists(Property))
		{
			if (UEdGraphPin* Pin = CreatePin(EGPD_Input, NAME_None, Property->GetFName()))
			{
				K2Schema->ConvertPropertyToPinType(Property, /*out*/ Pin->PinType);
				if (OutClassPins)
				{
					OutClassPins->Add(Pin);
				}

				if (ClassDefaultObject && K2Schema->PinDefaultValueIsEditable(*Pin))
				{
					FString DefaultValueAsString;
					const bool bDefaultValueSet = FBlueprintEditorUtils::PropertyValueToString(Property, reinterpret_cast<const uint8*>(ClassDefaultObject), DefaultValueAsString, this);
					check(bDefaultValueSet);
					K2Schema->SetPinAutogeneratedDefaultValue(Pin, DefaultValueAsString);
				}

				// Copy tooltip from the property.
				K2Schema->ConstructBasicPinTooltip(*Pin, Property->GetToolTipText(), Pin->PinToolTip);
			}
		}
	}
}

UClass* UK2Node_ExecuteEffect::GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch /*=NULL*/) const
{
	UClass* UseSpawnClass = nullptr;
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* ClassPin = GetClassPin(PinsToSearch);
	if (ClassPin && ClassPin->DefaultObject && ClassPin->LinkedTo.Num() == 0)
	{
		UseSpawnClass = CastChecked<UClass>(ClassPin->DefaultObject);
	}
	else if (ClassPin && ClassPin->LinkedTo.Num())
	{
		const UEdGraphPin* ClassSource = ClassPin->LinkedTo[0];
		UseSpawnClass = ClassSource ? Cast<UClass>(ClassSource->PinType.PinSubCategoryObject.Get()) : nullptr;
	}

	return UseSpawnClass;
}

void UK2Node_ExecuteEffect::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	AllocateDefaultPins();
	if (UClass* UseSpawnClass = GetClassToSpawn(&OldPins))
	{
		CreatePinsForClass(UseSpawnClass);
	}
	RestoreSplitPins(OldPins);
}

UK2Node::ERedirectType UK2Node_ExecuteEffect::DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	// the name of the outer pin was changed and its friendly name was updated to match
	// the legacy naming. Use this to identify the change
	if (NewPin->PinName == FK2Node_ExecuteEffectHelper::GetOuterPinName() &&
		OldPin->PinName == FK2Node_ExecuteEffectHelper::GetOuterPinFriendlyName())
	{
		return ERedirectType_Name;
	}
	return Super::DoPinsMatchForReconstruction(NewPin, NewPinIndex, OldPin, OldPinIndex);
}

void UK2Node_ExecuteEffect::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	if (UClass* UseSpawnClass = GetClassToSpawn())
	{
		CreatePinsForClass(UseSpawnClass);
	}
}

void UK2Node_ExecuteEffect::AddSearchMetaDataInfo(TArray<struct FSearchTagDataPair>& OutTaggedMetaData) const
{
	Super::AddSearchMetaDataInfo(OutTaggedMetaData);
	OutTaggedMetaData.Add(FSearchTagDataPair(FFindInBlueprintSearchTags::FiB_NativeName, CachedNodeTitle.GetCachedText()));
}

FSlateIcon UK2Node_ExecuteEffect::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.SpawnActor_16x");
	return Icon;
}

bool UK2Node_ExecuteEffect::IsSpawnVarPin(UEdGraphPin* Pin) const
{
	if (Pin->ParentPin)
	{
		return IsSpawnVarPin(Pin->ParentPin);
	}

	return(	Pin->PinName != UEdGraphSchema_K2::PN_Execute &&
			Pin->PinName != UEdGraphSchema_K2::PN_Then &&
			Pin->PinName != UEdGraphSchema_K2::PN_ReturnValue &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetClassPinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetWorldContextPinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetOuterPinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetIsReplicatePinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetIsOnlyTriggerFromServerPinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetOwnerActorPinName() &&
			Pin->PinName != FK2Node_ExecuteEffectHelper::GetSpawnSettingsPinName());
}

void UK2Node_ExecuteEffect::OnClassPinChanged()
{
	// Remove all pins related to archetype variables
	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldClassPins;

	for (UEdGraphPin* OldPin : OldPins)
	{
		if (IsSpawnVarPin(OldPin))
		{
			Pins.Remove(OldPin);
			OldClassPins.Add(OldPin);
		}
	}

	CachedNodeTitle.MarkDirty();

	TArray<UEdGraphPin*> NewClassPins;
	if (UClass* UseSpawnClass = GetClassToSpawn())
	{
		CreatePinsForClass(UseSpawnClass, &NewClassPins);
	}

	RestoreSplitPins(OldPins);

	// Rewire the old pins to the new pins so connections are maintained if possible
	RewireOldPinsToNewPins(OldClassPins, Pins, nullptr);

	// Refresh the UI for the graph so the pin changes show up
	GetGraph()->NotifyGraphChanged();

	// Mark dirty
	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

void UK2Node_ExecuteEffect::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin && (Pin->PinName == FK2Node_ExecuteEffectHelper::GetClassPinName()))
	{
		OnClassPinChanged();
	}
}

void UK2Node_ExecuteEffect::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (UEdGraphPin* ClassPin = GetClassPin())
	{
		SetPinToolTip(*ClassPin, LOCTEXT("ClassPinDescription", "The Effect class you want to Execute"));
	}
	if (UEdGraphPin* OuterPin = (UseOuter() ? GetOuterPin() : nullptr))
	{
		SetPinToolTip(*OuterPin, LOCTEXT("OuterPinDescription", "Owner of the Effect object"));
	}

	return Super::GetPinHoverText(Pin, HoverTextOut);
}

void UK2Node_ExecuteEffect::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && (ChangedPin->PinName == FK2Node_ExecuteEffectHelper::GetClassPinName()))
	{
		OnClassPinChanged();
	}
}

FText UK2Node_ExecuteEffect::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetThenPin()const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == FK2Node_ExecuteEffectHelper::GetClassPinName())
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ExecuteEffect::GetWorldContextPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ExecuteEffectHelper::GetWorldContextPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_ExecuteEffect::GetBaseNodeTitle() const
{
	return NSLOCTEXT("K2Node", "ExecuteEffect_BaseTitle", "Execute Effect from Class");
}

FText UK2Node_ExecuteEffect::GetDefaultNodeTitle() const
{
	return NSLOCTEXT("K2Node", "ExecuteEffect_Title_NONE", "Execute NONE");
}

FText UK2Node_ExecuteEffect::GetNodeTitleFormat() const
{
	return NSLOCTEXT("K2Node", "ExecuteEffect", "Execute {ClassName}");
}

FText UK2Node_ExecuteEffect::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		return GetBaseNodeTitle();
	}
	else if (const UClass* ClassToSpawn = GetClassToSpawn())
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("ClassName"), ClassToSpawn->GetDisplayNameText());
			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitle.SetCachedText(FText::Format(GetNodeTitleFormat(), Args), this);
		}
		return CachedNodeTitle;
	}
	return GetDefaultNodeTitle();
}

bool UK2Node_ExecuteEffect::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const 
{
	const UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || (FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph && Blueprint->GeneratedClass->GetDefaultObject()->ImplementsGetWorld()));
}

void UK2Node_ExecuteEffect::GetNodeAttributes( TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes ) const
{
	const UClass* ClassToSpawn = GetClassToSpawn();
	const FString ClassToSpawnStr = ClassToSpawn ? ClassToSpawn->GetName() : TEXT( "InvalidClass" );
	OutNodeAttributes.Add( TKeyValuePair<FString, FString>( TEXT( "Type" ), TEXT( "ExecuteEffectFromClass" ) ));
	OutNodeAttributes.Add( TKeyValuePair<FString, FString>( TEXT( "Class" ), GetClass()->GetName() ));
	OutNodeAttributes.Add( TKeyValuePair<FString, FString>( TEXT( "Name" ), GetName() ));
	OutNodeAttributes.Add( TKeyValuePair<FString, FString>( TEXT( "ActorClass" ), ClassToSpawnStr ));
}

void UK2Node_ExecuteEffect::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	const UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ExecuteEffect::GetMenuCategory() const
{
	return FText::FromString("GBW|Effect");
}

FNodeHandlingFunctor* UK2Node_ExecuteEffect::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	return new FNodeHandlingFunctor(CompilerContext);
}

bool UK2Node_ExecuteEffect::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const
{
	UClass* SourceClass = GetClassToSpawn();
	const UBlueprint* SourceBlueprint = GetBlueprint();
	const bool bResult = (SourceClass && (SourceClass->ClassGeneratedBy != SourceBlueprint));
	if (bResult && OptionalOutput)
	{
		OptionalOutput->AddUnique(SourceClass);
	}
	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bSuperResult || bResult;
}

FText UK2Node_ExecuteEffect::GetKeywords() const
{
	return LOCTEXT("ExecuteEffect", "Execute Effect");
}

void UK2Node_ExecuteEffect::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	static const FName ExecuteEffectFuncName = GET_FUNCTION_NAME_CHECKED(UGBWPowerfulEffectFuncLib, ExecuteEffect);
	static const FName InActorParamName(TEXT("InActor"));
	static const FName EffectClassParamName(TEXT("EffectClass"));
	static const FName EffectSpawnSettingsParamName(TEXT("EffectSpawnSettings"));
	static const FName IsReplicatedParamName(TEXT("bIsReplicated"));
	static const FName IsOnlyTriggerFromServerParamName(TEXT("bOnlyTriggerFromServer"));
	static const FName ParametersParamName(TEXT("Parameters"));
	static const FName EffectGuidParamName(TEXT("EffectGuid"));
	
	UK2Node_ExecuteEffect* SpawnNode = this;
	UEdGraphPin* SpawnNodeExec = SpawnNode->GetExecPin();
	
	UEdGraphPin* SpawnNodeIsReplicated = SpawnNode->GetIsReplicatedPin();
	UEdGraphPin* SpawnNodeIsOnlyTriggerFromServer = SpawnNode->GetIsOnlyTriggerFromServerPin();
	UEdGraphPin* SpawnNodeOwnerPin = SpawnNode->GetOwnerActorPin();
	UEdGraphPin* SpawnNodeSpawnSettings = SpawnNode->GetSpawnSettingsPin();
	
	UEdGraphPin* SpawnClassPin = SpawnNode->GetClassPin();
	UEdGraphPin* SpawnNodeThen = SpawnNode->GetThenPin();
	UEdGraphPin* SpawnNodeEffectGuid = SpawnNode->GetEffectGuidPin();

	UClass* SpawnClass = (SpawnClassPin != nullptr) ? Cast<UClass>(SpawnClassPin->DefaultObject) : nullptr;
	if ( !SpawnClassPin || ((0 == SpawnClassPin->LinkedTo.Num()) && (nullptr == SpawnClass)))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ExecuteEffectMissingClass_Error", "ExecuteEffect @@ must have a @@ specified.").ToString(), SpawnNode, SpawnClassPin);
		// we break exec links so this is the only error we get, don't want the SpawnActor node being considered and giving 'unexpected node' type warnings
		SpawnNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'begin spawn' call node
	UK2Node_CallFunction* CallExecuteEffectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
	CallExecuteEffectNode->FunctionReference.SetExternalMember(ExecuteEffectFuncName, UGBWPowerfulEffectFuncLib::StaticClass());
	CallExecuteEffectNode->AllocateDefaultPins();

	UEdGraphPin* CallExecuteEffectNodeExec = CallExecuteEffectNode->GetExecPin();
	UEdGraphPin* CallExecuteEffectNodeThen = CallExecuteEffectNode->GetThenPin();
	UEdGraphPin* CallExecuteEffectNodeInActorPin = CallExecuteEffectNode->FindPinChecked(InActorParamName);
	UEdGraphPin* CallExecuteEffectNodeEffectClassPin = CallExecuteEffectNode->FindPinChecked(EffectClassParamName);
	UEdGraphPin* CallExecuteEffectNodeEffectSpawnSettingsPin = CallExecuteEffectNode->FindPinChecked(EffectSpawnSettingsParamName);
	UEdGraphPin* CallExecuteEffectNodeIsReplicatedPin = CallExecuteEffectNode->FindPinChecked(IsReplicatedParamName);
	UEdGraphPin* CallExecuteEffectNodeOnlyTriggerFromServerPin = CallExecuteEffectNode->FindPinChecked(IsOnlyTriggerFromServerParamName);
	UEdGraphPin* CallExecuteEffectNodeParametersPin = CallExecuteEffectNode->FindPinChecked(ParametersParamName);
	UEdGraphPin* CallExecuteEffectNodeEffectGuidPin = CallExecuteEffectNode->FindPinChecked(EffectGuidParamName);

	// Cache the class to spawn. Note, this is the compile time class that the pin was set to or the variable type it was connected to. Runtime it could be a child.
	const UClass* ClassToSpawn = GetClassToSpawn();

	// Move 'exec' connection from spawn node to 'ExecuteEffect'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallExecuteEffectNodeExec);

	if(SpawnClassPin->LinkedTo.Num() > 0)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallExecuteEffectNodeEffectClassPin);
	}
	else
	{
		CallExecuteEffectNodeEffectClassPin->DefaultObject = SpawnClass;
	}

	if (SpawnNodeOwnerPin != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeOwnerPin, *CallExecuteEffectNodeInActorPin);
	}
	if (SpawnNodeIsReplicated != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeIsReplicated, *CallExecuteEffectNodeIsReplicatedPin);
	}
	if (SpawnNodeIsOnlyTriggerFromServer != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeIsOnlyTriggerFromServer, *CallExecuteEffectNodeOnlyTriggerFromServerPin);
	}
	if (SpawnNodeSpawnSettings != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeSpawnSettings, *CallExecuteEffectNodeEffectSpawnSettingsPin);
	}
	if (SpawnNodeEffectGuid != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeEffectGuid, *CallExecuteEffectNodeEffectGuidPin);
	}

	//CallExecuteEffectNodeParametersPin->DefaultValue = GetSpawnAssignmentParameters(CompilerContext, SpawnNode, ClassToSpawn);

	// Move 'then' connection from spawn node to 'ExecuteEffectNodeThen'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *CallExecuteEffectNodeThen);

	// create 'set var' nodes

	/*// Get 'result' pin from 'begin spawn', this is the actual actor we want to set properties on
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallExecuteEffectNode, SpawnNode, CallBeginResult, ClassToSpawn );

	// Make exec connection between 'then' on last node and 'finish'
	LastThen->MakeLinkTo(CallExecuteEffectNodeThen);*/

	// Break any links to the expanded node
	SpawnNode->BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
