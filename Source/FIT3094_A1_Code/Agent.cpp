// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"


#include "EngineUtils.h"
#include "LevelGenerator.h"

// Sets default values

AAgent::AAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100; // change to 100 since there are less food due to two agent type...
	MoveSpeed = 100;
	Tolerance = 20;
}

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<ALevelGenerator>ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		levelReference = Cast<ALevelGenerator>(*ActorItr);
		if (levelReference)
		{
			break;
		}
	}
	// Set a timer for every two seconds and call the decrease health function
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAgent::DecreaseHealth, 2.0f, true, 2.0f);
}

void AAgent::DecreaseHealth()
{
	// Decrease health by one and if 0 then destroy object
	Health--;

	if(Health <= 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		Destroy();
	}
}

// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if(Path.Num()>Health)
	//{
	//	// this is used to prevent agent died before they reach to nearest food, if the path required longer distance than current health, then recalculate the root to prevent death.
	//	GridNode* dest = levelReference->findNearestFood(Cast<AAgent>(currentNode->ObjectAtLocation));


	//	Path = levelReference->FoodPathGenerate(currentNode, dest);
	//}

	if(Path.Num() > 0)
	{

		FVector CurrentPosition = GetActorLocation();

		float TargetXPos = Path[0]->X * ALevelGenerator::GRID_SIZE_WORLD;
		float TargetYPos = Path[0]->Y * ALevelGenerator::GRID_SIZE_WORLD;
			
		FVector TargetPosition(TargetXPos, TargetYPos, CurrentPosition.Z);

		FVector Direction = TargetPosition - CurrentPosition;
		Direction.Normalize();

		CurrentPosition += Direction * MoveSpeed * DeltaTime;
		SetActorLocation(CurrentPosition);
	
		//if(levelReference->conflictsNode.Contains(currentNode))
		//{
		//	//if the conflict nodes has bee
		//	levelReference->conflictsNode.Remove(currentNode);
		//}
		if(FVector::Dist(CurrentPosition, TargetPosition) <= Tolerance)
		{
			CurrentPosition = TargetPosition;
			DecreaseHealth();
			if(Path[0]->ObjectAtLocation && Path[0]->ObjectAtLocation->IsA<AFood>())
			{
				//Remove food from foodactors and destroy it upon agent eaten
				AFood* foodToDestroy = Cast<AFood>(Path[0]->ObjectAtLocation);
				if (levelReference->FoodActors.Contains(foodToDestroy))
				{	//delete used foods in foodactors, usually clean by Foodpathgenerate, but rarely happen due to tolerance value 
					levelReference->FoodActors.Remove(foodToDestroy);
				}
				Path[0]->ObjectAtLocation = nullptr;

				Health=100;
				foodToDestroy->Destroy();
				
			}
			else
			{
				Path.RemoveAt(0);
			}
		}


	}
	else
	{// regenerate new path after agent find foods
		
		GridNode* dest = levelReference->findNearestFood(Cast<AAgent>(currentNode->ObjectAtLocation));
		

		Path = levelReference->FoodPathGenerate(currentNode, dest);
	}

	
}

