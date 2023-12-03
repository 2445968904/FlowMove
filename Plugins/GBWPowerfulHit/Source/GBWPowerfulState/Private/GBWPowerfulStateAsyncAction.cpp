// Copyright GanBowen 2022-2023. All Rights Reserved.

#include "GBWPowerfulStateAsyncAction.h"

#include "GBWPSFuncLib.h"

UGBWPSAsync_ListenEvent::UGBWPSAsync_ListenEvent()
{
	WorldContextObject = nullptr;
}


UGBWPSAsync_ListenEvent::~UGBWPSAsync_ListenEvent()
{
	
}

UGBWPSAsync_ListenEvent* UGBWPSAsync_ListenEvent::PS_ListenEvent(
	AActor* InActor,
	FGameplayTagQuery EventToListen,
	bool bStopWhenEvent,
	FGameplayTagQuery StateToListen,
	bool bStopWhenState,
	float Timeout)
{
	UGBWPowerfulStateComponent* PSC = UGBWPSFuncLib::GetPowerfulStateComponent(InActor);
	if (!PSC)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid InActor. Cannot execute ListenEvent."), ELogVerbosity::Error);
		return nullptr;
	}

	UGBWPSAsync_ListenEvent* AsyncNode = NewObject<UGBWPSAsync_ListenEvent>();
	AsyncNode->WorldContextObject = InActor->GetWorld();

	FTimerDynamicDelegate Delegate;
	Delegate.BindUFunction(AsyncNode, FName("CompletedEvent"));
	AsyncNode->TimerHandle = AsyncNode->SetTimerDelegate(Delegate, Timeout);

	AsyncNode->PSC = PSC;
	AsyncNode->PSC->OnPowerfulEvent.AddDynamic(AsyncNode, &UGBWPSAsync_ListenEvent::OnPEEvent);
	AsyncNode->PSC->OnPowerfulState.AddDynamic(AsyncNode, &UGBWPSAsync_ListenEvent::OnPSEvent);
	AsyncNode->TheEventToListen = EventToListen;
	AsyncNode->bStopWhenEvent = bStopWhenEvent;
	AsyncNode->TheStateToListen = StateToListen;
	AsyncNode->bStopWhenState = bStopWhenState;
	
	AsyncNode->RegisterWithGameInstance(const_cast<UObject*>(AsyncNode->WorldContextObject));

	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddUObject(AsyncNode, &UGBWPSAsync_ListenEvent::PreGarbageCollect);
	
	return AsyncNode;
}

FTimerHandle UGBWPSAsync_ListenEvent::SetTimerDelegate(FTimerDynamicDelegate Delegate, float Time)
{
	FTimerHandle Handle;
	if (Delegate.IsBound())
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (World)
		{
			FTimerManager& TimerManager = World->GetTimerManager();
			Handle = TimerManager.K2_FindDynamicTimerHandle(Delegate);
			TimerManager.SetTimer(Handle, Delegate, Time, false);
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Warning,
			TEXT("SetTimer passed a bad function (%s) or object (%s)"),
			*Delegate.GetFunctionName().ToString(), *GetNameSafe(Delegate.GetUObject()));
	}

	return Handle;
}


void UGBWPSAsync_ListenEvent::PreGarbageCollect()
{
	if (bIsCompleted)
	{
		SetReadyToDestroy();
	}
}

void UGBWPSAsync_ListenEvent::Stop()
{
	if(TimerHandle.IsValid())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
    	TimerManager.ClearTimer(TimerHandle);
	}
	CompletedEvent();
}

void UGBWPSAsync_ListenEvent::Activate()
{
	if (!WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute ListenEvent."), ELogVerbosity::Error);
		return;
	}

	Then.Broadcast(this,Timer,FrameDeltaTime,FPE_Event(),FPS_Event());
}


void UGBWPSAsync_ListenEvent::CompletedEvent()
{
	if (bIsCompleted)
	{
		return;
	}
	bIsCompleted = true;
	PSC->OnPowerfulEvent.RemoveDynamic(this, &UGBWPSAsync_ListenEvent::OnPEEvent);
	PSC->OnPowerfulState.RemoveDynamic(this, &UGBWPSAsync_ListenEvent::OnPSEvent);

	if (TimerHandle.IsValid())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(TimerHandle);
	}
	
	Completed.Broadcast(this,Timer,FrameDeltaTime,FPE_Event(),FPS_Event());
}

void UGBWPSAsync_ListenEvent::OnPEEvent(FGameplayTag EventKey, FPE_Event PE_Event)
{
	if (TheEventToListen.IsEmpty() || TheEventToListen.Matches(PE_Event.Key.GetSingleTagContainer()))
	{
		OnEvent.Broadcast(this,Timer,FrameDeltaTime,PE_Event,FPS_Event());

		if (bStopWhenEvent)
		{
			CompletedEvent();
		}
	}
}

void UGBWPSAsync_ListenEvent::OnPSEvent(FGameplayTag StateKey, FPS_Event PS_Event)
{
	if (TheStateToListen.IsEmpty() || TheStateToListen.Matches(PS_Event.StateKey.GetSingleTagContainer()))
	{
		OnState.Broadcast(this,Timer,FrameDeltaTime,FPE_Event(),PS_Event);

		if (bStopWhenState)
		{
			CompletedEvent();
		}
	}
}

void UGBWPSAsync_ListenEvent::Tick(float DeltaTime)
{
	if ( LastFrameNumberWeTicked == GFrameCounter )
		return;

	if(bIsCompleted)
	{
		this->ConditionalBeginDestroy();
		return;
	}
	
	Timer = Timer + DeltaTime;
	FrameDeltaTime = DeltaTime;
	Update.Broadcast(this,Timer,FrameDeltaTime,FPE_Event(),FPS_Event());
	
	LastFrameNumberWeTicked = GFrameCounter;
}

ETickableTickType UGBWPSAsync_ListenEvent::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

TStatId UGBWPSAsync_ListenEvent::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
}

bool UGBWPSAsync_ListenEvent::IsTickableWhenPaused() const
{
	return true;
}

bool UGBWPSAsync_ListenEvent::IsTickableInEditor() const
{
	return false;
}
