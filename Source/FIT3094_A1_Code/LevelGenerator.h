// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Agent.h"
#include "CoreMinimal.h"
#include "Food.h"
#include "Vegetation.h"
#include "Meat.h"
#include "herbivore.h"
#include "carnivore.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"

#include "LevelGenerator.generated.h"


UCLASS()
class FIT3094_A1_CODE_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

	// Maximum Size for World Map
	static const int MAX_MAP_SIZE = 256;

public:

	// Grid Size in World Units
	static const int GRID_SIZE_WORLD = 100;
	static const int NUM_FOOD = 60;
	static const int NUM_AGENTS = 4;
	TArray<GridNode*> FoodPathGenerate(GridNode* starterNode, GridNode* destination);
	GridNode* findNearestFood(AAgent* inputAgent);
	

	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(BlueprintReadOnly)
		int MapSizeX;
	UPROPERTY(BlueprintReadOnly)
		int MapSizeY;
	
	// This is a 2D Array for holding all the elements of the world loaded from file
	char CharMapArray[MAX_MAP_SIZE][MAX_MAP_SIZE];

	// This is a 2D Array for holding nodes for each part of the world
	GridNode* WorldArray[MAX_MAP_SIZE][MAX_MAP_SIZE];

	UPROPERTY()
		TArray<AFood*> FoodActors;

	UPROPERTY()
		TArray<AAgent*> AgentActors;

	// Actors for spawning into the world
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> WallBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> OpenBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> TreeBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> SwampBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> WaterBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> FoodBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> AgentBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AFood> VegetationBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AFood> MeatBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> herbivoreBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> carnivoreBlueprint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnWorldActors();

	void GenerateNodeGrid();
	void ResetAllNodes();
	



	

	float CalculateDistanceBetween(GridNode* first, GridNode* second);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void GenerateWorldFromFile(TArray<FString> WorldArray);

};
