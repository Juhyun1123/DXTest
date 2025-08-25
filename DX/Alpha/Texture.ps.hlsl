
Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
	
	// 이 텍스쳐 좌표 위치에서 샘플러를 사용해 텍스처에서 픽셀 색상을 샘플링
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	return textureColor;
}