// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/SphereComponent.h"
#include "BaseItem.h"


#include "BalingaBase.generated.h"

class CameraController_Balinga;

UCLASS()
class ABalingaBase : public ACharacter
{
	typedef ACharacter Super;

public:
	//ABalingaBase();
	ABalingaBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) 
	class UBalingaMovement* BalingaMovement;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Move Balinga|other|Attack")
	class USphereComponent* AttackSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UBalingaCamera* CameraController;

	//Ground parameters
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump|GravityScale")
	float BaseGravityScale = 9.8f;
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump|GravityScale")
	float JumpGravityScale = 3.9f;
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|Jump")
	float JumpVelocity = 1200;
	UPROPERTY(EditAnywhere, Category = "Move Balinga|Ground|")
	float MoveSpeed = 750;

	FTimerHandle AttackCooldownTimer;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void CheckJumpInput(float DeltaTime) override;

	//to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartJump();
	void EndJump();

	void Land();

	void CamChange();
	int camMode = 0;

	#pragma region AttackFunctions

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown = 1.0f;

	bool bCanAttack = true;

	void TryAttack();
	void AttackCD();	//CD = Cooldown


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

	UPROPERTY()
	ABaseItem* HeldItem = nullptr;

	void DropItem();
	void PickUpItem(ABaseItem* Item);

#pragma endregion

private:
	GENERATED_BODY()
};
