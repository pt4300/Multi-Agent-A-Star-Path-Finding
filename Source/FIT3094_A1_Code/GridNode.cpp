// Fill out your copyright notice in the Description page of Project Settings.


#include "GridNode.h"

GridNode::GridNode()
{
	X = 0;
	Y = 0;

	GridType = Open;
	Parent = nullptr;
	ObjectAtLocation = nullptr;

	G = 0;
	H = 0;
	F = 0;
	
}

float GridNode::GetTravelCost() const
{
	switch(GridType)
	{
		case Open:
			return 1;
		case Wall:
			return 999999;
		case Forest:
			return 7;
		case Swamp:
			return 10;
		case Water:
			return 15;
		default:
			return 1;
	}
}
