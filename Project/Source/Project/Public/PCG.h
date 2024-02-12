// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PCG.generated.h"

UCLASS()
class PROJECT_API APCG : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APCG();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void CreateMesh();

private:
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;
	
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2d> UVs;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* MaterialInstance;
	
	

};
