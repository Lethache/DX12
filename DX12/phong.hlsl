cbuffer SceneData : register(b0)
{
    float4x4 WVP;
    float4 DiffuseColor;
    float4 AmbientLight;

    float3 LightDirection;
    float pad0;

    float4 LightColor;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

Texture2D g_diffuse : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = mul(float4(input.position, 1.0f), WVP);
    result.normal = input.normal;
    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 lightDir = normalize(-LightDirection);

    float lambertian = saturate(
        dot(lightDir, normalize(input.normal)));

    float4 color =
        g_diffuse.Sample(g_sampler, input.uv)
        * DiffuseColor
        * LightColor;

    float4 intensity = AmbientLight + lambertian;

    return color * intensity;
}