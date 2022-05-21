// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);
}



void ABaseCharacter::MoveForward(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		// Get Yaw Rotation From the Character Controller
		const FRotator YawRotation(0.f,Controller->GetControlRotation().Yaw, 0.f);

		// Get a Direction From a RotationMatrix to get Vector.Forward of ccharacter controller
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		// Add Movement to Player
		AddMovementInput(Direction, value);
	}

}
			 
void ABaseCharacter::MoveRight(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		// Get Yaw Rotation From the Character Controller
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

		// Get a Direction From a RotationMatrix to get Vector.Right of ccharacter controller
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// Add Movement to Player
		AddMovementInput(Direction, value);
	}
}
			 
void ABaseCharacter::Turn(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		AddControllerYawInput(value);
	}
}
			 
void ABaseCharacter::LookUp(float value)
{
	if (Controller != nullptr && value != 0.f)
	{
		AddControllerPitchInput(value);
	}
}
