// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BalingaBase.generated.h"

class CameraController;
class ABaseItem;
class SphereComponent;
class UBalingaStatemachine;

UCLASS()
class ABalingaBase : public ACharacter
{
	typedef ACharacter Super;

public:
	//ABalingaBase();
	ABalingaBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Balinga Movement")
	class UBalingaMovement* BalingaMovement;
	UPROPERTY()
	TObjectPtr<USceneComponent> LeftLiftArrow;
	UPROPERTY()
	TObjectPtr<USceneComponent> RightLiftArrow;
	UPROPERTY()
	TObjectPtr<USceneComponent> DragArrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	class ABalingaControllerBase* ControllerBase;


	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balinga Movement|Other|Attack")
	class USphereComponent* AttackSphere;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Balinga State Machine")
	UBalingaStatemachine* StateMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UBalingaCamera* BalingaCamera;

	UPROPERTY(EditAnywhere, Category = "Help")
	float bDoesThisWork = true;

	UFUNCTION()
	void OnAttackOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void CheckJumpInput(float DeltaTime) override;
	void PickUpItem(ABaseItem* Item);

	//to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	int camMode = 0;

	UPROPERTY()
	ABaseItem* HeldItem = nullptr;

#pragma endregion

private:
	GENERATED_BODY()
};
