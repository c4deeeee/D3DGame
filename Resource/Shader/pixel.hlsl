Texture2D texture2d;
SamplerState sample;

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInput input) : SV_Target
{
    float4 textureColor = texture2d.Sample(sample, input.tex);
    
    return textureColor;
}