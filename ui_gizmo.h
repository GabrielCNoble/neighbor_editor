#ifndef UI_GIZMO_H
#define UI_GIZMO_H

#include "lib/dstuff/ds_matrix.h"
#include <stdint.h>

enum TRANSFORM_OP
{
    TRANSFORM_OP_TRANSLATION = 0,
    TRANSFORM_OP_ROTATION,
    TRANSFORM_OP_SCALE,
    TRANSFORM_OP_BOUNDS
};

enum TRANSFORM_MODE
{
    TRANSFORM_MODE_LOCAL = 0,
    TRANSFORM_MODE_WORLD
};

#ifdef __cplusplus
extern "C"
{
#endif

void ui_gizmo_BeginFrame();

void ui_gizmo_EndFrame();

void ui_gizmo_SetRect(float x, float y, float w, float h);

void ui_gizmo_Manipulate(mat4_t *view_matrix, mat4_t *projection_matrix, uint32_t operation, uint32_t mode, mat4_t *gizmo_transform);

#ifdef __cplusplus
}
#endif

#endif // UI_GIZMO_H
