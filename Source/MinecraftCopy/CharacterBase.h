// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Default,
	Shoulder
};

UCLASS()
class MINECRAFTCOPY_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	virtual void SetCharacterControlData(const class UCharacterControlData* CharacterControlData);
	
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UCharacterControlData*> CharacterControlManager;	
};
