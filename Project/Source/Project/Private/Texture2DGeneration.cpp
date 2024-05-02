// Fill out your copyright notice in the Description page of Project Settings.


#include "Texture2DGeneration.h"

#include "MyBlueprintFunctionLibrary.h"



// Sets default values
ATexture2DGeneration::ATexture2DGeneration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UStaticMeshComponent>("Plane");
	MaterialInterface = CreateDefaultSubobject<UMaterialInterface>("MaterialInterface");
	//poly
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	CityMaterialInterface = CreateDefaultSubobject<UMaterialInterface>("CityMaterialInterface");

	SetRootComponent(ProceduralMesh);

	
	
	
	
}

// Called when the game starts or when spawned
void ATexture2DGeneration::BeginPlay()
{
	Super::BeginPlay();

	Plane->SetWorldLocation(FVector(100.0f,100.0f,-10.0f));
	

	
	
	//create texture2d
	pTexture = UMyBlueprintFunctionLibrary::CreateTexture2D(200,200);
	//set texture to material
	MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	MaterialInstance->SetTextureParameterValue(FName("BaseTexture"), pTexture);
	//set material to object
	Plane->SetMaterial(0, MaterialInstance);
	//clear texture2d
	UMyBlueprintFunctionLibrary::ClearTexture2D(pTexture, FColor::White);
	//draw any (x,y) on texture2d pixel
	//UMyBlueprintFunctionLibrary::SetTexture2DPixels(Texture2D, 20, 20, FColor::Red);
	//
	UMyBlueprintFunctionLibrary::SetVoronoiSeed(VectorSeedA, AOffset, AAmplitude, VectorSeedB, BOffset, BAmplitude);
	UMyBlueprintFunctionLibrary::SetCellCount(CellCount);
	
	UMyBlueprintFunctionLibrary::InitializeClosestCellVoronoiSeedXY(pTexture);
	UMyBlueprintFunctionLibrary::VoronoiCalculation(pTexture);
	UMyBlueprintFunctionLibrary::DrawVoronoiSeedsOnTexture2D(pTexture, FColor::Black);
	UMyBlueprintFunctionLibrary::CalculateVertices(pTexture);
	UMyBlueprintFunctionLibrary::Add4VerticesOnWholeTextureCorner(pTexture);
	UMyBlueprintFunctionLibrary::DrawVerticesOnTexture2D(pTexture,FColor::Cyan);
	UMyBlueprintFunctionLibrary::AssignCellNumbers(pTexture);
	UMyBlueprintFunctionLibrary::MergeSameCornerVertices();
	UMyBlueprintFunctionLibrary::Merge4CellCountVertices();
	UMyBlueprintFunctionLibrary::DrawMergedVerticesOnTexture2D(pTexture, FColor::Green);
	UMyBlueprintFunctionLibrary::PrintVertexPosAndUniqueCellNumber();
	UMyBlueprintFunctionLibrary::GroupVerticesWithSharedCells(pTexture);
	UMyBlueprintFunctionLibrary::PrintPairedVertices();
	UMyBlueprintFunctionLibrary::DrawDebugEdges(GetWorld());
	//functions below is to create polygons
	UMyBlueprintFunctionLibrary::ProcessVerticesForPolyCalculation(pTexture);
	UMyBlueprintFunctionLibrary::AssignVertexUniqueIndex();
	UMyBlueprintFunctionLibrary::DistinguishEachCell(pTexture);
	UMyBlueprintFunctionLibrary::GroupEdgesInCells();
	UMyBlueprintFunctionLibrary::AssignEachCellStruct();
	UMyBlueprintFunctionLibrary::SortVerticesInCells();
	UMyBlueprintFunctionLibrary::PrintCellsArray();
	//normal distribution
	UMyBlueprintFunctionLibrary::SetCityCenterHeightSigma(MaxHeight, CenterPos, SigmaX,
	SigmaY);
	UMyBlueprintFunctionLibrary::CreateVoronoiShapePolygon(ProceduralMesh, CityMaterialInterface);
	
	
}

// Called every frame
void ATexture2DGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

