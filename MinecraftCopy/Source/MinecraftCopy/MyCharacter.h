// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MINECRAFTCOPY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	

	// 이동 함수
	void Move(const FInputActionValue& Value);

	// 시야 함수
	void Look(const FInputActionValue& Value);

private:
	// 카메라 붐 (Spring Arm)
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	// 따라다니는 카메라
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	// 기본 입력 매핑 컨텍스트
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	// 이동 입력 액션
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	// 시야 입력 액션
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
};
