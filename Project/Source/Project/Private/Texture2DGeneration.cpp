// Fill out your copyright notice in the Description page of Project Settings.


#include "Texture2DGeneration.h"



// Sets default values
ATexture2DGeneration::ATexture2DGeneration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATexture2DGeneration::BeginPlay()
{
	Super::BeginPlay();
	CreateRealTimeDynamicTexture();
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATexture2DGeneration::CreateRealTimeDynamicTexture(int32 Width, int32 Height)
{
	DynamicTexture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),Width, Height);
	DynamicTexture->ClearColor = FLinearColor::White;
	DynamicTexture->OnCanvasRenderTargetUpdate.AddDynamic(this, &ATexture2DGeneration::OnUpdateCanvasRenderTarget2D);
	DynamicTexture->UpdateResource();
	
	
}

void ATexture2DGeneration::OnUpdateCanvasRenderTarget2D(UCanvas* Canvas,  int32 Width, int32 Height)
{
	if(Canvas)
	{
		const FColor PixelColor = FColor::Green;
		Canvas->SetDrawColor(PixelColor);
		for(int X = 0; X < Width; X++)
		{
			for(int Y = 0; Y < Height; Y++)
			{
				Canvas->K2_DrawBox(FVector2D(X, Y), FVector2D(1, 1), 1.0f);
			}
		}
		
		
	}
}
