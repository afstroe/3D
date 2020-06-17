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

  void decodeNormalsByPolygonVertex(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals, size_t numVerticesPerPolygon)
  {
    const auto polygonCount = mesh->GetPolygonCount();

    auto numPolys = mesh->GetPolygonCount();

    FbxVector4 fbxNormal;

    FOR (poly, numPolys)
    {      
      FOR(index, numVerticesPerPolygon)
      {
        fbxNormal.Set(0, 0, 0);
        auto indexInBuffer = mesh->GetPolygonVertex(poly, index);

        if (indexInBuffer < 0)
        {
          continue;
        }
        mesh->GetPolygonVertexNormal(poly, index, fbxNormal);

        normals[indexInBuffer] += { static_cast<float>(fbxNormal[0]), static_cast<float>(fbxNormal[1]), static_cast<float>(fbxNormal[2]) };
        normals[indexInBuffer].normalize();
      }
      
    }
  }

  void decodeNormals(const FbxMesh* mesh, std::vector<Geometry::Point3<float>>& normals, size_t numIndices, size_t numVerticesPerPolygon)
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
        decodeNormalsByControlPoint(mesh, normals, numVerticesPerPolygon);
      break;
      case FbxLayerElement::eByPolygonVertex:
        decodeNormalsByPolygonVertex(mesh, normals, numVerticesPerPolygon);
      break;

      case FbxLayerElement::eByPolygon:
      case FbxLayerElement::eByEdge:
      case FbxLayerElement::eNone:
      default:
      break;
    }
  }

  void decodeTextureCoordinatesByPolygonVertex(const FbxMesh* mesh, std::vector<Geometry::Point2<float>>& textureCoordinates, size_t numVerticesPerPolygon, const char* uvName)
  {
    const auto polygonCount = mesh->GetPolygonCount();

    auto numPolys = mesh->GetPolygonCount();

    int written = 0;

    auto* uvElement = mesh->GetElementUV(0);
    auto uvReferenceMode = uvElement->GetReferenceMode();

    FOR(poly, numPolys)
    {
      FbxVector2 uv;
      bool unmapped;
      FOR(index, numVerticesPerPolygon)
      {
        auto indexInBuffer = mesh->GetPolygonVertex(poly, index);
        if (indexInBuffer < 0)
        {
          continue;
        }

        if (uvReferenceMode == FbxLayerElement::eIndexToDirect)
        {
          auto polyVertexIndex = mesh->GetPolygonVertexIndex(poly) + index;

          auto ind = uvElement->GetIndexArray().GetAt(polyVertexIndex);
          uv = uvElement->GetDirectArray().GetAt(ind);
          
          FbxVector2 uv2;
          mesh->GetPolygonVertexUV(poly, index, uvName, uv2, unmapped);

          int a = 0;
        }
        else
        {
          mesh->GetPolygonVertexUV(poly, index, uvName, uv, unmapped);
          if (unmapped)
          {
            int a = 0;
          }
        }

        if (1/*textureCoordinates[indexInBuffer].length() == 0*/)
        {
          //double ip = 0;
          //float s = modf(static_cast<double>(uv[0]), &ip);
          //float t = modf(static_cast<double>(uv[1]), &ip);
          //s = s < 0 ? 1 + s : s;
          //t = t < 0 ? 1 + t : t;

          //textureCoordinates[indexInBuffer] = { 1 - s, t };
          textureCoordinates[indexInBuffer] = { static_cast<float>(uv[0]), static_cast<float>(1 - uv[1]) };
          written++;
        }

        //textureCoordinates[indexInBuffer] = { 1 - static_cast<float>(uv[0]), static_cast<float>(uv[1]) };

      }      
    }
  }

  void decodeTextureCoordinates(const FbxMesh* mesh, std::vector<Geometry::Point2<float>>& textureCoordinates, size_t numIndices, size_t numVerticesPerPolygon)
  {
    auto hasUV = mesh->GetElementUVCount() > 0;

    if (!hasUV)
    {
      return;
    }
    
    auto uvMappingMode = mesh->GetElementUV(0)->GetMappingMode();    

    textureCoordinates.resize(numIndices);

    FbxStringList uvNames;
    mesh->GetUVSetNames(uvNames);
    const char* uvName = nullptr;
    if (uvNames.GetCount())
    {
      uvName = uvNames[0];
    }


    switch (uvMappingMode)
    {
      case FbxLayerElement::eByControlPoint:
        ;
      break;
      case FbxLayerElement::eByPolygonVertex:
        decodeTextureCoordinatesByPolygonVertex(mesh, textureCoordinates, numVerticesPerPolygon, uvName);
      break;

      case FbxLayerElement::eByPolygon:
      case FbxLayerElement::eByEdge:
      case FbxLayerElement::eNone:
      default:
      break;
    }
    ;
  }

  void getMeshVertexPool(FbxMesh* mesh, std::vector<Geometry::Point3<float>>& geometryVertices)
  {
    auto numVerticesInPool = mesh->GetControlPointsCount();
    auto fbxVertexPool = mesh->GetControlPoints();    
    geometryVertices.resize(numVerticesInPool);
    FOR(i, numVerticesInPool)
    {
      geometryVertices[i].x = static_cast<float>(fbxVertexPool[i].mData[0]);
      geometryVertices[i].y = static_cast<float>(fbxVertexPool[i].mData[1]);
      geometryVertices[i].z = static_cast<float>(fbxVertexPool[i].mData[2]);
    }
  }

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

  void getIndexPool(FbxMesh* mesh, std::vector<int>& geometryIndices, size_t numVertex)
  {
    if (numVertex > 4)
    {
      // only triangles and quadrilaterals supported for now.
      return;
    }
    auto numPolys = numPolygons(mesh, numVertex);
    auto numIndices = numPolys * numVertex;
    geometryIndices.resize(numIndices);
    size_t crtIndex = 0;
    
    numPolys = mesh->GetPolygonCount();

    FOR(p, numPolys)
    {
      if (numVertex == mesh->GetPolygonSize(p))
      {
        FOR(i, numVertex)
        {
          geometryIndices[crtIndex++] = mesh->GetPolygonVertex(p, i);
        }
      }
    }
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



            //if (strstr(name, "pony") || strstr(name, "Eye"))
            //{
            //  int a = 0;
            //}

            //FbxAMatrix& globalTransform = topOfStack.node->EvaluateGlobalTransform(0);
            //auto& data = globalTransform.mData;

            //glm::mat4 transformMatrix(data[0].mData[0], data[1].mData[0], data[2].mData[0], data[3].mData[0],
            //  data[0].mData[1], data[1].mData[1], data[2].mData[1], data[3].mData[1],
            //  data[0].mData[2], data[1].mData[2], data[2].mData[2], data[3].mData[2],
            //  data[0].mData[3], data[1].mData[3], data[2].mData[3], data[3].mData[3]);
            //
            //transformMatrix = glm::mat4(1);// topOfStack.accumulatedMatrix * transformMatrix;

            //auto* mesh = static_cast<FbxMesh*>(attribute);

            //std::vector<Geometry::Point3<float>> geometryVertices;
            //getMeshVertexPool(mesh, geometryVertices);

            //
            //std::vector<int> geometryIndices;
            //getIndexPool(mesh, geometryIndices, 3);

            //auto numMaterials = topOfStack.node->GetMaterialCount();

            //auto* renderGeometry3 = new PositionedGeometry;

            //// only supporting one material for now
            //if(numMaterials > 0)
            //{
            //  const auto* fbxMaterial = topOfStack.node->GetMaterial(0);

            //  renderGeometry3->material() = decodeMaterial(fbxMaterial);
            //}

            //std::vector<Geometry::Point3<float>> normals;
            //decodeNormals(mesh, normals, geometryVertices.size(), 3);

            //std::vector<Geometry::Point2<float>> textureCoordinates;
            //decodeTextureCoordinates(mesh, textureCoordinates, geometryVertices.size(), 3);
            //
            //renderGeometry3->vertices() = geometryVertices;
            //renderGeometry3->indices() = std::move(geometryIndices);
            //renderGeometry3->normals() = std::move(normals);
            //renderGeometry3->textureCoordinates() = std::move(textureCoordinates);

            //renderGeometry3->createBuffers();
            //renderGeometry3->transform() = transformMatrix;
            //renderGeometry3->mode() = { GL_TRIANGLES, GL_FILL, GL_FRONT };
            //fbxGeometry.push_back(renderGeometry3);

            //getIndexPool(mesh, geometryIndices, 4);

            //if (geometryIndices.size() > 0)
            //{
            //  decodeNormals(mesh, normals, geometryVertices.size(), 4);
            //  decodeTextureCoordinates(mesh, textureCoordinates, geometryVertices.size(), 4);


            //  auto* renderGeometry4 = new PositionedGeometry;
            //  renderGeometry4->vertices() = std::move(geometryVertices);
            //  renderGeometry4->indices() = std::move(geometryIndices);
            //  renderGeometry4->material() = renderGeometry3->material();
            //  renderGeometry4->normals() = std::move(normals);
            //  renderGeometry4->textureCoordinates() = std::move(textureCoordinates);

            //  renderGeometry4->createBuffers();
            //  renderGeometry4->transform() = transformMatrix;
            //  renderGeometry4->mode() = { GL_QUADS, GL_FILL, GL_FRONT };
            //  fbxGeometry.push_back(renderGeometry4);
            //}
          }
          break;
        }
      }
      
      // pop it out of the stack
      parseStack.pop();
    }
  }
}
