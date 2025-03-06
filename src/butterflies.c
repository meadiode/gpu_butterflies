
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define SUPPORT_TRACELOG        1
#define SUPPORT_TRACELOG_DEBUG  1
#include <utils.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define TARGET_FPS 240

const int SCREE_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

static bool show_controls = true;

static Mesh butt_mesh = {0};
static Texture2D butt_tex = {0};
static Texture2D butt_tex2 = {0};
static Shader butt_shader = {0};
static Material butt_mat = {0};

static Camera camera = {0};

static float t_factor = 0.02;
static float dispersion = 8.3;
static int nx = 3;
static int ny = 7;
static int nz = 11;
static Vector2 phi = {0.1, 0.7};
static Vector3 scale = {30.0, 14.0, 30.0};
static Vector3 offset = {0.0, 0.0, 0.0};
static Color butt_color1 = {0, 204, 255, 255};
static Color butt_color2 = {255, 192, 0, 255};
static float butt_distrib = 0.5;
static int n_butterflies = 1000;

static int time_loc = -1;
static int dispersion_loc = -1;
static int nx_loc = -1;
static int ny_loc = -1;
static int nz_loc = -1;
static int phi_loc = -1;
static int scale_loc = -1;
static int offset_loc = -1;
static int distrib_loc = -1;
static int color1_loc = - 1;
static int color2_loc = - 1;


static void init_assets(void);
static void main_loop(void);
static void draw_instances(Mesh mesh, Material material, int instances);
static void draw_controls(void);


int main(void)
{
    SetTraceLogLevel(LOG_DEBUG);

    InitWindow(SCREE_WIDTH, SCREEN_HEIGHT, "GPU Butterflies");
    SetExitKey(KEY_ESCAPE);

    camera.position = (Vector3){40.0f, 40.0f, 5.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    init_assets();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    // SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        main_loop();
    }

    CloseWindow();
#endif

    return 0;
}


static void main_loop(void)
{
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        int n_upd = IsKeyDown(KEY_LEFT_SHIFT) ? 10 : 1;
        for (int i = 0; i < n_upd; i++)
        {
            UpdateCamera(&camera, CAMERA_FREE);
        }
    }

    if (IsKeyPressed(KEY_O))
    {
        show_controls = !show_controls;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    float time = GetTime() * t_factor;

    SetShaderValue(butt_shader, time_loc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(butt_shader, dispersion_loc, &dispersion, SHADER_UNIFORM_FLOAT);
    SetShaderValue(butt_shader, nx_loc, &nx, SHADER_UNIFORM_INT);
    SetShaderValue(butt_shader, ny_loc, &ny, SHADER_UNIFORM_INT);
    SetShaderValue(butt_shader, nz_loc, &nz, SHADER_UNIFORM_INT);
    SetShaderValue(butt_shader, phi_loc, &phi, SHADER_UNIFORM_VEC2);
    SetShaderValue(butt_shader, scale_loc, &scale, SHADER_UNIFORM_VEC3);
    SetShaderValue(butt_shader, offset_loc, &offset, SHADER_UNIFORM_VEC3);
    SetShaderValue(butt_shader, distrib_loc, &butt_distrib, SHADER_UNIFORM_FLOAT);

    Vector4 nc1 = ColorNormalize(butt_color1);
    SetShaderValue(butt_shader, color1_loc, &nc1, SHADER_UNIFORM_VEC4);
    Vector4 nc2 = ColorNormalize(butt_color2);
    SetShaderValue(butt_shader, color2_loc, &nc2, SHADER_UNIFORM_VEC4);

    BeginMode3D(camera);
    
    rlDisableBackfaceCulling();
    
    draw_instances(butt_mesh, butt_mat, n_butterflies);

    DrawGrid(20, 1.0f);

    EndMode3D();

    if (show_controls)
    {
        draw_controls();
    }

    EndDrawing();    
}


static void init_assets(void)
{
    const float VERTICES[3 * 6] =
    {
        -0.5,  0.5,  0.0,    0.0,  0.5,  0.0,    0.5,  0.5,  0.0,
        -0.5, -0.5,  0.0,    0.0, -0.5,  0.0,    0.5, -0.5,  0.0,
    };

    const float NORMALS[3 * 6] =
    {
        0.0,  0.0,  1.0,     0.0,  0.0,  1.0,    0.0,  0.0,  1.0,
        0.0,  0.0,  1.0,     0.0,  0.0,  1.0,    0.0,  0.0,  1.0,
    };

    const float TEXCOORDS[2 * 6] =
    {
        0.0,  1.2,   1.0,  1.2,   0.0,  1.2,
        0.0, -0.2,   1.0, -0.2,   0.0, -0.2,
    };

    const unsigned short INDICES[4 * 3] =
    {
        0, 3, 4,  0, 4, 1,  1, 4, 5,  1, 5, 2,
    };


    butt_mesh.triangleCount = 4;
    butt_mesh.vertexCount = 6;
    butt_mesh.vertices = MemAlloc(sizeof(VERTICES));
    butt_mesh.normals = MemAlloc(sizeof(NORMALS));
    butt_mesh.texcoords = MemAlloc(sizeof(TEXCOORDS));
    butt_mesh.indices = MemAlloc(sizeof(INDICES));

    memcpy(butt_mesh.vertices, VERTICES, sizeof(VERTICES));
    memcpy(butt_mesh.normals, NORMALS, sizeof(NORMALS));
    memcpy(butt_mesh.texcoords, TEXCOORDS, sizeof(TEXCOORDS));
    memcpy(butt_mesh.indices, INDICES, sizeof(INDICES));

    UploadMesh(&butt_mesh, false);

    butt_tex = LoadTexture("./resources/butterfly01.png");
    SetTextureWrap(butt_tex, TEXTURE_WRAP_CLAMP);

    butt_tex2 = LoadTexture("./resources/butterfly02.png");
    SetTextureWrap(butt_tex2, TEXTURE_WRAP_CLAMP);

    butt_shader = LoadShader("./resources/butterflies.vs", "./resources/butterflies.fs");
    
    time_loc = GetShaderLocation(butt_shader, "time");
    dispersion_loc = GetShaderLocation(butt_shader, "dispersion");
    nx_loc = GetShaderLocation(butt_shader, "nx");
    ny_loc = GetShaderLocation(butt_shader, "ny");
    nz_loc = GetShaderLocation(butt_shader, "nz");
    phi_loc = GetShaderLocation(butt_shader, "phi");
    scale_loc = GetShaderLocation(butt_shader, "scale");
    offset_loc = GetShaderLocation(butt_shader, "offset");
    distrib_loc = GetShaderLocation(butt_shader, "distribution");
    color1_loc = GetShaderLocation(butt_shader, "color1");
    color2_loc = GetShaderLocation(butt_shader, "color2");

    butt_mat = LoadMaterialDefault();
    butt_mat.shader = butt_shader;

    butt_mat.maps[MATERIAL_MAP_ALBEDO].texture = butt_tex;
    butt_mat.maps[MATERIAL_MAP_METALNESS].texture = butt_tex2;
}


static void draw_instances(Mesh mesh, Material material, int instances)
{
    rlEnableShader(material.shader.id);

    Matrix matView = rlGetMatrixModelview();
    Matrix matModelView = MatrixIdentity();
    Matrix matProjection = rlGetMatrixProjection();

    rlEnableVertexArray(mesh.vaoId);

    rlDisableVertexBuffer();
    rlDisableVertexArray();

    matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

    int mdiff = MATERIAL_MAP_DIFFUSE;
    rlActiveTextureSlot(MATERIAL_MAP_DIFFUSE);
    rlEnableTexture(material.maps[MATERIAL_MAP_DIFFUSE].texture.id);
    rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + MATERIAL_MAP_DIFFUSE],
                 &mdiff, SHADER_UNIFORM_INT, 1);

    mdiff = MATERIAL_MAP_METALNESS;
    rlActiveTextureSlot(MATERIAL_MAP_METALNESS);
    rlEnableTexture(material.maps[MATERIAL_MAP_METALNESS].texture.id);
    rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + MATERIAL_MAP_METALNESS],
                 &mdiff, SHADER_UNIFORM_INT, 1);

    rlEnableVertexArray(mesh.vaoId);

    Matrix matModelViewProjection = MatrixIdentity();
    matModelViewProjection = MatrixMultiply(matModelView, matProjection);
    rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

    rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount * 3, 0, instances);

    rlActiveTextureSlot(MATERIAL_MAP_DIFFUSE);
    rlDisableTexture();

    rlActiveTextureSlot(MATERIAL_MAP_METALNESS);
    rlDisableTexture();

    rlDisableVertexArray();
    rlDisableShader();
}


static void draw_controls(void)
{
    const int MULTIPLIERS[] = {1, 10, 100, 1000, 10000, 100000, 1000000};
    const char *MP_STR = "x1;x10;x100;x1000;x10000;x100000;x1000000"; 
    
    static char str[40] = {0};
    static int mp_id = 3;

    GuiPanel((Rectangle){4, 4, 320, 100 + 28 * 14 + 84}, "GPU Butterflies");

    snprintf(str, sizeof(str), "FPS: %d", GetFPS());
    DrawText(str, 20, 40, 20, DARKGRAY);

    snprintf(str, sizeof(str), "BUTTERFLIES: %d", n_butterflies);
    DrawText(str, 20, 60, 20, DARKGRAY);

    float fnum = (float)n_butterflies / (float)MULTIPLIERS[mp_id];
    snprintf(str, sizeof(str), "Base number: %d", (int)fnum);
    GuiSliderBar((Rectangle){20, 100 + 28 * 0, 140, 20}, NULL, str, &fnum, 1.0, 100.0);
    if (GuiComboBox((Rectangle){20, 100 + 28 * 1, 140, 20}, MP_STR, &mp_id))
    {
        fnum /= 10.0;
    }

    n_butterflies = (int)fnum * MULTIPLIERS[mp_id];

    GuiSliderBar((Rectangle){20, 100 + 28 * 2, 140, 20}, NULL, "t factor", &t_factor, 0.001, 0.1);
    GuiSliderBar((Rectangle){20, 100 + 28 * 3, 140, 20}, NULL, "dispersion", &dispersion, 0.0, 80.0);

    GuiSpinner((Rectangle){20, 100 + 28 * 4, 140, 20}, "nx", &nx, 1, 3000, false);
    GuiSpinner((Rectangle){20, 100 + 28 * 5, 140, 20}, "ny", &ny, 1, 3000, false);
    GuiSpinner((Rectangle){20, 100 + 28 * 6, 140, 20}, "nz", &nz, 1, 3000, false);
    GuiSliderBar((Rectangle){20, 100 + 28 * 7, 140, 20}, NULL, "phi_x", &phi.x, 0.0, PI * 2.0);
    GuiSliderBar((Rectangle){20, 100 + 28 * 8, 140, 20}, NULL, "phi_y", &phi.y, 0.0, PI * 2.0);
    GuiSliderBar((Rectangle){20, 100 + 28 * 9, 140, 20}, NULL, "scale_x", &scale.x, 0.1, 300.0);
    GuiSliderBar((Rectangle){20, 100 + 28 * 10, 140, 20}, NULL, "scale_y", &scale.y, 0.1, 300.0);
    GuiSliderBar((Rectangle){20, 100 + 28 * 11, 140, 20}, NULL, "scale_z", &scale.z, 0.1, 300.0);
    GuiSliderBar((Rectangle){20, 100 + 28 * 12, 140, 20}, NULL, "distribution", &butt_distrib, 0.0, 1.0);

    // GuiSliderBar((Rectangle){20, 100 + 28 * 12, 140, 20}, NULL, "offset_x", &offset.x, -100.0, 100.0);
    // GuiSliderBar((Rectangle){20, 100 + 28 * 13, 140, 20}, NULL, "offset_y", &offset.y, -100.0, 100.0);
    // GuiSliderBar((Rectangle){20, 100 + 28 * 14, 140, 20}, NULL, "offset_z", &offset.z, -100.0, 100.0);
    // GuiColorPicker((Rectangle){20, 100 + 28 * 13, 100, 80}, "color 1", &butt_color1);
    // GuiColorPicker((Rectangle){20, 100 + 28 * 13 + 80 + 8, 100, 80}, "color 2", &butt_color2);


    DrawText("O to show/hide controls", 20, 100 + 28 * 14, 20, GRAY);
    DrawText("RMB + WSAD to navigate", 20, 100 + 28 * 15, 20, GRAY);
    DrawText("+SHIFT to navigate faster", 20, 100 + 28 * 16, 20, GRAY);

}