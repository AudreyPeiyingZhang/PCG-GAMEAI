// Fill out your copyright notice in the Description page of Project Settings.


#include "Texture2DGeneration.h"

#include "MyBlueprintFunctionLibrary.h"
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
	
	Texture2D = UMyBlueprintFunctionLibrary::CreateTexture2D(24,24 );
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	MaterialInstance->SetTextureParameterValue(FName("BaseTexture"), Texture2D);
	Plane->SetMaterial(0, MaterialInstance);
	UMyBlueprintFunctionLibrary::ClearTexture2D(Texture2D, FColor::White);
	UMyBlueprintFunctionLibrary::SetTexture2DPixels(Texture2D, 20, 20, FColor::Red);
	
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}


