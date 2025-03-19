// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

AMyGameModeBase::AMyGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> CharacterClassRef(TEXT("/Script/CoreUObject.Class'/Script/MinecraftCopy.MyCharacter'"));
	if (CharacterClassRef.Class)
	{
		DefaultPawnClass = CharacterClassRef.Class;
	}
}
