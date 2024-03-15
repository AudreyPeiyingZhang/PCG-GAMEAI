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
	constexpr float CellScale = 150.0f;
	//create texture2d
	pTexture = UMyBlueprintFunctionLibrary::CreateTexture2D(512,512 );
	//set texture to material
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	MaterialInstance->SetTextureParameterValue(FName("BaseTexture"), pTexture);
	//set material to object
	Plane->SetMaterial(0, MaterialInstance);
	//clear texture2d
	UMyBlueprintFunctionLibrary::ClearTexture2D(pTexture, FColor::White);
	//draw any (x,y) on texture2d pixel
	//UMyBlueprintFunctionLibrary::SetTexture2DPixels(Texture2D, 20, 20, FColor::Red);
	//create random FVector2D by FVector2D seed, return FVector2D on the range FVector2D.X[0,1], FVector2D.Y[0,1]
	//const FVector2D Seed = FVector2D(13.56, 87.31);
	//UMyBlueprintFunctionLibrary::RandomVector2DtoVector2D(Seed);

	//drawVoronoi
	UMyBlueprintFunctionLibrary::DrawVoronoiOnTexture2D(pTexture, CellScale);
	
	
	
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}


