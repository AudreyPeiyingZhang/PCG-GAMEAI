// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FVerticesEdgesStruct
{
	GENERATED_BODY()

public:

	FVector2D VertexPosition;
	TSet<int32> CurrentCellsUniqueNumbers;

	bool IsContainMoreThan3Element( const FVerticesEdgesStruct Other) const
	{

		int Count = 0;
		for (const int32 Element : CurrentCellsUniqueNumbers)
		{
			if (Other.CurrentCellsUniqueNumbers.Contains(Element))
			{
				Count++;
				
				
			}
		}

		

		if(Count>=3)
		{
			return true;
		}

		return false;
	}

	bool IsTwoEquivalent(const FVerticesEdgesStruct& Other) const
	{
		if (CurrentCellsUniqueNumbers.Num() != Other.CurrentCellsUniqueNumbers.Num())
		{
			return false; 
		}

		for (const int32 Element : CurrentCellsUniqueNumbers)
		{
			if (!Other.CurrentCellsUniqueNumbers.Contains(Element))
			{
				return false;
			}
		}

		return true;
	}

	bool IsWholeTextureEdge(int32 Width, int32 Height) const
	{
		if(VertexPosition.X == 1 || VertexPosition.X == Width - 2 || VertexPosition.Y == 1 ||VertexPosition.Y == Height - 2)
		{
			return true;
		}

		return false;
	}

	

	
};

USTRUCT(BlueprintType)
struct FPairedVertices
{
	GENERATED_BODY()
public:

	FVector2D FirstVertex;
	FVector2D SecondVertex;

	FPairedVertices() {}
	
	FPairedVertices(FVector2D InFirstVertex, FVector2D InSecondVertex)
	: FirstVertex(InFirstVertex), SecondVertex(InSecondVertex) {}
	
	bool IsEquivalent(const FPairedVertices& Other) const
	{
		
		return (FirstVertex == Other.FirstVertex && SecondVertex == Other.SecondVertex) ||
			   (FirstVertex == Other.SecondVertex && SecondVertex == Other.FirstVertex);
		
	}

	
	
	
};


USTRUCT(BlueprintType)
struct FPairedVerticesWith3DInfo
{
	GENERATED_BODY()
public:

	FVector FirstVertexPosition;
	FVector SecondVertexPosition;

	FPairedVerticesWith3DInfo() {}
	
	FPairedVerticesWith3DInfo(FVector InFirstVertex, FVector InSecondVertex)
	: FirstVertexPosition(InFirstVertex), SecondVertexPosition(InSecondVertex) {}

	
	
	
};



USTRUCT(BlueprintType)
struct FVertexIndexPairStruct
{
	
	GENERATED_BODY()
public:
	
	int32 FirstVertexIndex;
	int32 SecondVertexIndex;
};

USTRUCT(BlueprintType)
struct FCellStruct
{
	GENERATED_BODY()
public:
	int32 CellIndex;
	//
	

	TArray<FVertexIndexPairStruct> VertexIndexPairs;
	TArray<FVector> VerticesPosition;

	//
	
	TArray<FPairedVerticesWith3DInfo> VertexPositionPairs;
	//

	int32 CentroidIndex;
	

	FVector CalculateCentroid()
	{
		if (VerticesPosition.Num() == 0)
			return FVector(0,0,0);

		FVector Sum(0.0f, 0.0f, 0.0f);
		for (const FVector& vertexPos : VerticesPosition)
		{
			Sum += vertexPos;
		}
		FVector Centroid = Sum / VerticesPosition.Num();

		
		return Centroid;
	}



	
	
};



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
	static void VoronoiCalculation (UTexture2D* Texture2D, float CellCount);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void CalculateVertices(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void Add4VerticesOnWholeTextureCorner(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void DrawVerticesOnTexture2D(UTexture2D* Texture2D,FColor color);
	UFUNCTION(BlueprintCallable, Category = "Edges Calculation")
	static void AssignCellNumbers(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void MergeSameCornerVertices();
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void Merge4CellCountVertices();
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation")
	static void DrawMergedVerticesOnTexture2D(UTexture2D* Texture2D,FColor color);
	UFUNCTION(BlueprintCallable, Category = "Vertices Calculation HUD")
	static void DrawVertexPositionsAndCellNumbersOnHUD(AHUD* HUD, APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category = "Edges Calculation")
	static void PrintVertexPosAndUniqueCellNumber();
	UFUNCTION(BlueprintCallable, Category = "Edges Calculation")
	static void GroupVerticesWithSharedCells(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Edges Calculation")
	static void PrintPairedVertices();
	UFUNCTION(BlueprintCallable, Category = "Edges Calculation HUD")
	static void DrawDebugEdges(UWorld* World);
	//functions below is used for polygon calculation
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void ProcessVerticesForPolyCalculation(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void AssignVertexUniqueIndex();
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void DistinguishEachCell(UTexture2D* Texture2D);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void GroupEdgesInCells();
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void AssignEachCellStruct();
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void PrintCellsArray();
	//triangles
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void SortVerticesInCells();
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void CreateVoronoiShapePolygon(UProceduralMeshComponent* ProceduralMesh);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void TriangleFanSubdivide(TArray<int32> VtxIndex,TArray<FVector> VtxPos);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static FVector FindKeyByValue(const TMap<FVector, int32>& Map, int32 ValueToFind);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void DivideQuadIntoTriangle(TArray<int32> TwoBaseVerticesIndex, TArray<int32> TwoMiddleVerticesIndex, TArray<int32>& DownLeftTriangle, TArray<int32>& DownRightTriangle);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void CheckWindingOrder(TArray<int32>& VtxIndex);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void MakeTriangle(TArray<int32> VtxIndex);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static float CalculatePolygonArea(const TArray<FVector>& VerticesPos);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static int32 AddVertex(FVector VtxPos);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static FVector RoundVector(FVector Vec, float Precision = 0.001f);

	

	
public:	
	static TArray<FVector2D> VoronoiSeeds;
	static TArray<FVector2D> Vertices;
	static TArray<FVector2D> MergedVertices;
	static TArray<TArray<float>> DistField;
	static void InitializeDistField(UTexture2D* Texture2D);
	static TArray<TArray<float>> GradientField;
	static void InitializeGradientField(UTexture2D* Texture2D);
	static TArray<TArray<FVector2D>> ClosestCellVoronoiSeedXY;
	static void InitializeClosestCellVoronoiSeedXY(UTexture2D* Texture2D);
	static TArray<FVerticesEdgesStruct> VerticesWithUniqueCellNumber;
	static TArray<FVerticesEdgesStruct> MergedCornerVerticesEdges;
	static TArray<FVerticesEdgesStruct> Merged4CellCountVerticesEdges;
	static TMap<FVector2D, int32> CellUniqueNumbers;
	static TArray<FPairedVertices> PairedVertices;
	//below is for poly calculation
	static TArray<FVerticesEdgesStruct> ProcessedVertices;
	static TMap<FVector, int32> VertexIndexMap;
	static TArray<FVector> SameCellVertices;
	static TMap<int32, TArray<FVector>> CellWithVerticesArrays;
	static TMap<int32, TArray<FPairedVerticesWith3DInfo>>  CellWithEdgesArrays;
	static TArray<FCellStruct> Cells;
	//Make Triangles
	static TArray<FVector> WholeVerticesInTexture;
	static TArray<int32> Triangles;
	static TMap<FVector, int32> GlobalVertexIndexMap; 

	//function to make texture 2d

	//function to set pixel of texturer 2d// inefficient/slow

	//function to set pixels without relocking texture (your understand when you do the above)

	//function draw veronoi edges base on input noise texture

	//function to make perlin noise using rand

	//perhPS more random functions with turbulance etc.... look at shader toy

	//far down the lihe, make geometry from texture somehow?

	
};
