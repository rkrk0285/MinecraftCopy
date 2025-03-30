// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterControlData.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Spring Arm (카메라 붐) 생성 및 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 캐릭터와 카메라 거리 설정
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 기준으로 회전

	// 카메라 생성 및 Spring Arm에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 자체 회전하지 못하도록

	// 매핑 컨텍스트, 액션 찾아서 할당. 레퍼런스 주소 활용
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/InputSystem/IMC_Default.IMC_Default'"));
	if (MappingContext.Object)
	{
		DefaultMappingContext = MappingContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> QuaterContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/InputSystem/IMC_Quater.IMC_Quater'"));
	if (QuaterContext.Object)
	{
		ShoulderMappingContext = QuaterContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MOVEACTION(TEXT("/Script/EnhancedInput.InputAction'/Game/InputSystem/IA_Move.IA_Move'"));
	if (MOVEACTION.Object)
	{
		MoveAction = MOVEACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LOOKACTION(TEXT("/Script/EnhancedInput.InputAction'/Game/InputSystem/IA_Look.IA_Look'"));
	if (LOOKACTION.Object)
	{
		LookAction = LOOKACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JUMPACTION(TEXT("/Script/EnhancedInput.InputAction'/Game/InputSystem/IA_Jump.IA_Jump'"));
	if (JUMPACTION.Object)
	{
		JumpAction = JUMPACTION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SHOULDERMOVE(TEXT("/Script/EnhancedInput.InputAction'/Game/InputSystem/IA_ShoulderMove.IA_ShoulderMove'"));
	if (SHOULDERMOVE.Object)
	{
		ShoulderMoveAction = SHOULDERMOVE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CHARACTERCHANGE(TEXT("/Script/EnhancedInput.InputAction'/Game/InputSystem/IA_ChangeCharacter.IA_ChangeCharacter'"));
	if (CHARACTERCHANGE.Object)
	{
		CharacterChangeAction = CHARACTERCHANGE.Object;
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController()); // 플레이어 컨트롤러 가져오기

	// 특정 플레이어의 Enhanced Input 서브 시스템을 가져와서
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		// 가져온 서브 시스템에 매핑 컨텍스트를 할당하는 역할.
		Subsystem->AddMappingContext(DefaultMappingContext, 0);		
		// 매핑 컨텍스트를 삭제할 때.
		//Subsystem->RemoveMappingContext(DefaultMappingContext);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 캐스팅 전 체크, 없는 경우 완벽한 Null 을 보장
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{		
		//Action에 따른 함수 바인드
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Shoulder
		EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::ShoulderMove);

		// Change Character
		EnhancedInputComponent->BindAction(CharacterChangeAction, ETriggerEvent::Started, this, &AMyCharacter::ChangeCharacter);
	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveValue = Value.Get<FVector2D>();
	if (Controller)
	{
		// 현재 컨트롤러의 회전값을 가져온뒤.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 회전 값을 기준으로 X축, Y축 벡터를 가져온다.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 이를 상하좌우 이동 값에 대입.
		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);   // 좌우 회전
	AddControllerPitchInput(LookValue.Y); // 상하 회전
}

void AMyCharacter::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	
	if (MovementVectorSizeSquared > 1.0f) 
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else 
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AMyCharacter::ChangeCharacter(const FInputActionValue& Value)
{	
	if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Default);
		UE_LOG(LogTemp, Log, TEXT("Default"));
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Default)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
		UE_LOG(LogTemp, Log, TEXT("Quater"));
	}
}

void AMyCharacter::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);			
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AMyCharacter::SetCharacterControlData(const UCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeLocation(FVector(0, 0, CharacterControlData->CameraBoomZ));
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}
