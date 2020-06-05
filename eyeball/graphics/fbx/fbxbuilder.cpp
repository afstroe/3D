#include <stack>

#include "../opengl/glm.h"

#include "fbxbuilder.h"
#include "eyeball/utils/debugout.h"
#include <eyeball/graphics/opengl/geometry.h>
#include <eyeball/graphics/opengl/material.h>


#pragma comment(lib, "zlib-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

namespace {
  std::pair<glm::vec4, std::string> getMaterialChannel(const FbxSurfaceMaterial* material, const char* propertyName, const char* factorPropertyName)
  {
    glm::vec4 channelColor;
    std::string channelTextureFile;

    const auto property = material->FindProperty(propertyName);
    const auto factorProperty = material->FindProperty(factorPropertyName);

    if (property.IsValid())
    {
      auto color = property.Get<FbxDouble3>();
      
      if (factorProperty.IsValid())
      {
        auto factor = factorProperty.Get<FbxDouble>();

        color[0] *= factor;
        color[1] *= factor;
        color[2] *= factor;
      }
      channelColor = { color[0], color[1], color[2], 1.0 };

      if (property.GetSrcObjectCount<FbxFileTexture>() > 0)
      {
        const auto* texture = property.GetSrcObject<FbxFileTexture>();

        if (texture)
        {
          const auto* relativeFileName = texture->GetRelativeFileName();
          if (relativeFileName && strcmp(relativeFileName, ""))
          {
            channelTextureFile = relativeFileName;
          }
          else
          {
            const char* fileName = texture->GetFileName();
            if (fileName && strcmp(fileName, ""))
            {
              channelTextureFile = fileName;
            }
          }
        }
      }
    }
    return std::make_pair(channelColor, channelTextureFile);
  }

  std::shared_ptr<Material> decodeMaterial(const FbxSurfaceMaterial* material)
  {
    auto outMaterial = std::make_shared<Material>();
    
    const auto emissive  = getMaterialChannel(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
    const auto diffuse = getMaterialChannel(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
    const auto ambient = getMaterialChannel(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
    const auto specular = getMaterialChannel(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

    float shininess = 0.0f;
    const auto shininessProperty = material->FindProperty(FbxSurfaceMaterial::sShininess);
    if (shininessProperty.IsValid())
    {
      shininess = static_cast<float>(shininessProperty.Get<FbxDouble>());      
    }

    outMaterial->emissive() = emissive.first; outMaterial->emissiveTexture() = emissive.second;
    outMaterial->ambient() = ambient.first; outMaterial->ambientTexture() = ambient.second;
    outMaterial->diffuse() = diffuse.first; outMaterial->diffuseTexture() = diffuse.second;
    outMaterial->specular() = specular.first; outMaterial->specularTexture() = specular.second;
    outMaterial->shininess() = shininess;

    return outMaterial;
  }

  void decodeNormalsByControlPoint(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals)
  {
    debugLog("Normals by control point not yet supported");
  }

  enum class NumPolygonVerts
  {
    Triangle = 3,
    Quad = 4
  };

  void decodeNormalsByPolygonVertex(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals, NumPolygonVerts numPolygonVerts)
  {
    const auto polygonCount = mesh->GetPolygonCount();

    const auto* normalElement = mesh->GetElementNormal(0);

    auto numPolys = mesh->GetPolygonCount();
    auto numNormalsInVector = 0;

    FbxVector4 fbxNormal;

    FbxArray<FbxVector4> fbxNormals;
    mesh->GetPolygonVertexNormals(fbxNormals);

    int a = 0;

    //FOR (poly, numPolys)
    //{
    //  int numVerts = mesh->GetPolygonSize(poly);
    //  if (numVerts == static_cast<int>(numPolygonVerts))
    //  {
    //    FOR(index, static_cast<int>(numPolygonVerts))
    //    {
    //      auto indexInBuffer = mesh->GetPolygonVertex(poly, index);
    //      auto success = mesh->GetPolygonVertexNormal(poly, indexInBuffer, fbxNormal);
    //      normals[numNormalsInVector++] = { static_cast<float>(fbxNormal[0]), static_cast<float>(fbxNormal[1]), static_cast<float>(fbxNormal[2]) };
    //    }
    //  }
    //  
    //}
  }

  void decodeNormals(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals, size_t numIndices, NumPolygonVerts numPolygonVerts)
  {
    auto hasNormal = mesh->GetElementNormalCount() > 0;
    if (!hasNormal)
    {
      return;
    }

    normals.resize(numIndices);

    auto normalMappingMode = mesh->GetElementNormal(0)->GetMappingMode();

    switch (normalMappingMode)
    {
      case FbxLayerElement::eByControlPoint:
        decodeNormalsByControlPoint(mesh, normals);
      break;
      case FbxLayerElement::eByPolygonVertex:
        decodeNormalsByPolygonVertex(mesh, normals, numPolygonVerts);
      break;

      case FbxLayerElement::eByPolygon:
      case FbxLayerElement::eByEdge:
      case FbxLayerElement::eNone:
      default:
      break;
    }
  }
}

void FbxBuilder::load(const char* fileName, std::vector<PositionedGeometry*>& fbxGeometry)
{
  // Create the FBX SDK manager
  auto* fbxManager = FbxManager::Create();

  // Create an IOSettings object.
  auto* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
  fbxManager->SetIOSettings(ios);

  // Create an importer.
  auto* importer = FbxImporter::Create(fbxManager, "");

  // Initialize the importer.
  bool importSuccessful = importer->Initialize(fileName, -1, fbxManager->GetIOSettings());  

  if (!importSuccessful)
  {
    debugLog("Call to FbxImporter::Initialize() failed.");
    debugLog("Error returned: %", importer->GetStatus().GetErrorString());
  }
  else
  {
    // Create a new scene so it can be populated by the imported file.
    auto* scene = FbxScene::Create(fbxManager, "myScene");

    // Import the contents of the file into the scene.
    importer->Import(scene);

    // Convert mesh, NURBS and patch into triangle mesh
    FbxGeometryConverter geomConverter(fbxManager);
    try {
      geomConverter.Triangulate(scene, /*replace*/true);
    }
    catch (std::runtime_error) {
      debugLog("Scene integrity verification failed.\n");
      return;
    }

    parseScene(scene, fbxGeometry);

    scene->Destroy();
  }
  

  importer->Destroy();
  ios->Destroy();
  fbxManager->Destroy();
}

void FbxBuilder::parseScene(FbxScene* scene, std::vector<PositionedGeometry*>& fbxGeometry)
{
  struct ParseControl 
  {
    int lastParsedChildIdx = 0;
    FbxNode* node = nullptr;
    glm::mat4 accumulatedMatrix = glm::mat4(1);
  };

  auto* node = scene->GetRootNode();

  if (!node)
  {
    return;
  }

  std::stack<ParseControl> parseStack;
  parseStack.push({ 0, node, glm::mat4(1) });

  while (!parseStack.empty())
  {
    auto& topOfStack = parseStack.top();
    const auto* name = topOfStack.node->GetName();
    debugLog(name);
    if (topOfStack.node->GetChildCount() > topOfStack.lastParsedChildIdx)
    {
      FbxAMatrix& globalTransform = topOfStack.node->EvaluateGlobalTransform(0);
      auto& data = globalTransform.mData;
     
      glm::mat4 transformMatrix(data[0].mData[0], data[1].mData[0], data[2].mData[0], data[3].mData[0], 
                                data[0].mData[1], data[1].mData[1], data[2].mData[1], data[3].mData[1], 
                                data[0].mData[2], data[1].mData[2], data[2].mData[2], data[3].mData[2], 
                                data[0].mData[3], data[1].mData[3], data[2].mData[3], data[3].mData[3]);

      parseStack.push({ 0, topOfStack.node->GetChild(topOfStack.lastParsedChildIdx), transformMatrix });
      topOfStack.lastParsedChildIdx++;
    }
    else
    {
      // leaf node - parse it
      auto* attribute = topOfStack.node->GetNodeAttribute();

      const auto* name = topOfStack.node->GetName();      

      if (attribute)
      {
        auto attributeType = attribute->GetAttributeType();
        switch (attributeType)
        {
          case FbxNodeAttribute::eMesh:
          {
            auto* mesh = static_cast<FbxMesh*>(attribute);

            auto numVerticesInPool = mesh->GetControlPointsCount();
            auto fbxVertexPool = mesh->GetControlPoints();
            std::vector<Geometry::Point3<float>> geometryVertices;
            geometryVertices.resize(numVerticesInPool);
            FOR(i, numVerticesInPool)
            {
              geometryVertices[i].x =  static_cast<float>(fbxVertexPool[i].mData[0]);
              geometryVertices[i].y =  static_cast<float>(fbxVertexPool[i].mData[1]);
              geometryVertices[i].z = -static_cast<float>(fbxVertexPool[i].mData[2]);
            }

            auto numVertsInPolys = mesh->GetPolygonVertexCount();
            auto fbxIndexBuffer = mesh->GetPolygonVertices();
            auto numPolys = mesh->GetPolygonCount();

            size_t numIndicesInTriangles = 0;
            size_t numIndicesInQuads = 0;
            FOR(p, numPolys)
            {
              switch (int numVerts = mesh->GetPolygonSize(p))
              {
                case 3:
                {
                  numIndicesInTriangles += 3;
                }
                break;
                case 4:
                {
                  numIndicesInQuads += 4;
                }
                break;
                default:
                  debugLog("Polygon with % vertices not supported", numVerts);

              }
            }

            std::vector<int> geometryIndicesTriangles;
            geometryIndicesTriangles.resize(numIndicesInTriangles);
            std::vector<int> geometryIndicesQuads;
            geometryIndicesQuads.resize(numIndicesInQuads);
            size_t crtIndexTris = 0;
            size_t crtIndexQuads = 0;
            FOR(p, numPolys)
            {
              switch (mesh->GetPolygonSize(p))
              {
                case 3:
                {
                  FOR(i, 3)
                  {
                    geometryIndicesTriangles[crtIndexTris++] = mesh->GetPolygonVertex(p, i);
                  }
                }
                break;
                case 4:
                {
                  FOR(i, 4)
                  {
                    geometryIndicesQuads[crtIndexQuads++] = mesh->GetPolygonVertex(p, i);
                  }
                }
                break;
              }
            }

            //geometryIndices.assign(fbxIndexBuffer, fbxIndexBuffer + numVertsInPolys + 1);

            auto numMaterials = topOfStack.node->GetMaterialCount();

            auto* renderGeometry = new PositionedGeometry;

            // only supporting one material for now
            if(numMaterials > 0)
            {
              const auto* fbxMaterial = topOfStack.node->GetMaterial(0);

              renderGeometry->material() = decodeMaterial(fbxMaterial);
            }

            std::vector<Geometry::Point3<float>> normalsInQuads;
            // decodeNormals(mesh, normalsInQuads, numIndicesInQuads, NumPolygonVerts::Quad);
            
            renderGeometry->vertices() = geometryVertices;
            renderGeometry->indices() = std::move(geometryIndicesQuads);
            renderGeometry->normals() = std::move(normalsInQuads);

            renderGeometry->createBuffers();            
            renderGeometry->transform() = topOfStack.accumulatedMatrix;
            renderGeometry->mode() = { GL_QUADS, GL_FILL, GL_FRONT_AND_BACK };            
            fbxGeometry.push_back(renderGeometry);

            auto material = renderGeometry->material();

            std::vector<Geometry::Point3<float>> normalsInTriangles;
            decodeNormals(mesh, normalsInTriangles, numIndicesInTriangles, NumPolygonVerts::Triangle);

            renderGeometry = new PositionedGeometry;
            renderGeometry->material() = material;
            renderGeometry->vertices() = std::move(geometryVertices);
            renderGeometry->indices() = std::move(geometryIndicesTriangles);
            renderGeometry->normals() = std::move(normalsInTriangles);

            renderGeometry->createBuffers();
            renderGeometry->transform() = topOfStack.accumulatedMatrix;
            renderGeometry->mode() = { GL_TRIANGLES, GL_FILL, GL_FRONT_AND_BACK };
            fbxGeometry.push_back(renderGeometry);
          }
          break;
        }
      }
      
      // pop it out of the stack
      parseStack.pop();
    }
  }

  int a = 0;
}
