// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "MyBlueprintFunctionLibrary.h"

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UMyBlueprintFunctionLibrary::DrawVertexPositionsAndCellNumbersOnHUD(this, PlayerController);
}
