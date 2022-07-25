// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Food.h"

#include "Vegetation.generated.h"

/**
 * 
 */
UCLASS()
class FIT3094_A1_CODE_API AVegetation : public AFood
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVegetation();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
