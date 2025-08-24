// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "BaseItem.generated.h"

class USphereComponent;

UCLASS()
class PROJECT_BALINGA_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereColli;

	void ItemPickUp(USceneComponent* AttachTo);
	void ItemDrop();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
