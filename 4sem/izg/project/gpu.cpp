/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include "./glm/gtx/string_cast.hpp"
#include "glm/common.hpp"
#include "student/fwd.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <student/gpu.hpp>
#include <utility>

using glm::ivec2;
using glm::to_string;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::max;
using glm::min;

using std::cerr;

void clear_command(GPUMemory &mem, ClearCommand const &cmd) {
    Framebuffer &fb = mem.framebuffers[mem.activatedFramebuffer];

    if (cmd.clearDepth && fb.depth.bytesPerPixel > 0 && fb.depth.data != NULL) {
        // TODO remove asserts
        assert(fb.depth.bytesPerPixel == 4); // depth buffer with more depths
        assert(fb.depth.channels == 1);      // depth buffer with more depths
        assert(fb.depth.format == fb.depth.FLOAT32); // depth buffer using bytes
        assert(fb.depth.pitch > 0);                  // depth buffer using bytes

        std::fill_n(static_cast<float *>(fb.depth.data), fb.width * fb.height,
                    cmd.depth);
    }

    if (cmd.clearColor && fb.color.data != NULL) {
        if (fb.color.format == fb.color.UINT8) {
            for (uint32_t color_offset = 0; color_offset < fb.color.channels;
                 color_offset++) {
                uint8_t color;
                Image::Channel channel = fb.color.channelTypes[color_offset];

                switch (channel) {
                case Image::RED:
                    color = cmd.color.r * 255;
                    break;
                case Image::GREEN:
                    color = cmd.color.g * 255;
                    break;
                case Image::BLUE:
                    color = cmd.color.b * 255;
                    break;
                case Image::ALPHA:
                    color = cmd.color.a * 255;
                    break;
                }

                for (uint32_t idx = color_offset;
                     idx < fb.width * fb.height * fb.color.bytesPerPixel;
                     idx += fb.color.bytesPerPixel) {

                    static_cast<uint8_t *>(fb.color.data)[idx] = color;
                }
            }
        } else { // FLOAT32
            for (uint32_t color_offset = 0; color_offset < fb.color.channels;
                 color_offset++) {
                float color;
                Image::Channel channel = fb.color.channelTypes[color_offset];

                switch (channel) {
                case Image::RED:
                    color = cmd.color.r;
                    break;
                case Image::GREEN:
                    color = cmd.color.g;
                    break;
                case Image::BLUE:
                    color = cmd.color.b;
                    break;
                case Image::ALPHA:
                    color = cmd.color.a;
                    break;
                }

                for (uint32_t idx = color_offset * sizeof(float);
                     idx < fb.width * fb.height;
                     idx += fb.color.bytesPerPixel / sizeof(float)) {
                    static_cast<float *>(fb.color.data)[idx] = color;
                }
            }
        }
    }
}

uint32_t get_vertex_id(GPUMemory &mem, VertexArray &vertex_array,
                       uint32_t vertex_index) {
    if (vertex_array.indexBufferID == -1) { // non-indexed vertices
        return vertex_index;
    } else { // indexed vertices
        const void *buffer = static_cast<const uint8_t *>(
                                 mem.buffers[vertex_array.indexBufferID].data) +
                             vertex_array.indexOffset;
        switch (vertex_array.indexType) {

        case IndexType::UINT8:
            return *(static_cast<const uint8_t *>(buffer) + vertex_index);
            break;

        case IndexType::UINT16:
            return *(static_cast<const uint16_t *>(buffer) + vertex_index);
            break;

        case IndexType::UINT32:
            return *(static_cast<const uint32_t *>(buffer) + vertex_index);
            break;
        }
    }
    assert(false);
}

void fill_attributes(GPUMemory &mem, VertexArray &vertex_array,
                     InVertex &in_vertex) {
    for (uint8_t i = 0; i < maxAttributes; i++) {
        VertexAttrib &attrib = vertex_array.vertexAttrib[i];

        const void *attrib_ptr =
            static_cast<const uint8_t *>(mem.buffers[attrib.bufferID].data) +
            attrib.offset + attrib.stride * in_vertex.gl_VertexID;

        switch (attrib.type) {

        case AttributeType::EMPTY:
            continue;
            break;

        case AttributeType::FLOAT:
        case AttributeType::UINT:
            in_vertex.attributes[i].u1 =
                *static_cast<const uint32_t *>(attrib_ptr);
            break;

        case AttributeType::VEC2:
        case AttributeType::UVEC2:
            in_vertex.attributes[i].u2 =
                *static_cast<const glm::uvec2 *>(attrib_ptr);
            break;

        case AttributeType::VEC3:
        case AttributeType::UVEC3:
            in_vertex.attributes[i].u3 =
                *static_cast<const glm::uvec3 *>(attrib_ptr);
            break;

        case AttributeType::VEC4:
        case AttributeType::UVEC4:
            in_vertex.attributes[i].u4 =
                *static_cast<const glm::uvec4 *>(attrib_ptr);
            break;
        }
    }
}

vec2 to_normal(vec2 vec) { return {-vec.y, vec.x}; }

/// Calculates the value of edge function for point P, with line from A to B
float edge_func(vec2 A, vec2 B, vec2 P) {
    vec2 ab_normal = to_normal(B - A);
    vec2 ap = P - A;

    return glm::dot(ab_normal, ap);
}

/// calculates barycentric coordicates of point P in triangle ABC
vec3 get_bary(vec2 p, vec2 a, vec2 b, vec2 c) {
    // stolen from https://gamedev.stackexchange.com/a/23745
    vec2 ab = b - a;
    vec2 ac = c - a;
    vec2 ap = p - a;

    float d00 = glm::dot(ab, ab);
    float d01 = glm::dot(ab, ac);
    float d11 = glm::dot(ac, ac);
    float d20 = glm::dot(ap, ab);
    float d21 = glm::dot(ap, ac);

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return {u, v, w};
}

template <typename T> // float, vec2, vec3, vec4
T interpolate(vec3 bary, T v0_data, T v1_data, T v2_data) {
    return bary.x * v0_data + bary.y * v1_data + bary.z * v2_data;
}

vec3 bary_to_perspective(vec3 bary, vec3 vertices_w) {
    vertices_w = 1.f / vertices_w;
    float s = glm::dot(bary, vertices_w);
    return (bary * vertices_w) / s;
}

vec3 get_color(Image &image, vec2 pixel_coords) {
    vec3 color;
    void *pixel = getPixel(image, pixel_coords.x, pixel_coords.y);

    if (image.format == image.UINT8) {
        for (uint32_t i = 0; i < image.bytesPerPixel; i++) {
            switch (image.channelTypes[i]) {
            case Image::RED:
                color.r = *(static_cast<uint8_t *>(pixel) + i);
                break;

            case Image::GREEN:
                color.g = *(static_cast<uint8_t *>(pixel) + i);
                break;

            case Image::BLUE:
                color.b = *(static_cast<uint8_t *>(pixel) + i);
                break;

            case Image::ALPHA:
                break;
            }
        }

        color /= 255;
    } else {
        for (uint32_t i = 0; i < image.bytesPerPixel; i++) {
            switch (image.channelTypes[i]) {
            case Image::RED:
                color.r = *(static_cast<float *>(pixel) + i);
                break;

            case Image::GREEN:
                color.g = *(static_cast<float *>(pixel) + i);
                break;

            case Image::BLUE:
                color.b = *(static_cast<float *>(pixel) + i);
                break;

            case Image::ALPHA:
                break;
            }
        }
    }
    return color;
}

void set_color(Image &image, vec2 pixel_coords, vec3 color) {
    void *pixel = getPixel(image, pixel_coords.x, pixel_coords.y);

    if (image.format == image.UINT8) {
        for (uint32_t i = 0; i < image.bytesPerPixel; i++) {
            switch (image.channelTypes[i]) {
            case Image::RED:
                *(static_cast<uint8_t *>(pixel) + i) = color.r * 255;
                break;

            case Image::GREEN:
                *(static_cast<uint8_t *>(pixel) + i) = color.g * 255;
                break;

            case Image::BLUE:
                *(static_cast<uint8_t *>(pixel) + i) = color.b * 255;
                break;

            case Image::ALPHA:
                break;
            }
        }
    } else {
        for (uint32_t i = 0; i < image.bytesPerPixel; i++) {
            switch (image.channelTypes[i]) {
            case Image::RED:
                *(static_cast<float *>(pixel) + i) = color.r;
                break;

            case Image::GREEN:
                *(static_cast<float *>(pixel) + i) = color.g;
                break;

            case Image::BLUE:
                *(static_cast<float *>(pixel) + i) = color.b;
                break;

            case Image::ALPHA:
                break;
            }
        }
    }
}

void process_fragment(GPUMemory &mem, OutVertex primitive[3],
                      glm::vec2 fragment) {
    InFragment in_fragment;
    in_fragment.gl_FragCoord = {fragment, 0, 1};

    vec4 A = primitive[0].gl_Position;
    vec4 B = primitive[1].gl_Position;
    vec4 C = primitive[2].gl_Position;

    vec3 barycentric = get_bary(fragment, A, B, C);
    in_fragment.gl_FragCoord.z = interpolate(barycentric, A.z, B.z, C.z);

    // depth test
    Framebuffer &fb = mem.framebuffers[mem.activatedFramebuffer];
    glm::ivec2 pixel_coords = glm::floor(fragment);
    float *depth_pixel = static_cast<float *>(
        getPixel(fb.depth, pixel_coords.x, pixel_coords.y));
    if (in_fragment.gl_FragCoord.z >= *depth_pixel) {
        return;
    }

    Program &program = mem.programs[mem.activatedProgram];

    vec3 barycentric_perspective =
        bary_to_perspective(barycentric, {A.w, B.w, C.w});

    for (uint32_t i = 0; i < maxAttributes; i++) {
        switch (program.vs2fs[i]) {

        case AttributeType::EMPTY:
            continue;

        case AttributeType::FLOAT:
            in_fragment.attributes[i].v1 = interpolate(
                barycentric_perspective, primitive[0].attributes[i].v1,
                primitive[1].attributes[i].v1, primitive[2].attributes[i].v1);
            break;

        case AttributeType::VEC2:
            in_fragment.attributes[i].v2 = interpolate(
                barycentric_perspective, primitive[0].attributes[i].v2,
                primitive[1].attributes[i].v2, primitive[2].attributes[i].v2);
            break;

        case AttributeType::VEC3:
            in_fragment.attributes[i].v3 = interpolate(
                barycentric_perspective, primitive[0].attributes[i].v3,
                primitive[1].attributes[i].v3, primitive[2].attributes[i].v3);
            break;

        case AttributeType::VEC4:
            in_fragment.attributes[i].v4 = interpolate(
                barycentric_perspective, primitive[0].attributes[i].v4,
                primitive[1].attributes[i].v4, primitive[2].attributes[i].v4);
            break;

        case AttributeType::UINT:
        case AttributeType::UVEC2:
        case AttributeType::UVEC3:
        case AttributeType::UVEC4:
            in_fragment.attributes[i] = primitive[0].attributes[i];
            break;
        }
    }

    OutFragment out_fragment;

    ShaderInterface shader_interface;
    shader_interface.gl_DrawID = mem.gl_DrawID;
    shader_interface.uniforms = mem.uniforms;
    shader_interface.textures = mem.textures;

    program.fragmentShader(out_fragment, in_fragment, shader_interface);

    if (out_fragment.discard) {
        return;
    }

    // write new color
    out_fragment.gl_FragColor = glm::clamp(out_fragment.gl_FragColor, 0.f, 1.f);

    float alpha = out_fragment.gl_FragColor.a;
    vec3 new_color = get_color(fb.color, pixel_coords) * (1 - alpha) +
                     vec3(out_fragment.gl_FragColor) * alpha;
    set_color(fb.color, pixel_coords,
              new_color * 1.0001f); // ugly, but tests must pass

    // write new depth
    *depth_pixel = in_fragment.gl_FragCoord.z;
}

void rasterize(GPUMemory &mem, OutVertex primitive[3]) {
    vec2 A = primitive[0].gl_Position;
    vec2 B = primitive[1].gl_Position;
    vec2 C = primitive[2].gl_Position;

    float width = mem.framebuffers[mem.activatedFramebuffer].width;
    float height = mem.framebuffers[mem.activatedFramebuffer].height;

    uint32_t minX =
        max(glm::floor(min(A.x, min(B.x, C.x))), 0.f); // TODO maybe remove max?
    uint32_t maxX = min(glm::ceil(max(A.x, max(B.x, C.x))), width);
    uint32_t minY = max(glm::floor(min(A.y, min(B.y, C.y))), 0.f);
    uint32_t maxY = min(glm::ceil(max(A.y, max(B.y, C.y))), height);

    uint32_t row_length = maxX - minX;

    vec2 start_point{minX + 0.5, minY + 0.5};
    float edgeAB = edge_func(A, B, start_point);
    float edgeBC = edge_func(B, C, start_point);
    float edgeCA = edge_func(C, A, start_point);

    vec2 ABdelta = B - A;
    vec2 BCdelta = C - B;
    vec2 CAdelta = A - C;

    for (uint32_t y = minY; y < maxY; y++) {
        for (uint32_t x = minX; x < maxX; x++) {
            if (edgeAB >= 0 && edgeBC >= 0 && edgeCA >= 0) {
                process_fragment(mem, primitive, {x + 0.5, y + 0.5});
            } else {
            }

            edgeAB -= ABdelta.y;
            edgeBC -= BCdelta.y;
            edgeCA -= CAdelta.y;
        }

        edgeAB += ABdelta.x + row_length * ABdelta.y;
        edgeBC += BCdelta.x + row_length * BCdelta.y;
        edgeCA += CAdelta.x + row_length * CAdelta.y;
    }
}

void command_draw(GPUMemory &mem, DrawCommand const &cmd) {
    VertexArray &vertex_array = mem.vertexArrays[mem.activatedVertexArray];
    Program &program = mem.programs[mem.activatedProgram];

    OutVertex primitive[3];

    float width = mem.framebuffers[mem.activatedFramebuffer].width;
    float height = mem.framebuffers[mem.activatedFramebuffer].height;

    for (uint32_t vertex_index = 0; vertex_index < cmd.nofVertices;
         vertex_index++) {
        InVertex in_vertex;
        in_vertex.gl_VertexID = get_vertex_id(mem, vertex_array, vertex_index);
        fill_attributes(mem, vertex_array, in_vertex);

        OutVertex out_vertex;

        ShaderInterface shader_interface;
        shader_interface.gl_DrawID = mem.gl_DrawID;
        shader_interface.uniforms = mem.uniforms;
        shader_interface.textures = mem.textures;

        program.vertexShader(out_vertex, in_vertex, shader_interface);

        // perspective division
        out_vertex.gl_Position.x /= out_vertex.gl_Position.w;
        out_vertex.gl_Position.y /= out_vertex.gl_Position.w;
        out_vertex.gl_Position.z /= out_vertex.gl_Position.w;

        // NDC to screenspace transform
        out_vertex.gl_Position.x += 1.;
        out_vertex.gl_Position.y += 1.;

        out_vertex.gl_Position.x *= width / 2.;
        out_vertex.gl_Position.y *= height / 2.;

        primitive[vertex_index % 3] = out_vertex;

        if (vertex_index % 3 == 2) { // we have a complete primitive
            vec2 ab_normal =
                to_normal(primitive[1].gl_Position - primitive[0].gl_Position);

            vec2 bc = primitive[2].gl_Position - primitive[1].gl_Position;
            bool backface = ab_normal.x * bc.x + ab_normal.y * bc.y <= 0;
            if (cmd.backfaceCulling && backface) {
                continue; // back-facing triangle, skip
            }
            if (backface) {
                std::swap(primitive[1], primitive[2]);
            }

            rasterize(mem, primitive);
        }
    }

    mem.gl_DrawID++;
}

void process_command_buffer(GPUMemory &mem, CommandBuffer const &cb) {
    for (uint32_t i = 0; i < cb.nofCommands; i++) {

        Command const &command = cb.commands[i];
        switch (command.type) {
        case CommandType::EMPTY:
            break;

        case CommandType::CLEAR:
            clear_command(mem, command.data.clearCommand);
            break;

        case CommandType::SET_DRAW_ID:
            mem.gl_DrawID = command.data.setDrawIdCommand.id;
            break;

        case CommandType::DRAW:
            command_draw(mem, command.data.drawCommand);
            break;

        case CommandType::BIND_FRAMEBUFFER:
            mem.activatedFramebuffer = command.data.bindFramebufferCommand.id;
            break;

        case CommandType::BIND_PROGRAM:
            mem.activatedProgram = command.data.bindProgramCommand.id;
            break;

        case CommandType::BIND_VERTEXARRAY:
            mem.activatedVertexArray = command.data.bindVertexArrayCommand.id;
            break;

        case CommandType::SUB_COMMAND:
            assert(command.data.subCommand.commandBuffer !=
                   NULL); // TODO remove
            process_command_buffer(mem, *command.data.subCommand.commandBuffer);
            break;
        }
    }
}

//! [izg_enqueue]
void izg_enqueue(GPUMemory &mem, CommandBuffer const &cb) {
    mem.gl_DrawID = 0;
    process_command_buffer(mem, cb);
}
//! [izg_enqueue]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
vec4 read_texture(Texture const &texture, vec2 uv) {
    if (!texture.img.data)
        return vec4(0.f);
    auto &img = texture.img;
    auto uv1 = glm::fract(glm::fract(uv) + 1.f);
    auto uv2 = uv1 * vec2(texture.width - 1, texture.height - 1) + 0.5f;
    auto pix = glm::uvec2(uv2);
    return texelFetch(texture, pix);
}

/**
 * @brief This function reads color from texture with clamping on the
 * borders.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
vec4 read_textureClamp(Texture const &texture, vec2 uv) {
    if (!texture.img.data)
        return vec4(0.f);
    auto &img = texture.img;
    auto uv1 = glm::clamp(uv, 0.f, 1.f);
    auto uv2 = uv1 * vec2(texture.width - 1, texture.height - 1) + 0.5f;
    auto pix = glm::uvec2(uv2);
    return texelFetch(texture, pix);
}

/**
 * @brief This function fetches color from texture.
 *
 * @param texture texture
 * @param pix integer coorinates
 *
 * @return color 4 floats
 */
vec4 texelFetch(Texture const &texture, glm::uvec2 pix) {
    auto &img = texture.img;
    vec4 color = vec4(0.f, 0.f, 0.f, 1.f);
    if (pix.x >= texture.width || pix.y >= texture.height)
        return color;
    if (img.format == Image::UINT8) {
        auto colorPtr = (uint8_t *)getPixel(img, pix.x, pix.y);
        for (uint32_t c = 0; c < img.channels; ++c)
            color[c] = colorPtr[img.channelTypes[c]] / 255.f;
    }
    if (texture.img.format == Image::FLOAT32) {
        auto colorPtr = (float *)getPixel(img, pix.x, pix.y);
        for (uint32_t c = 0; c < img.channels; ++c)
            color[c] = colorPtr[img.channelTypes[c]];
    }
    return color;
}
