// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MultiplayerTPS/Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Set Actor to Replicate
	bReplicates = true;

	// Create WeaponMesh And Set it as Root Component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// Set Collision Resposes
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create Collision Area for Weapon
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

	// By Default Disable Collisions Response (Will be setted from server)
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create OverHeadWidget
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (PickUpWidget)
	{
		//Hide Widget By Default
		PickUpWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		// Set AreaSphere Collisions
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		
		// Bind OnBeginOverlap
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnSphereOverlap);

		// Bbind EndOverlap
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnSphereEndOverlap);
	}
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Replication
void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
}
void ABaseWeapon::SetWeaponState(EWeaponState weaponState)
{
	WeaponState = weaponState;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		ShowPickupWidget(true);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	break;
	case EWeaponState::EWS_Dropped:
		ShowPickupWidget(true);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

// Sphere Overlap Event Listener
void ABaseWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Cast the Other Actor to our Character
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);

	// If BaseCharacter is Valid Set Overlapping Weapon
	if (BaseCharacter)
	{
		BaseCharacter->SetOverlappingWeapon(this);
	}
}

void ABaseWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Cast the Other Actor to our Character
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);

	// If BaseCharacter is Valid Set Overlapping Weapon to nullptr
	if (BaseCharacter)
	{
		BaseCharacter->SetOverlappingWeapon(nullptr);
	}
}
// Toggles PickUp Messaage visibility
void ABaseWeapon::ShowPickupWidget(bool bShow) 
{
	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShow);
	}
}
