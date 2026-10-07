cbuffer SceneData : register(b0)
{
    float4x4 WVP;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = mul(float4(input.position, 1.0f), WVP);
    result.color = input.color;
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 col0 = g_texture0.Sample(g_sampler, input.uv);
    float4 col1 = g_texture1.Sample(g_sampler, input.uv);

    return lerp(col0, col1, 0.3f) * input.color;
}