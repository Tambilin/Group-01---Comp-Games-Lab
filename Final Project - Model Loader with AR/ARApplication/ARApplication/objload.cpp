#include "objload.h"

objload::objload(){
	g_enableTextures = true;
}

void objload::enableTextures(bool g_textured){
	g_enableTextures = g_textured;
}

void objload::enableBackFaceCulling(bool g_cullBackFaces){
		if (g_cullBackFaces)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
}

void objload::LoadModel(const char *pszFilename)
{
    // Import the OBJ file and normalize to unit length.

    SetCursor(LoadCursor(0, IDC_WAIT));

    if (!g_model.import(pszFilename))
    {
        //SetCursor(LoadCursor(0, IDC_ARROW));
        throw std::runtime_error("Failed to load model.");
    }

    g_model.normalize();

    // Load any associated textures.
    // Note the path where the textures are assumed to be located.

    const ModelOBJ::Material *pMaterial = 0;
    GLuint textureId = 0;
    std::string::size_type offset = 0;
    std::string filename;

    for (int i = 0; i < g_model.getNumberOfMaterials(); ++i)
    {
        pMaterial = &g_model.getMaterial(i);

        // Look for and load any diffuse color map textures.

        if (pMaterial->colorMapFilename.empty())
            continue;

        // Try load the texture using the path in the .MTL file.
        textureId = LoadTexture(pMaterial->colorMapFilename.c_str());

        if (!textureId)
        {
            offset = pMaterial->colorMapFilename.find_last_of('\\');

            if (offset != std::string::npos)
                filename = pMaterial->colorMapFilename.substr(++offset);
            else
                filename = pMaterial->colorMapFilename;

            // Try loading the texture from the same directory as the OBJ file.
            textureId = LoadTexture((g_model.getPath() + filename).c_str());
        }

        if (textureId)
            g_modelTextures[pMaterial->colorMapFilename] = textureId;

        // Look for and load any normal map textures.

        if (pMaterial->bumpMapFilename.empty())
            continue;

        // Try load the texture using the path in the .MTL file.
        textureId = LoadTexture(pMaterial->bumpMapFilename.c_str());

        if (!textureId)
        {
            offset = pMaterial->bumpMapFilename.find_last_of('\\');

            if (offset != std::string::npos)
                filename = pMaterial->bumpMapFilename.substr(++offset);
            else
                filename = pMaterial->bumpMapFilename;

            // Try loading the texture from the same directory as the OBJ file.
            textureId = LoadTexture((g_model.getPath() + filename).c_str());
        }

        if (textureId)
            g_modelTextures[pMaterial->bumpMapFilename] = textureId;
    }

    SetCursor(LoadCursor(0, IDC_ARROW));
}

GLuint objload::LoadTexture(const char *pszFilename)
{
    GLuint id = 0;
    Bitmap bitmap;

    if (bitmap.loadPicture(pszFilename))
    {
        // The Bitmap class loads images and orients them top-down.
        // OpenGL expects bitmap images to be oriented bottom-up.
        bitmap.flipVertical();

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bitmap.width, bitmap.height,
            GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap.getPixels());
    }

    return id;
}

GLuint objload::CreateNullTexture(int width, int height)
{
    // Create an empty white texture. This texture is applied to OBJ models
    // that don't have any texture maps. This trick allows the same shader to
    // be used to draw the OBJ model with and without textures applied.

    int pitch = ((width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
    std::vector<GLubyte> pixels(pitch * height, 255);
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
        GL_UNSIGNED_BYTE, &pixels[0]);

    return texture;
}

void objload::DrawModelUsingFixedFuncPipeline()
{
    const ModelOBJ::Mesh *pMesh = 0;
    const ModelOBJ::Material *pMaterial = 0;
    const ModelOBJ::Vertex *pVertices = 0;
    ModelTextures::const_iterator iter;

    for (int i = 0; i < g_model.getNumberOfMeshes(); ++i)
    {
        pMesh = &g_model.getMesh(i);
        pMaterial = pMesh->pMaterial;
        pVertices = g_model.getVertexBuffer();

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pMaterial->ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pMaterial->diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMaterial->shininess * 128.0f);

        if (g_enableTextures)
        {
            iter = g_modelTextures.find(pMaterial->colorMapFilename);

            if (iter == g_modelTextures.end())
            {
                glDisable(GL_TEXTURE_2D);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, iter->second);
				//cout << "BINDING TEXTURE" << endl;
            }
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }

        if (g_model.hasPositions())
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, g_model.getVertexSize(),
                g_model.getVertexBuffer()->position);
        }

        if (g_model.hasTextureCoords())
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, g_model.getVertexSize(),
                g_model.getVertexBuffer()->texCoord);
        }

        if (g_model.hasNormals())
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, g_model.getVertexSize(),
                g_model.getVertexBuffer()->normal);
        }

        glDrawElements(GL_TRIANGLES, pMesh->triangleCount * 3, GL_UNSIGNED_INT,
            g_model.getIndexBuffer() + pMesh->startIndex);

        if (g_model.hasNormals())
            glDisableClientState(GL_NORMAL_ARRAY);

        if (g_model.hasTextureCoords())
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        if (g_model.hasPositions())
            glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void objload::InitGL()
{
    /*glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
}

void objload::UnloadModel()
{
    SetCursor(LoadCursor(0, IDC_WAIT));

    ModelTextures::iterator i = g_modelTextures.begin();

    while (i != g_modelTextures.end())
    {
        glDeleteTextures(1, &i->second);
        ++i;
    }

    g_modelTextures.clear();
    g_model.destroy();

    SetCursor(LoadCursor(0, IDC_ARROW));
}

objload::~objload()  
{
	UnloadModel();
} // end destructor