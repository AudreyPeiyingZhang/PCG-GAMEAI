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
	

};
