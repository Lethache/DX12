struct Material
{
    float4 DiffuseColor;
    float4 SpecularColor;

    float SpecularPower;
    float SpecularStrength;
    float2 Padding;
};

struct Light
{
    float4 AmbientColor;

    float3 Position;
    float Padding;

    float4 Color;

    // X = constant, Y = linear, Z = quadratic.
    float3 ConstLinQuad;
    float Padding1;
};

cbuffer SceneData : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4x4 NormalMatrix;

    Material material;
    Light light;

    float4 CameraPosition;

    uint IsLightCube;
    uint UseSpecularMap;
    float2 ScenePadding;
};

Texture2D g_diffuse : register(t0);
Texture2D g_specular : register(t1);

SamplerState g_sampler : register(s0);

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

float3 SafeNormalize(float3 value)
{
    return value * rsqrt(max(dot(value, value), 0.000001f));
}

PSInput VSMain(VSInput input)
{
    PSInput output;

    output.position =
        mul(float4(input.position, 1.0f), WVP);

    output.worldPosition =
        mul(float4(input.position, 1.0f), World).xyz;

    output.normal =
        mul(float4(input.normal, 0.0f), NormalMatrix).xyz;

    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // The light marker itself does not fade.
    if (IsLightCube != 0)
    {
        return material.DiffuseColor;
    }

    float4 diffuseSample =
        g_diffuse.Sample(g_sampler, input.uv);

    float3 specularSample = float3(1.0f, 1.0f, 1.0f);

    if (UseSpecularMap != 0)
    {
        specularSample =
            g_specular.Sample(g_sampler, input.uv).rgb;
    }

    float3 normal = SafeNormalize(input.normal);

    float3 lightOffset =
        light.Position - input.worldPosition;

    float distanceToLight = length(lightOffset);

    float3 lightDirection = SafeNormalize(lightOffset);

    float3 viewDirection =
        SafeNormalize(CameraPosition.xyz - input.worldPosition);

    float diffuseAmount =
        saturate(dot(normal, lightDirection));

    float specularAmount = 0.0f;

    if (diffuseAmount > 0.0f)
    {
        float3 reflectedDirection =
            reflect(-lightDirection, normal);

        specularAmount = pow(
            saturate(dot(reflectedDirection, viewDirection)),
            max(material.SpecularPower, 1.0f));
    }

    float attenuationDenominator =
        light.ConstLinQuad.x
        + light.ConstLinQuad.y * distanceToLight
        + light.ConstLinQuad.z * distanceToLight * distanceToLight;

    float attenuation =
        1.0f / max(attenuationDenominator, 0.000001f);

    float3 baseColor =
        diffuseSample.rgb * material.DiffuseColor.rgb;

    // Ambient remains visible even far from the point light.
    float3 ambient =
        baseColor * light.AmbientColor.rgb;

    float3 diffuse =
        baseColor
        * light.Color.rgb
        * diffuseAmount;

    float3 specular =
        specularSample
        * material.SpecularColor.rgb
        * light.Color.rgb
        * material.SpecularStrength
        * specularAmount;

    // Distance attenuation affects diffuse and specular lighting.
    float3 finalColor =
        ambient + (diffuse + specular) * attenuation;

    return float4(
        saturate(finalColor),
        diffuseSample.a * material.DiffuseColor.a);
}