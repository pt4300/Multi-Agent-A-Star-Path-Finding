// Fill out your copyright notice in the Description page of Project Settings.


#include "Vegetation.h"

// Sets default values
AVegetation::AVegetation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVegetation::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVegetation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
