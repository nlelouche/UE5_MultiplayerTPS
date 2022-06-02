// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerTPS/Weapons/BaseWeapon.h"
#include "MultiplayerTPS/Components/CombatComponent.h"

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

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Overhead Widget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Combat->SetIsReplicated(true);
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

// Replicated Props
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABaseCharacter::SetOverlappingWeapon(ABaseWeapon* weapon)
{
	// If we have a previous weapon, hide its widgets
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = weapon;

	// If the weapon is valid, and we are locally controlled (Server wont hear the OnRep_Notify)
	// Show PickUp Widget
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}

}

void ABaseCharacter::OnRep_OverlappingWeapon(ABaseWeapon* lastWeapon)
{
	if (lastWeapon)
	{
		lastWeapon->ShowPickupWidget(false);
	}

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABaseCharacter::EquipButtonPressed);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABaseCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();			
		}
	}
}

void ABaseCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
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
