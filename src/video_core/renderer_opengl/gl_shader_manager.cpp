// Copyright 2018 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "video_core/renderer_opengl/gl_shader_manager.h"

namespace OpenGL::GLShader {

using Tegra::Engines::Maxwell3D;

void MaxwellUniformData::SetFromRegs(const Maxwell3D& maxwell, std::size_t shader_stage) {
    const auto& regs = maxwell.regs;
    const auto& state = maxwell.state;

    // TODO(bunnei): Support more than one viewport
    viewport_flip[0] = regs.viewport_transform[0].scale_x < 0.0 ? -1.0f : 1.0f;
    viewport_flip[1] = regs.viewport_transform[0].scale_y < 0.0 ? -1.0f : 1.0f;

    u32 func = static_cast<u32>(regs.alpha_test_func);
    // Normalize the gl variants of opCompare to be the same as the normal variants
    const u32 op_gl_variant_base = static_cast<u32>(Maxwell3D::Regs::ComparisonOp::Never);
    if (func >= op_gl_variant_base) {
        func = func - op_gl_variant_base + 1U;
    }

    alpha_test.enabled = regs.alpha_test_enabled;
    alpha_test.func = func;
    alpha_test.ref = regs.alpha_test_ref;

    instance_id = state.current_instance;

    // Assign in which stage the position has to be flipped
    // (the last stage before the fragment shader).
    constexpr u32 geometry_index = static_cast<u32>(Maxwell3D::Regs::ShaderProgram::Geometry);
    if (maxwell.regs.shader_config[geometry_index].enable) {
        flip_stage = geometry_index;
    } else {
        flip_stage = static_cast<u32>(Maxwell3D::Regs::ShaderProgram::VertexB);
    }

    // Y_NEGATE controls what value S2R returns for the Y_DIRECTION system value.
    y_direction = regs.screen_y_control.y_negate == 0 ? 1.f : -1.f;
}

} // namespace OpenGL::GLShader
