// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereColli = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
} 

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
}
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseItem::ItemPickUp(USceneComponent* AttachTo)
{
	SphereColli->SetSimulatePhysics(false);
	GEngine->AddOnScreenDebugMessage(4, 2, FColor::Cyan, FString("item pick up"));
	AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ABaseItem::ItemDrop()
{
	SphereColli->SetSimulatePhysics(true);
	GEngine->AddOnScreenDebugMessage(4, 2, FColor::Cyan, FString("item drop"));
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

