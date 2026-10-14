cbuffer SceneData : register(b0)
{
    float4x4 WVP;
    float4 yuvControls;
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
};

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = mul(float4(input.position, 1.0f), WVP);
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 rgb = input.color.rgb;

    float Y = dot(rgb, float3(
        0.299f, 0.587f, 0.114f));

    float U = dot(rgb, float3(
        -0.14713f, -0.28886f, 0.436f));

    float V = dot(rgb, float3(
        0.615f, -0.51499f, -0.10001f));

    Y *= yuvControls.x;
    U *= yuvControls.y;
    V *= yuvControls.z;

    float3 result;

    result.r = Y + 1.13983f * V;
    result.g = Y - 0.39465f * U - 0.58060f * V;
    result.b = Y + 2.03211f * U;

    result = saturate(result);

    if (yuvControls.w > 0.5f)
    {
        result = 1.0f - result;
    }

    return float4(result, input.color.a);
}