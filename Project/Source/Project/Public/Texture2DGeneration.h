// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Texture2DGeneration.generated.h"

class UProceduralMeshComponent;



UCLASS()
class PROJECT_API ATexture2DGeneration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATexture2DGeneration();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Clear")
	void ClearLastGeneration();
	UFUNCTION(BlueprintCallable, Category = "ResetParameters")
	void ResetParameters();
	UFUNCTION(BlueprintCallable, Category = "Regenerate")
	void Regenerate();
	UFUNCTION(BlueprintCallable, Category = "Generate")
	void Generate();

	
	
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Texture Creation")
	UTexture2D* pTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object to render")
	UStaticMeshComponent* Plane;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* MaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterialInterface* MaterialInterface;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Polygons")
	UProceduralMeshComponent* ProceduralMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Polygons")
	UMaterialInterface* CityMaterialInterface;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float MaxHeight = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	FVector2D CenterPos = FVector2D(100,100);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float SigmaX = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float SigmaY = 50.0f;
	//voronoi seed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiSeed")
	float AOffset = 9.45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiSeed")
	float BOffset= 10.01;

	// cell count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiScale")
	int CellCount = 16;

	//road width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoadWidth")
	float RoadWidth = 3.0f;

	//pavement width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PavementWidth")
	float PavementWidth = RoadWidth/2;

	//resolution
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureResolution")
	int32 TextureResolutionInX = 256;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureResolution")
	int32 TextureResolutionInY = 256;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PerlinNoiseSeed")
	int32 PerlinNoiseSeed = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infrasturcture")
	UInstancedStaticMeshComponent* TreeStaticMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Infrasturcture")
	UInstancedStaticMeshComponent* StreetLightStaticMesh;
	



};


