// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERTPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	friend class ABaseCharacter;
	// Sets default values for this component's properties
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(ABaseWeapon* WeaponToEquip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	ABaseCharacter* Character;
	ABaseWeapon* EquippedWeapon;

		
};
