#include <stack>
#include <unordered_set>

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
          //const auto* relativeFileName = texture->GetRelativeFileName();
          //if (relativeFileName && strcmp(relativeFileName, ""))
          //{
          //  channelTextureFile = relativeFileName;
          //}
          //else
          //{
            const char* fileName = texture->GetFileName();
            if (fileName && strcmp(fileName, ""))
            {
              channelTextureFile = fileName;
            }
          //}
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

    outMaterial->emissive() = emissive.first; outMaterial->textureFile(Material::TextureNames::Emissive) = emissive.second;
    outMaterial->ambient() = ambient.first; outMaterial->textureFile(Material::TextureNames::Ambient) = ambient.second;
    outMaterial->diffuse() = diffuse.first; outMaterial->textureFile(Material::TextureNames::Diffuse) = diffuse.second;
    outMaterial->specular() = specular.first; outMaterial->textureFile(Material::TextureNames::Specular) = specular.second;
    outMaterial->shininess() = shininess;
    outMaterial->name() = material->GetName();

    return outMaterial;
  }

  void decodeNormalsByControlPoint(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals, size_t numVerticesPerPolygon)
  {
    UNREFERENCED_PARAMETER(mesh);
    UNREFERENCED_PARAMETER(normals);
    UNREFERENCED_PARAMETER(numVerticesPerPolygon);
    debugLog("Normals by control point not yet supported");
  }

  enum class NumPolygonVerts
  {
    Triangle = 3,
    Quad = 4
  };


  size_t numPolygons(FbxMesh* mesh, size_t numVertex)
  {
    size_t num = 0;
    auto numPolys = mesh->GetPolygonCount();

    FOR(poly, numPolys)
    {
      if (mesh->GetPolygonSize(poly) == numVertex)
      {
        ++num;
      }
    }

    return num;
  }

  void getVertexData(FbxMesh* mesh, std::vector<Geometry::Vertex>& vertexData, std::vector<int>& indexBuffer)
  {    
    auto numPolys = mesh->GetPolygonCount();
    auto numTris = numPolygons(mesh, 3);

    // Bit of a sanity check
    if (numPolys == numTris)
    {
      debugLog("Mesh % contains N-gons", mesh->GetName());
    }

    indexBuffer.resize(numPolys * 3);

    auto hasUV = mesh->GetElementUVCount() > 0;

    const char* uvName = nullptr;
    if (hasUV)
    {
      FbxStringList uvNames;
      mesh->GetUVSetNames(uvNames);

      if (uvNames.GetCount())
      {
        uvName = uvNames[0];
      }
    }
 
    // worst case scenario
    // each face has separate polygons
    std::unordered_set<Geometry::Vertex, Geometry::Vertex::Hash> vertexDataMap;

    // the fbx mesh's vertex pool
    auto* fbxVertexCache = mesh->GetControlPoints();
  
    Geometry::Point3<float> position;
    Geometry::Point3<float> normal;
    Geometry::Point2<float> textureCoordinates;

    FbxVector4 fbxNormal;
    FbxVector2 fbxUV;
    bool unmapped = false;

    Geometry::Vertex cvertex;

    // iterate through the polygons
    // find the vertex in the fbx mesh vertex pool
    // see if we already have it in our data map
    size_t indexInVB = 0;
    size_t indexInIB = 0;
    FOR(poly, numPolys)
    {
      FOR(index, 3)
      {
        fbxNormal.Set(0, 0, 0);
        fbxUV.Set(0, 0);       
        position.set(0, 0, 0);
        normal.set(0, 0, 0);
        textureCoordinates.set(0, 0);

        auto i = mesh->GetPolygonVertex(poly, index);
        position.x = static_cast<float>(fbxVertexCache[i][0]);
        position.y = static_cast<float>(fbxVertexCache[i][1]);
        position.z = static_cast<float>(fbxVertexCache[i][2]);

        if (mesh->GetPolygonVertexNormal(poly, index, fbxNormal))
        {
          normal.set(static_cast<float>(fbxNormal[0]), static_cast<float>(fbxNormal[1]), static_cast<float>(fbxNormal[2]));
        }
        if (mesh->GetPolygonVertexUV(poly, index, uvName, fbxUV, unmapped))
        {
          textureCoordinates.set(static_cast<float>(fbxUV[0]), static_cast<float>(1 - fbxUV[1]));
        }

        cvertex.position = position;
        cvertex.normal = normal;
        cvertex.textureCoordinates = textureCoordinates;
        cvertex.index = indexInVB;

        auto ite = vertexDataMap.find(cvertex);

        if (ite == vertexDataMap.end())
        {
          vertexDataMap.insert(cvertex);
          indexBuffer[indexInIB++] = static_cast<int>(indexInVB++);
        }
        else
        {
          indexBuffer[indexInIB++] = static_cast<int>((*ite).index);
        }

        int a = 0;
      }
    }

    vertexData.resize(vertexDataMap.size());
    for (auto& v : vertexDataMap)
    {
      vertexData[v.index] = v;
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

    // Convert Axis System to what is used in this example, if needed
    FbxAxisSystem sceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem ourAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    if (sceneAxisSystem != ourAxisSystem)
    {
      ourAxisSystem.ConvertScene(scene);
    }

    //// Convert Unit System to what is used in this example, if needed
    FbxSystemUnit sceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
    if (sceneSystemUnit.GetScaleFactor() != 1.0)
    {
      //The unit in this example is centimeter.
      FbxSystemUnit::cm.ConvertScene(scene);
    }

    //auto lAxisSytemReference = scene->GetGlobalSettings().GetAxisSystem();
    //int lUpVectorSign = 1;
    //int lFrontVectorSign = 1;
    ////get upVector and its sign.
    //auto lUpVector = lAxisSytemReference.GetUpVector(lUpVectorSign);
    ////get FrontVector and its sign.
    //auto lFrontVector = lAxisSytemReference.GetFrontVector(lFrontVectorSign);
    ////get uCoorSystem. 
    //auto lCoorSystem = lAxisSytemReference.GetCoorSystem();

    // Convert mesh, NURBS and patch into triangle mesh
    FbxGeometryConverter geomConverter(fbxManager);

    try 
    {
      geomConverter.Triangulate(scene, true);
    }
    catch (std::runtime_error) {
      debugLog("Scene integrity verification failed.\n");
      return;
    }

    parseScene(scene, fbxGeometry, geomConverter);

    scene->Destroy();
  }
  

  importer->Destroy();
  ios->Destroy();
  fbxManager->Destroy();
}

void FbxBuilder::parseScene(FbxScene* scene, std::vector<PositionedGeometry*>& fbxGeometry, FbxGeometryConverter& geometryConverter)
{
  UNREFERENCED_PARAMETER(geometryConverter);

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

      transformMatrix = glm::mat4(1);// topOfStack.accumulatedMatrix * transformMatrix;

      parseStack.push({ 0, topOfStack.node->GetChild(topOfStack.lastParsedChildIdx), transformMatrix });
      topOfStack.lastParsedChildIdx++;
    }
    else
    {
      // leaf node - parse it
      auto* attribute = topOfStack.node->GetNodeAttribute();      

      if (attribute)
      {
        auto attributeType = attribute->GetAttributeType();
        switch (attributeType)
        {
          case FbxNodeAttribute::eMesh:
          {
            auto* mesh = static_cast<FbxMesh*>(attribute);
            std::vector<Geometry::Vertex> vertexData;
            std::vector<int> indexBuffer;
            getVertexData(mesh, vertexData, indexBuffer);

            auto* renderGeometry = new PositionedGeometry;

            auto numMaterials = topOfStack.node->GetMaterialCount();
            if(numMaterials > 0)
            {
              const auto* fbxMaterial = topOfStack.node->GetMaterial(0);

              renderGeometry->material() = decodeMaterial(fbxMaterial);
            }
            renderGeometry->vertices().resize(vertexData.size());
            renderGeometry->normals().resize(vertexData.size());
            renderGeometry->textureCoordinates().resize(vertexData.size());
            
            size_t index = 0;
            for (const auto& v : vertexData)
            {
              renderGeometry->vertices()[index] = v.position;
              renderGeometry->normals()[index] = v.normal;
              renderGeometry->textureCoordinates()[index++] = v.textureCoordinates;
            }
            renderGeometry->indices() = std::move(indexBuffer);

            renderGeometry->createBuffers();

            fbxGeometry.push_back(renderGeometry);
          }
          break;
        }
      }
      
      // pop it out of the stack
      parseStack.pop();
    }
  }
}
