#pragma once
#include "UObject.h"

class UTexture2D;

struct FTextureLookup
{
  int32 TexCoordIndex = 0;
  int32 TextureIndex = 0;

  float UScale = 1.;
  float VScale = 1.;

  friend FStream& operator<<(FStream& s, FTextureLookup& l);
};

class FMaterial {
public:
  void Serialize(FStream& s);

  std::vector<FString> CompoilerErrors;
  std::map<PACKAGE_INDEX, int32> TextureDependencyLengthMap;
  int32 MaxTextureDependencyLength = 0;
  FGuid Id;
  uint32 NumUserTexCoords = 0;
  std::vector<PACKAGE_INDEX> UniformExpressionTextures;
  bool bUsesSceneColor = false;
  bool bUsesSceneDepth = false;
  bool bUsesDynamicParameter = false;
  bool bUsesLightmapUVs = false;
  bool bUsesMaterialVertexPositionOffset = false;
  uint32 UsingTransforms = 0;
  std::vector<FTextureLookup> TextureLookups;
  uint32 FallbackComponents = 0;
  uint32 Unk1 = 0;
  uint32 Unk2 = 0;
  uint32 Unk3 = 0;
};

struct FStaticSwitchParameter {
  FName ParameterName;
  bool Value = false;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FStaticSwitchParameter& p);
};

struct FStaticComponentMaskParameter {
  FName ParameterName;
  bool R = false;
  bool G = false;
  bool B = false;
  bool A = false;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FStaticComponentMaskParameter& p);
};

struct FNormalParameter {
  FName ParameterName;
  uint8 CompressionSettings = 0;
  bool bOverride = false;
  FGuid ExpressionGUID;

  friend FStream& operator<<(FStream& s, FNormalParameter& p);
};

struct FStaticTerrainLayerWeightParameter {
  FName ParameterName;
  bool bOverride = false;
  FGuid ExpressionGUID;
  int32 WeightmapIndex = 0;

  friend FStream& operator<<(FStream& s, FStaticTerrainLayerWeightParameter& p);
};

struct FStaticParameterSet {
  FGuid BaseMaterialId;
  std::vector<FStaticSwitchParameter> StaticSwitchParameters;
  std::vector<FStaticComponentMaskParameter> StaticComponentMaskParameters;
  std::vector<FNormalParameter> NormalParameters;
  std::vector<FStaticTerrainLayerWeightParameter> TerrainLayerWeightParameters;

  friend FStream& operator<<(FStream& s, FStaticParameterSet& ps);
};

class UMaterialInterface : public UObject {
public:
  DECL_UOBJ(UMaterialInterface, UObject);

  bool RegisterProperty(FPropertyTag* property) override;

  UTexture2D* GetTextureParameterValue(const FString& name) const;
  UTexture2D* GetDiffuseTexture() const;
  EBlendMode GetBlendMode() const;
  UObject* GetParent() const;

protected:
  UPROP(std::vector<FPropertyValue*>, TextureParameterValues, {});
  UPROP(EBlendMode, BlendMode, EBlendMode::BLEND_Opaque);
  UPROP(PACKAGE_INDEX, Parent, INDEX_NONE);
};

class UMaterial : public UMaterialInterface {
public:
  DECL_UOBJ(UMaterial, UMaterialInterface);

  void Serialize(FStream& s) override;

protected:
  FMaterial MaterialResource;
};

class UMaterialInstance : public UMaterialInterface {
public:
  DECL_UOBJ(UMaterialInstance, UMaterialInterface);

  UPROP(bool, bHasStaticPermutationResource, false);

  void Serialize(FStream& s) override;

  bool RegisterProperty(FPropertyTag* property) override;

  FMaterial StaticPermutationResource;
  FStaticParameterSet StaticParameters;
};

class UMaterialInstanceConstant : public UMaterialInstance {
public:
  DECL_UOBJ(UMaterialInstanceConstant, UMaterialInstance);
};

// UObjectFactory uses 'Component' keyword to detect UComponents but
// UMaterialExpressionComponentMask is not a component. Define it here as a UObject
// to fix incorrect class construction
class UMaterialExpressionComponentMask : public UObject {
public:
  DECL_UOBJ(UMaterialExpressionComponentMask, UObject);
};