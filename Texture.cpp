#include "Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;
    //////////////////////////////////TEXTURES\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
   
        //width, hight, number of color channals, ex 16bit 32 bit 64 bit
    int widthImg, heightImg, numColCh;

    //note to self, open gl reads from bottom left to top right, so it might be upsidedown
    stbi_set_flip_vertically_on_load(true);

    //store image
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    

    //generate a texture
    //(number of textures, refrance)
    glGenTextures(1, &ID);

    //create a texture unit, a texture unit basicly is an index that holds a bunch of images
    glActiveTexture(slot);
    // set/bind texture, texture type and refrance value
    glBindTexture(texType, ID);

    //we can use GL_Nearest, this makes it more pixly when expanding the screen, (pixle art), or GLLinear which creates new pixles
    //input type, setting we want to modify and the value we give our setting
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); //we can use GLLinear too.
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//we can use GLLinear too.


    //now how do we want our textures to be mirrored?, repeat mirrored, clamp to edge and clamp to border
    //we can mix and mach because its only on one axis (X,Y,Z)

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);//side
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);//top

    //note to self: to do border color
    //float flatcolor[] = {1, 1, 1, 1};
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatcolor);


    //generate the texture
    //(type of texutre, type of color channels, width and hight, nothing, type of color channels our image has, Datatype of pixles, image data)
    glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

    //create a mini map
    glGenerateMipmap(texType);



    //cleanup
    //this prevents changing the image when we don't want to do that
    stbi_image_free(bytes);
    glBindTexture(texType, 0);

   

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    //create a uniform
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    shader.Activate();
    glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}