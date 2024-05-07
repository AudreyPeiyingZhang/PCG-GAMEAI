// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	float SigmaX = 45.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	float SigmaY = 45.0f;
	//voronoi seed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiSeed")
	float AOffset = 9.45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiSeed")
	float BOffset= 10.01;

	// cell count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoronoiScale")
	int CellCount = 20;

	//road width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoadWidth")
	float RoadWidth = 1.0f;

	//resolution
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureResolution")
	int32 TextureResolutionInX = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextureResolution")
	int32 TextureResolutionInY = 200;
};
