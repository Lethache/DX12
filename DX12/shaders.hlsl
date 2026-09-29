cbuffer CameraBuffer : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 projection;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(
    float3 position : POSITION,
    float4 color : COLOR)
{
    PSInput result;

    float4 worldPosition = float4(position, 1.0f);
    float4 viewPosition = mul(worldPosition, view);

    result.position = mul(viewPosition, projection);
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}