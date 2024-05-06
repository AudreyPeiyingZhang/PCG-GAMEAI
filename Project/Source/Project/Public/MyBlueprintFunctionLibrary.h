// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */



UENUM(BlueprintType)
enum class EPlane : uint8
{
	XY,
	XZ,
	YZ
};


USTRUCT(BlueprintType)
struct FVerticesEdgesStruct
{
	GENERATED_BODY()

public:

	FVector2D VertexPosition;
	TSet<int32> CurrentCellsUniqueNumbers;

	bool IsContainOtherArray( const FVerticesEdgesStruct Other) const
	{

		for (const int32& Element : Other.CurrentCellsUniqueNumbers)
		{
			if (!CurrentCellsUniqueNumbers.Contains(Element)) 
			{
				return false;
			}
		}

		return true;


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
	
	FVector CalculateIncenter()
	{
		if (VerticesPosition.Num() < 3)
		{
			return FVector(0, 0, 0); // Not enough vertices
		}

		FVector WeightedIncenter(0.0f, 0.0f, 0.0f);
		float TotalWeight = 0.0f;
		
		for (int i = 0; i < VerticesPosition.Num(); ++i)
		{
			
			FVector CurrentVertex = VerticesPosition[i];
			FVector NextVertex = VerticesPosition[(i + 1) % VerticesPosition.Num()];
			FVector PrevVertex = VerticesPosition[(i - 1 + VerticesPosition.Num()) % VerticesPosition.Num()];

		
			float Edge1Length = (CurrentVertex - NextVertex).Size();
			float Edge2Length = (CurrentVertex - PrevVertex).Size();

		
			float Weight = Edge1Length + Edge2Length;

			
			WeightedIncenter += CurrentVertex * Weight;


			TotalWeight += Weight;
		}

		
		if (TotalWeight > 0.0f)
		{
			WeightedIncenter /= TotalWeight;
		}

		return WeightedIncenter;
		
	}




	
	
};


USTRUCT(BlueprintType)
struct FVertexData
{
	GENERATED_BODY()

public:
	// Position
	FVector VtxPos;
	// Index
	int32 VtxIndex;
	// Local UV
	FVector2D VtxUV0;
	// World UV
	FVector2D VtxUV1;
	// Distinguish UV Channel
	FVector2D VtxUV2;
	// Normal
	FVector VtxNormal;

	// Default constructor with initializers
	FVertexData()
		: VtxPos(0.0f, 0.0f, 0.0f)
		, VtxIndex(0)
		, VtxUV0(0.0f, 0.0f)
		, VtxUV1(0.0f, 0.0f)
		, VtxUV2(0.0f, 0.0f)
		, VtxNormal(0.0f, 0.0f, 0.0f)
	{
	}

	// Parameterized constructor for custom initialization
	FVertexData(const FVector& InVtxPos, int32 InVtxIndex, const FVector2D& InVtxUV0, const FVector2D& InVtxUV1, const FVector2D& InVtxUV2, const FVector& InVtxNormal)
		: VtxPos(InVtxPos)
		, VtxIndex(InVtxIndex)
		, VtxUV0(InVtxUV0)
		, VtxUV1(InVtxUV1)
		, VtxUV2(InVtxUV2)
		, VtxNormal(InVtxNormal)
	{
	}

	// Equality operator to support comparison
	friend bool operator==(const FVertexData& Lhs, const FVertexData& Rhs)
	{
		return Lhs.VtxPos == Rhs.VtxPos &&
			   Lhs.VtxIndex == Rhs.VtxIndex &&
			   Lhs.VtxUV0 == Rhs.VtxUV0 &&
			   Lhs.VtxUV1 == Rhs.VtxUV1 &&
			   Lhs.VtxUV2 == Rhs.VtxUV2 &&
			   Lhs.VtxNormal == Rhs.VtxNormal;
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
	static void VoronoiCalculation (UTexture2D* Texture2D);
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
	static void MergeSpecialCaseWithFourOrMoreVertices(TArray<FVerticesEdgesStruct>& Array);
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
	static void CreateVoronoiShapePolygon(UProceduralMeshComponent* ProceduralMesh, UMaterialInterface* MaterialInstance);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void TriangleFanSubdivide(FVertexData PreVtx, FVertexData NextNextVtx,TArray<int32> VtxIndex,TArray<FVertexData> VtxData);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static FVector CalculateBisector(FVector VtxA, FVector VtxB, FVector VtxC);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static FVector FindKeyByValue(const TMap<FVector, int32>& Map, int32 ValueToFind);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void DivideQuadIntoTriangle(TArray<int32> TwoBaseVerticesIndex, TArray<int32> TwoMiddleVerticesIndex, TArray<int32>& DownLeftTriangle, TArray<int32>& DownRightTriangle);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void CheckWindingOrder(TArray<FVertexData>& VtxData, EPlane Plane = EPlane::XY);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void MakeTriangle(TArray<int32> VtxIndex);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static float CalculatePolygonArea(const TArray<FVector>& VerticesPos, EPlane Plane = EPlane::XY);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static int32 AddVertex(FVertexData VertexData);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static FVector RoundVector(FVector Vec, float Precision = 0.001f);
	UFUNCTION(BlueprintCallable, Category = "Polygons Calculation")
	static void ExtrudePolygon(TArray<int32> BaseTriangle, TArray<FVertexData> BaseVtxData,TArray<int32>& ExtrudeTriangles);
	UFUNCTION(BlueprintCallable, Category = "Normal Distribution")
	static float UseNormalDistributionToGetBuildingHeight(FVector2D CurrentPos);

	//set voronoi seed Parameter
	UFUNCTION(BlueprintCallable, Category = "Normal Distribution")
	static void SetCityCenterHeightSigma( float maxHeight, FVector2D centerPos, float sigmaX, float sigmaY);
	UFUNCTION(BlueprintCallable, Category = "Set Voronoi Parameter")
	static void SetVoronoiSeed(FVector2D vectorSeedA, float aOffset, float aAmplitude, FVector2D vectorSeedB, float bOffset, float bAmplitude );
	UFUNCTION(BlueprintCallable, Category = "Set Voronoi Parameter")
	static void SetCellCount(int32 cellCount);

	//road width
	UFUNCTION(BlueprintCallable, Category = "Set Road Width")
	static void SetRoadWidth(float roadWidth);

	//clear all data
	UFUNCTION(BlueprintCallable, Category = "Clear Data")
	static void ClearAllArrays();


	

	
public:	
	static TArray<FVector2D> VoronoiSeeds;
	static TArray<FVector2D> Vertices;
	static TArray<FVector2D> MergedVertices;
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
	static TArray<FVector> WholeVertices;
	static TArray<int32> Triangles;
	static TArray<int32> WholeVerticesIndex;
	//static TMap<FVertexData, int32> GlobalVertexIndexMap;
	static TArray<FVector2D> UV0;
	static TArray<FVector2D> UV1;
	static TArray<FVector2D> UV2;
	static TArray<FVector> Normal;
	static float UVScale;
	//normal distribution
	static float MaxHeight;
	static FVector2D CityCenterPos;
	static float SigmaX;
	static float SigmaY;

	//road width
	static float RoadWidth;
	
	//setvoronoi
	static FVector2D VectorASeed;
	static float AOffset;
	static float AAmplitude;
	static FVector2D VectorBSeed;
	static float BOffset;
	static float BAmplitude;

	//set cell count
	static int32 CellCount;

	//function to make texture 2d

	//function to set pixel of texturer 2d// inefficient/slow

	//function to set pixels without relocking texture (your understand when you do the above)

	//function draw veronoi edges base on input noise texture

	//function to make perlin noise using rand

	//perhPS more random functions with turbulance etc.... look at shader toy

	//far down the lihe, make geometry from texture somehow?

	
};
