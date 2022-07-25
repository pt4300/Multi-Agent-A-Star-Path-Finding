// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGenerator.h"
#include "Containers/Queue.h"
#include "stack"
#include "Agent.h"
#include "Engine/World.h"
#include "UObject/UObjectIterator.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Should spawn more food if there are not the right number
	
	//Both veggie and meat will generated if the number of foods drop below half
	if (FoodActors.Num() < NUM_FOOD/2)
	{
			//generate both veggie and meat together to avoid synchronize issue, if in separate if for loop sometimes only one type food will be generated
			for (int i = 0; i < (NUM_FOOD/ 2); i++)
			{
				int RandXPos = 0;
				int RandYPos = 0;
				int RandXPos2 = 0;
				int RandYPos2= 0;
				bool isFree = false;

				while (!isFree) {
					RandXPos = FMath::RandRange(0, MapSizeX - 1);
					RandYPos = FMath::RandRange(0, MapSizeY - 1);
					RandXPos2 = FMath::RandRange(0, MapSizeX - 1);
					RandYPos2= FMath::RandRange(0, MapSizeY - 1);

					if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::Open && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr&&WorldArray[RandXPos2][RandYPos2]->GridType == GridNode::Open && WorldArray[RandXPos2][RandYPos2]->ObjectAtLocation == nullptr)
					{
						isFree = true;
					}
				}

				FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
				FVector Position2(RandXPos2 * GRID_SIZE_WORLD, RandYPos2 * GRID_SIZE_WORLD, 20);
				AVegetation* NewFood = GetWorld()->SpawnActor<AVegetation>(VegetationBlueprint, Position, FRotator::ZeroRotator);
				AMeat* NewFood2 = GetWorld()->SpawnActor<AMeat>(MeatBlueprint, Position2, FRotator::ZeroRotator);
				WorldArray[RandXPos][RandYPos]->ObjectAtLocation = NewFood;
				WorldArray[RandXPos2][RandYPos2]->ObjectAtLocation = NewFood2;
				NewFood->currentNode = WorldArray[RandXPos][RandYPos];
				NewFood2->currentNode = WorldArray[RandXPos2][RandYPos2];
				FoodActors.Add(NewFood);
				FoodActors.Add(NewFood2);

			}



		
	}

	

}

void ALevelGenerator::GenerateWorldFromFile(TArray<FString> WorldArrayStrings)
{
	// If empty array exit immediately something is horribly wrong
	if(WorldArrayStrings.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("World Array is empty!"));
		return;
	}

	// Second line is Height (aka X value)
	FString Height = WorldArrayStrings[1];
	Height.RemoveFromStart("height ");
	MapSizeX = FCString::Atoi(*Height);
	UE_LOG(LogTemp, Warning, TEXT("Height: %d"), MapSizeX);

	// Third line is Width (aka Y value)
	FString Width = WorldArrayStrings[2];
	Width.RemoveFromStart("width ");
	MapSizeY = FCString::Atoi(*Width);
	UE_LOG(LogTemp, Warning, TEXT("Width: %d"), MapSizeY);
	
	// After removing top 4 lines this is the map itself so iterate each line
	for (int LineNum = 4; LineNum < WorldArrayStrings.Num(); LineNum++)
	{
		for (int CharNum = 0; CharNum < WorldArrayStrings[LineNum].Len(); CharNum++)
		{
			CharMapArray[LineNum-4][CharNum] = WorldArrayStrings[LineNum][CharNum];
		}
	}

	GenerateNodeGrid();
	SpawnWorldActors();

	for (int i = 0; i < AgentActors.Num(); i++)
	{
		GridNode* dest = findNearestFood(AgentActors[i]);
		TArray<GridNode*> temps = FoodPathGenerate(AgentActors[i]->currentNode, dest);
		AgentActors[i]->Path = temps;

	}

	

	
	
}

void ALevelGenerator::SpawnWorldActors()
{
	UWorld* World = GetWorld();

	// Make sure that all blueprints are connected. If not then fail
	if(WallBlueprint && OpenBlueprint && WaterBlueprint && SwampBlueprint && TreeBlueprint)
	{
		// For each grid space spawn an actor of the correct type in the game world
		for(int x = 0; x < MapSizeX; x++)
		{
			for (int y = 0; y < MapSizeY; y++)
			{
				float XPos = x * GRID_SIZE_WORLD;
				float YPos = y * GRID_SIZE_WORLD;

				FVector Position(XPos, YPos, 0);

				switch (CharMapArray[x][y])
				{
					case '.':
					case 'G':
						World->SpawnActor(OpenBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case '@':
					case 'O':
						World->SpawnActor(WallBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case 'T':
						World->SpawnActor(TreeBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case 'S':
						World->SpawnActor(SwampBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case 'W':
						World->SpawnActor(WaterBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					default:
						break;
				}
			}
		}
	}

	// Generate Initial Agent Positions
	if(herbivoreBlueprint)
	{
		for (int i = 0; i < NUM_AGENTS/2; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::Open && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			Aherbivore* Agent = World->SpawnActor<Aherbivore>(herbivoreBlueprint, Position, FRotator::ZeroRotator);


			WorldArray[RandXPos][RandYPos]->ObjectAtLocation = Agent;
			Agent->currentNode = WorldArray[RandXPos][RandYPos];
			AgentActors.Add(Agent);
		}
	}

	if (carnivoreBlueprint)
	{
		for (int i = 0; i < NUM_AGENTS / 2+1; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::Open && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			Acarnivore* Agent = World->SpawnActor<Acarnivore>(carnivoreBlueprint, Position, FRotator::ZeroRotator);


			WorldArray[RandXPos][RandYPos]->ObjectAtLocation = Agent;
			Agent->currentNode = WorldArray[RandXPos][RandYPos];
			AgentActors.Add(Agent);
		}
	}

	// Generate Initial Food Positions
	if(VegetationBlueprint)
	{
		for(int i = 0; i < NUM_FOOD/2; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::Open && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			AVegetation* NewFood = World->SpawnActor<AVegetation>(VegetationBlueprint, Position, FRotator::ZeroRotator);

			WorldArray[RandXPos][RandYPos]->ObjectAtLocation = NewFood;
			NewFood->currentNode = WorldArray[RandXPos][RandYPos];

			FoodActors.Add(NewFood);
		}
	}

	if (MeatBlueprint)
	{
		for (int i = 0; i < NUM_FOOD / 2; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::Open && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			AMeat* NewFood = World->SpawnActor<AMeat>(MeatBlueprint, Position, FRotator::ZeroRotator);

			WorldArray[RandXPos][RandYPos]->ObjectAtLocation = NewFood;
			NewFood->currentNode = WorldArray[RandXPos][RandYPos];

			FoodActors.Add(NewFood);
		}
	}
}

// Generates the grid of nodes used for pathfinding and also for placement of objects in the game world
void ALevelGenerator::GenerateNodeGrid()
{
	for(int X = 0; X < MapSizeX; X++)
	{
		for(int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y] = new GridNode();
			WorldArray[X][Y]->X = X;
			WorldArray[X][Y]->Y = Y;

			// Characters as defined from the map file
			switch(CharMapArray[X][Y])
			{
				case '.':
				case 'G':
					WorldArray[X][Y]->GridType = GridNode::Open;
					break;
				case '@':
				case 'O':
					WorldArray[X][Y]->GridType = GridNode::Wall;
					break;
				case 'T':
					WorldArray[X][Y]->GridType = GridNode::Forest;
					break;
				case 'S':
					WorldArray[X][Y]->GridType = GridNode::Swamp;
					break;
				case 'W':
					WorldArray[X][Y]->GridType = GridNode::Water;
					break;
			}
		}
	}
}

// Reset all node values (F, G, H & Parent)
void ALevelGenerator::ResetAllNodes()
{
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y]->F = 0;
			WorldArray[X][Y]->G = 0;
			WorldArray[X][Y]->H = 0;
			WorldArray[X][Y]->Parent = nullptr;
		}
	}
}





TArray<GridNode*> ALevelGenerator::FoodPathGenerate(GridNode* starterNode, GridNode* destination)
{
	//This is a sligh modified version of A* alogrithm to accomplish Multi agent path finding. In each node checking, the alogrithm will check whether it contains an agent reference to avoid collision
	GridNode* currentNode = nullptr;
	GridNode* tempNode = nullptr;
	TArray<GridNode*>openList;
	TArray<GridNode*>closeList;
	std::stack<GridNode*> temp;
	TArray<GridNode*> returnRoute;
	TArray<GridNode*> conflictsNode;

	starterNode->G = 0;
	starterNode->H = CalculateDistanceBetween(starterNode, destination);
	starterNode->F = starterNode->G + starterNode->H;

	openList.Add(starterNode);
	while (openList.Num()>0)
	{
		float lowestF = 999999999999999;
		for (int i =0;i<openList.Num();i++)
		{
			if(openList[i]->F<lowestF)
			{
				lowestF = openList[i]->F;
				currentNode = openList[i];
			}
		}
		openList.Remove(currentNode);
		closeList.Add(currentNode);
		if (currentNode == destination)
		{
			// if destination node is find then use stack to reverse the path
			while (currentNode->Parent != nullptr)
			{

				temp.push(currentNode);
				currentNode = currentNode->Parent;
			}
			while (!temp.empty())
			{
				tempNode = temp.top();
				returnRoute.Add(tempNode);
				temp.pop();
			}
			return returnRoute;
		}
		//LEFT
		if(currentNode->Y-1>0)
		{
			tempNode = WorldArray[currentNode->X][currentNode->Y - 1];
			//checking whether there is agent standing on tempnode, if yes, then push it into conflictNodes and add to closelist
			if (tempNode->ObjectAtLocation &&tempNode->ObjectAtLocation->IsA<AAgent>() )
			{
				if(!conflictsNode.Contains(tempNode))
				{
					conflictsNode.Push(tempNode);
					//closeList.Add(tempNode);
				}

			}
			else if(tempNode!=destination && tempNode->ObjectAtLocation&&tempNode->ObjectAtLocation->IsA<AFood>())
			{
				//do not consume other types foods
				conflictsNode.Push(tempNode);
			}
			else if(!conflictsNode.Contains(tempNode))
			{// if node not in conlifct area then process normal heuristic function check
				
				if (!closeList.Contains(tempNode) && tempNode->GridType != GridNode::Wall)
				{
					int possibleG = currentNode->G + currentNode->GetTravelCost();
					bool isPossibleBetter = false;
					if (!openList.Contains(tempNode))
					{
						openList.Add(tempNode);
						tempNode->H = CalculateDistanceBetween(tempNode, destination);
						isPossibleBetter = true;
					}
					else if (possibleG < tempNode->G)
					{
						isPossibleBetter = true;
					}
					if (isPossibleBetter)
					{
						tempNode->Parent = currentNode;
						tempNode->G = possibleG;
						tempNode->F = tempNode->G + tempNode->H;

					}
				}
			}

		}
		if (currentNode->X + 1 < MapSizeX )
		{
			tempNode = WorldArray[currentNode->X + 1][currentNode->Y];
			if ( tempNode->ObjectAtLocation &&tempNode->ObjectAtLocation->IsA<AAgent>() )
			{
				if (!conflictsNode.Contains(tempNode))
				{ //Only push conflict nodes once, conflicts node array used to avoid generating collsion path
					conflictsNode.Push(tempNode);
					//closeList.Add(tempNode);
				}

			}
			else if (tempNode != destination && tempNode->ObjectAtLocation&&tempNode->ObjectAtLocation->IsA<AFood>())
			{
				//do not consume other types foods
				conflictsNode.Push(tempNode);
			}
			else if(!conflictsNode.Contains(tempNode))
			{
				
				if (!closeList.Contains(tempNode) && tempNode->GridType != GridNode::Wall)
				{
					int possibleG = currentNode->G + currentNode->GetTravelCost();
					bool isPossibleBetter = false;
					if (!openList.Contains(tempNode))
					{
						openList.Add(tempNode);
						tempNode->H = CalculateDistanceBetween(tempNode, destination);
						isPossibleBetter = true;
					}
					else if (possibleG < tempNode->G)
					{
						isPossibleBetter = true;
					}
					if (isPossibleBetter)
					{
						tempNode->Parent = currentNode;
						tempNode->G = possibleG;
						tempNode->F = tempNode->G + tempNode->H;

					}
				}
			}

		}
		if (currentNode->Y + 1 < MapSizeY )
		{
			tempNode = WorldArray[currentNode->X][currentNode->Y + 1];
			if (tempNode->ObjectAtLocation &&tempNode->ObjectAtLocation->IsA<AAgent>())
			{
				if (!conflictsNode.Contains(tempNode))
				{
					conflictsNode.Push(tempNode);
					//closeList.Add(tempNode);
				}

			}
			else if (tempNode != destination && tempNode->ObjectAtLocation&&tempNode->ObjectAtLocation->IsA<AFood>())
			{
				//do not consume other types foods
				conflictsNode.Push(tempNode);
			}
			else if(!conflictsNode.Contains(tempNode))
			{
				
				if (!closeList.Contains(tempNode) && tempNode->GridType != GridNode::Wall)
				{
					int possibleG = currentNode->G + currentNode->GetTravelCost();
					bool isPossibleBetter = false;
					if (!openList.Contains(tempNode))
					{
						openList.Add(tempNode);
						tempNode->H = CalculateDistanceBetween(tempNode, destination);
						isPossibleBetter = true;
					}
					else if (possibleG < tempNode->G)
					{
						isPossibleBetter = true;
					}
					if (isPossibleBetter)
					{
						tempNode->Parent = currentNode;
						tempNode->G = possibleG;
						tempNode->F = tempNode->G + tempNode->H;

					}
				}
			}

		}

		if (currentNode->X - 1 > 0  )
		{
			tempNode = WorldArray[currentNode->X - 1][currentNode->Y];
			if (tempNode->ObjectAtLocation &&tempNode->ObjectAtLocation->IsA<AAgent>())
			{
				if (!conflictsNode.Contains(tempNode))
				{
					
					conflictsNode.Push(tempNode);
					//closeList.Add(tempNode);
				}

			}
			else if (tempNode != destination && tempNode->ObjectAtLocation&&tempNode->ObjectAtLocation->IsA<AFood>())
			{
				//do not consume other types foods
				conflictsNode.Push(tempNode);
			}
			else if(!conflictsNode.Contains(tempNode))
			{
				
				if (!closeList.Contains(tempNode) && tempNode->GridType != GridNode::Wall)
				{
					int possibleG = currentNode->G + currentNode->GetTravelCost();
					bool isPossibleBetter = false;
					if (!openList.Contains(tempNode))
					{
						openList.Add(tempNode);
						tempNode->H = CalculateDistanceBetween(tempNode, destination);
						isPossibleBetter = true;
					}
					else if (possibleG < tempNode->G)
					{
						isPossibleBetter = true;
					}
					if (isPossibleBetter)
					{
						tempNode->Parent = currentNode;
						tempNode->G = possibleG;
						tempNode->F = tempNode->G + tempNode->H;

					}
				}
			}

		}


	}


	return returnRoute;
	
}
GridNode* ALevelGenerator::findNearestFood(AAgent* inputAgent)
//This function use to generate optimal food destination depends on agent type
{
	//preset shortestNode to first food to avoid null return
	GridNode* shortestNode = FoodActors[0]->currentNode;
	

	int tempShortest = 999999999;
	//This is used to track if certain 
	AFood* returnFood = nullptr;
	for(int t=0;t<FoodActors.Num();t++)
	{
		//checking input agent type
		if(inputAgent->IsA<Acarnivore>())
		{
			int tempCurrentDistance = CalculateDistanceBetween(inputAgent->currentNode, FoodActors[t]->currentNode);
			if (FoodActors[t]->IsA<AMeat>() && tempCurrentDistance < tempShortest)
			{
				if (FoodActors[t]->currentNode->ObjectAtLocation)
				{
					shortestNode = FoodActors[t]->currentNode;
					tempShortest = tempCurrentDistance;
					
				}
				else
				{// if food does not have proper refernce to node, remove it from foodactors
					FoodActors[t]->Destroy();
					FoodActors.RemoveAt(t);
					
					
				}

			}
		}
		else if(inputAgent->IsA<Aherbivore>())
		{
			int tempCurrentDistance = CalculateDistanceBetween(inputAgent->currentNode, FoodActors[t]->currentNode);
			if (FoodActors[t]->IsA<AVegetation>() && tempCurrentDistance < tempShortest)
			{
				if (FoodActors[t]->currentNode->ObjectAtLocation)
				{
					shortestNode = FoodActors[t]->currentNode;
					tempShortest = tempCurrentDistance;
					
				}
				else
				{// if food does not have proper refernce to node, remove it from foodactors
					FoodActors[t]->Destroy();
					FoodActors.RemoveAt(t);
	

				}
				
			}
		}
		
	}


		AFood* foodToRemove = Cast<AFood>(shortestNode->ObjectAtLocation);
		FoodActors.Remove(foodToRemove);

	
	return shortestNode;
}


float ALevelGenerator::CalculateDistanceBetween(GridNode* first, GridNode* second)
{
	FVector distToTarget = FVector(second->X - first->X,
		second->Y - first->Y, 0);
	return distToTarget.Size();
}