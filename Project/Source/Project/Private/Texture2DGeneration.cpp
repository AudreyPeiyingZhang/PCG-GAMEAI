// Fill out your copyright notice in the Description page of Project Settings.


#include "Texture2DGeneration.h"

#include "Kismet/KismetRenderingLibrary.h"


// Sets default values
ATexture2DGeneration::ATexture2DGeneration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UStaticMeshComponent>("Plane");
	MaterialInterface = CreateDefaultSubobject<UMaterialInterface>("MaterialInterface");
	
	
	
	
}

// Called when the game starts or when spawned
void ATexture2DGeneration::BeginPlay()
{
	Super::BeginPlay();
	HeightfieldTextureRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024);
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	MaterialInstance->SetTextureParameterValue(FName("TextureRenderTarget"), HeightfieldTextureRenderTarget);
	Plane->SetMaterial(0, MaterialInstance);
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATexture2DGeneration::SetUpRealTimeDynamicTexture(int32 Width, int32 Height)
{
	
}

void ATexture2DGeneration::OnUpdateTextureRenderTarget2D()

{
	for(int X = 0; X < 1024; X++)
	{
		for(int Y = 0; Y<1024; Y++)
		{
			//HeightfieldTextureRenderTarget->
		}
	}
}
