// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameFramework/Actor.h"
#include "Engine/Canvas.h"
#include "Texture2DGeneration.generated.h"

UCLASS()
class PROJECT_API ATexture2DGeneration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATexture2DGeneration();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Texture Creation" )
	void CreateRealTimeDynamicTexture(int32 Width = 1024, int32 Height = 1024);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture Creation")
	UCanvasRenderTarget2D* DynamicTexture;
	UFUNCTION(BlueprintCallable, Category = "Texture Update")
	void OnUpdateCanvasRenderTarget2D(UCanvas* Canvas,  int32 Width = 1024, int32 Height = 1024);
	

};
