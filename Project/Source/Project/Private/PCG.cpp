// Fill out your copyright notice in the Description page of Project Settings.


#include "PCG.h"

#include "MyBlueprintFunctionLibrary.h"

// Sets default values
APCG::APCG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	RootComponent = ProceduralMesh;
	

}

// Called when the game starts or when spawned
void APCG::BeginPlay()
{
	Super::BeginPlay();
	CreateMesh();
	
	
}

// Called every frame
void APCG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APCG::CreateMesh()
{
	
	
	
	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(0 ,100, 0));
	Vertices.Add(FVector(100, 0, 0));
	Vertices.Add(FVector(100, 100, 0));
	
	//triangle1
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	//triangle2
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);

	//uvs
	UVs.Add(FVector2d(0,0));
	UVs.Add(FVector2d(0,1));
	UVs.Add(FVector2d(1,0));
	UVs.Add(FVector2d(1,1));
	
	ProceduralMesh->CreateMeshSection(0,Vertices,Triangles,TArray<FVector>(),UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	if(MaterialInstance)
	{
		ProceduralMesh->SetMaterial(0, MaterialInstance);
	}
}
