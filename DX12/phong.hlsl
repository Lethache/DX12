cbuffer SceneData : register(b0)
{
    float4x4 WVP;
    float4x4 NormalMatrix;

    float4 DiffuseColor;
    float4 AmbientLight;

    float3 LightDirection;
    float pad0;

    float4 LightColor;

    uint IsLightCube;
    float3 pad1;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

Texture2D g_diffuse : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position =
        mul(float4(input.position, 1.0f), WVP);

    result.normal =
        mul(float4(input.normal, 0.0f), NormalMatrix).xyz;

    result.uv = input.uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    if (IsLightCube != 0)
    {
        return DiffuseColor;
    }

    float3 normal = normalize(input.normal);
    float3 toLight = normalize(-LightDirection);

    float lambertian = saturate(dot(normal, toLight));

    float4 textureColor =
        g_diffuse.Sample(g_sampler, input.uv);

    float3 lighting =
        AmbientLight.rgb + LightColor.rgb * lambertian;

    float3 color =
        textureColor.rgb * DiffuseColor.rgb * lighting;

    return float4(
        saturate(color),
        textureColor.a * DiffuseColor.a);
}