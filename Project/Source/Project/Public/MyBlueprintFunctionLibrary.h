// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Make Texture")
	static UTexture2D* CreateTexture2D(int32 Width, int32 Height);
	UFUNCTION(BlueprintCallable, Category = "Make Texture")
	static void ClearTexture2D(UTexture2D* Texture2D, FColor Colour);
	UFUNCTION(BlueprintCallable, Category = "Make Texture")
	static void SetTexture2DPixels(UTexture2D* Texture2D, int32 X, int32 Y, FColor Colour);
	UFUNCTION(BlueprintCallable, Category = "Make Noise")
	static float RandomVector2DtoVector1D(FVector2D Vector2D, FVector2D a, float b, float c);
	UFUNCTION(BlueprintCallable, Category = "Make Noise")
	static FVector2D RandomVector2DtoVector2D (FVector2D Vector2D);
	UFUNCTION(BlueprintCallable, Category = "Make Noise")
	static float RandomVector1DtoVector1D (float RandomFloatNumber, float a,float b);
	UFUNCTION(BlueprintCallable, Category = "Make Noise")
	static FVector RandomVector1DtoVector3D (float RandomFloatNumber);
	//UFUNCTION(BlueprintCallable, Category = "Voronoi Calculation")
	//static void VoronoiSeedsCalculation(UTexture2D* Texture2D,float CellCount);
	UFUNCTION(BlueprintCallable, Category = "Voronoi Calculation")
	static void DrawVoronoiSeedsOnTexture2D(UTexture2D* Texture2D, FColor color);
	UFUNCTION(BlueprintCallable, Category = "Voronoi Calculation")
	static FColor VoronoiCalculation(FVector2D PixelLocation, float CellCount);
	UFUNCTION(BlueprintCallable, Category = "Voronoi Calculation")
	static void DrawVoronoiOnTexture2D (UTexture2D* Texture2D, float CellCount);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void CalculateVertices(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void DrawVerticesOnTexture2D(UTexture2D* Texture2D,FColor color);
	

	
public:	
	static TArray<FVector2D> VoronoiSeeds;
	static TArray<FVector2D> Vertices;
	static TArray<TArray<float>> DistField;
	static void InitializeDistField(UTexture2D* Texture2D);
	static TArray<TArray<float>> GradientField;
	static void InitializeGradientField(UTexture2D* Texture2D);
		

	//function to make texture 2d

	//function to set pixel of texturer 2d// inefficient/slow

	//function to set pixels without relocking texture (your understand when you do the above)

	//function draw veronoi edges base on input noise texture

	//function to make perlin noise using rand

	//perhPS more random functions with turbulance etc.... look at shader toy

	//far down the lihe, make geometry from texture somehow?

	
};
