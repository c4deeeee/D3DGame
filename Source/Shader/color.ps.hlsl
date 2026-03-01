struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 ColorPixelShader(PixelInput input) : SV_Target
{
    float4 outColor = mul(input.color, 0.5f);
    return outColor;
}