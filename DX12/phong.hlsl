cbuffer SceneData : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4x4 NormalMatrix;

    float4 DiffuseColor;
    float4 AmbientLight;

    float3 LightPosition;
    float pad0;

    float4 LightColor;

    float SpecularPower;
    float SpecularStrength;
    uint IsLightCube;
    float pad1;

    float4 SpecularColor;
    float4 CameraPosition;
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
    float3 worldPosition : TEXCOORD1;
};

Texture2D g_diffuse : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position =
        mul(float4(input.position, 1.0f), WVP);

    result.worldPosition =
        mul(float4(input.position, 1.0f), World).xyz;

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

    float3 lightOffset =
        LightPosition - input.worldPosition;

    float3 lightDirection =
        lightOffset * rsqrt(max(dot(lightOffset, lightOffset), 0.000001f));

    float3 viewOffset =
        CameraPosition.xyz - input.worldPosition;

    float3 viewDirection =
        viewOffset * rsqrt(max(dot(viewOffset, viewOffset), 0.000001f));

    float diffuseAmount =
        saturate(dot(normal, lightDirection));

    float3 reflection =
        reflect(-lightDirection, normal);

    float specularAmount = 0.0f;

    if (diffuseAmount > 0.0f)
    {
        specularAmount = pow(
            saturate(dot(reflection, viewDirection)),
            max(SpecularPower, 1.0f));
    }

    float4 textureColor =
        g_diffuse.Sample(g_sampler, input.uv);

    float3 diffuse =
        textureColor.rgb
        * DiffuseColor.rgb
        * (AmbientLight.rgb + LightColor.rgb * diffuseAmount);

    float3 specular =
        SpecularColor.rgb
        * LightColor.rgb
        * SpecularStrength
        * specularAmount;

    return float4(
        saturate(diffuse + specular),
        textureColor.a * DiffuseColor.a);
}