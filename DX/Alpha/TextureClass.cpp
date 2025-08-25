#include "stdafx.h"
#include "TextureClass.h"

#include "DDSTextureLoader11.h"

TextureClass::TextureClass()
{
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Init(ID3D11Device* device, const WCHAR* filename)
{
    if (FAILED(CreateDDSTextureFromFile(device, filename, &texture, &textureView)))
    {
        return false;
    }
    return true;
}

void TextureClass::Shutdown()
{
    if (textureView)
    {
        textureView->Release();
        textureView = nullptr;
    }

    if (texture)
    {
        texture->Release();
        texture = nullptr;
    }
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return textureView;
}
