#include <karbon/drive.h>
#include <karbon/app.h>
#include <karbon/math.h>
#include <GL/gl3w.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>


#define CHUNK_IMPL
#include <chunk/chunk.h>
#include <chunk/types.h>


#define GL_ERR(msg)                                           \
do {                                                          \
        GLuint err = glGetError();                            \
        if(err) {                                             \
                assert(sizeof(msg) < 256 && "GL msg too big");\
                char buf[512] = {0};                          \
                sprintf(buf, "GL ERR: %d - %s\n", err, msg);  \
                kd_log(KD_LOG_ERROR, buf);                    \
                assert(!msg);                                 \
        }                                                     \
} while(0);                                                   \


/* ----------------------------------------------------------- Application -- */


struct ed_ctx {
        GLuint vao, vbo, pro;

        struct ch_transform transforms[3];
} ed_ctx;


void
setup()
{
        kd_result ok = KD_RESULT_OK;
        ok = kd_log(KD_LOG_INFO, "Editor Startup");
        assert(ok == KD_RESULT_OK);

        memset(&ed_ctx, 0, sizeof(ed_ctx));
                
        ok = kd_gl_make_current();
        assert(ok == KD_RESULT_OK && "Failed to make current");

        if (gl3wInit()) {
                assert(!"FAILED TO INIT");
        }

        /* print out version */
        char buf[512] = {0};
        sprintf(
                buf,
                "OpenGL %s, GLSL %s, OGL %s",
                glGetString(GL_VERSION),
                glGetString(GL_SHADING_LANGUAGE_VERSION),
                glGetString(GL_VERSION));

        ok = kd_log(KD_LOG_INFO, buf);
        assert(ok == KD_RESULT_OK && "Failed to log");

        if (!gl3wIsSupported(3, 2)) {
                assert(!"OGL 3 2 not supported");
        }

        if(glPushDebugGroup) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Runtime Setup");
        }

        /* vao */
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        ed_ctx.vao = vao;
        
        if(glObjectLabel) {
                glObjectLabel(GL_VERTEX_ARRAY, vao, -1, "Cube::VAO");
        }
        
        /* vbo */
        /* pos v2 - color v3 - tc v2 */
        GLfloat verts[] = {
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

                -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

                -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

                -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        };

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        ed_ctx.vbo = vbo;

        if(glObjectLabel) {
                glObjectLabel(GL_BUFFER, vbo, -1, "Cube::VBO::f3f3f2");
        }

        /* shd */
        const GLchar *vs_src = ""
                "#version 130\n"
                "in vec3 position;\n"
                "in vec3 color;\n"
                "in vec2 texcoord;\n"
                "out vec3 Color;\n"
                "out vec2 Texcoord;\n"
                "uniform vec3 overrideColor;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 proj;\n"
                "void main() {\n"
                        "Texcoord = texcoord;\n"
                        "Color = overrideColor * color;\n"
                        "gl_Position = proj * view * model * vec4(position, 1.0);\n"
                "}";

        GLuint vs_shd = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs_shd, 1, &vs_src, NULL);
        glCompileShader(vs_shd);
        GLint vs_status;
        glGetShaderiv(vs_shd, GL_COMPILE_STATUS, &vs_status);
        if(vs_status == GL_FALSE) {
                char buffer[1024];
                glGetShaderInfoLog(vs_shd, sizeof(buffer), NULL, buffer);
                kd_log(KD_LOG_FATAL, buffer);
                assert(!"Failed to build vs shd");
        }

        const GLchar *fs_src = ""
                "#version 130\n"
                "in vec2 Texcoord;\n"
                "in vec3 Color;\n"
                "uniform sampler2D texKitten;\n"
                "uniform sampler2D texPuppy;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                        "outColor = vec4(Color, 1.0) * mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);\n"
                "}\n";

        GLuint fs_shd = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs_shd, 1, &fs_src, NULL);
        glCompileShader(fs_shd);
        GLint fs_status;
        glGetShaderiv(fs_shd, GL_COMPILE_STATUS, &fs_status);
        if(fs_status == GL_FALSE) {
                char buffer[1024];
                glGetShaderInfoLog(vs_shd, sizeof(buffer), NULL, buffer);
                kd_log(KD_LOG_FATAL, buffer);
                assert(!"Failed to build fs shd");
        }

        GLuint pro = glCreateProgram();
        glAttachShader(pro, vs_shd);
        glAttachShader(pro, fs_shd);
        glBindFragDataLocation(pro, 0, "outColor");
        glLinkProgram(pro);
        GLint pro_status;
        glGetProgramiv(pro, GL_LINK_STATUS, &pro_status);
        if(pro_status == GL_FALSE) {
                char buffer[1024];
                glGetProgramInfoLog(pro, sizeof(buffer), NULL, buffer);
                kd_log(KD_LOG_FATAL, buffer);
                assert(!"Failed to link");
        }

        glDeleteShader(vs_shd);
        glDeleteShader(fs_shd);

        ed_ctx.pro = pro;

        if(glObjectLabel) {
                glObjectLabel(GL_PROGRAM, pro, -1, "Cube::Fill");
        }

        GL_ERR("Setup")

        if(glPopDebugGroup) {
                glPopDebugGroup();
        }

        /* level edit data test */
        ed_ctx.transforms[0].x = 0.f;
        ed_ctx.transforms[0].y = 0.f;
        ed_ctx.transforms[0].z = 0.f;

        ed_ctx.transforms[1].x = 1.f;
        ed_ctx.transforms[1].y = 1.f;
        ed_ctx.transforms[1].z = 0.f;

        ed_ctx.transforms[2].x = -1.f;
        ed_ctx.transforms[2].y = -1.f;
        ed_ctx.transforms[2].z = 0.f;

        chunk_export export_tr;
        export_tr.chunk_idx = 1;
        export_tr.data = (void*)ed_ctx.transforms;
        export_tr.bytes = sizeof(ed_ctx.transforms);

        chunk_write_out(&export_tr, 1, CHUNK_VERSION, "foo.dat");
}


void
shutdown()
{
        glDeleteProgram(ed_ctx.pro);
        glDeleteBuffers(1, &ed_ctx.vbo);
        glDeleteVertexArrays(1, &ed_ctx.vao);

        /* chunk test */
        
}


void
render() {
        struct kd_window_desc win_desc;
        win_desc.type_id = KD_STRUCT_WINDOW_DESC;
        kd_window_get(&win_desc);

        GL_ERR("New frame");

        /* render */
        if(glPushDebugGroup) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Cube Render");
        }
       
        /* setup */
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        glBindVertexArray(ed_ctx.vao);
        GL_ERR("Setup - VAO")
        glUseProgram(ed_ctx.pro);
        GL_ERR("Setup - use program")
        glBindBuffer(GL_ARRAY_BUFFER, ed_ctx.vbo);
        GL_ERR("Setup - vbo")

        /* clear */
        glClearColor(0.2, 0.15, 0.15, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        GL_ERR("Clear")

        /* draw cube */
        GLint color = glGetUniformLocation(ed_ctx.pro, "overrideColor");
        glUniform3f(color, 1.0f, 1.0f, 1.0f);

        /* input */
        GLsizei jmp = 8 * sizeof(GLfloat);
        void *off = 0;

        GLint pos_a = glGetAttribLocation(ed_ctx.pro, "position");
        if(pos_a > -1) {
                glEnableVertexAttribArray(pos_a);
                glVertexAttribPointer(pos_a, 3, GL_FLOAT, GL_FALSE, jmp, off);
                GL_ERR("Input - POS")
        }
        off = (void*)(3 * sizeof(GLfloat));

        GLint col_a = glGetAttribLocation(ed_ctx.pro, "color");
        if(col_a > -1) {
                glEnableVertexAttribArray(col_a);
                glVertexAttribPointer(col_a, 3, GL_FLOAT, GL_FALSE, jmp, off);
                GL_ERR("Input - COL3")
        }
        off = (void*)(6 * sizeof(GLfloat));

        GLint tex_a = glGetAttribLocation(ed_ctx.pro, "texcoord");
        if(tex_a > -1) {
                glEnableVertexAttribArray(tex_a);
                glVertexAttribPointer(tex_a, 2, GL_FLOAT, GL_FALSE, jmp, off);
                GL_ERR("Input - TC")
        }
        GL_ERR("Vertex Attrs")

        int i;
        for(i = 0; i < 3; ++i) {
                /* mats */
                float model[16];
                kdm_mat4_id(model);
                model[12] = ed_ctx.transforms[i].x;
                model[13] = ed_ctx.transforms[i].y;
                model[14] = ed_ctx.transforms[i].z;

                GLint model_i = glGetUniformLocation(ed_ctx.pro, "model");
                glUniformMatrix4fv(model_i, 1, GL_FALSE, model);
        
                float view[16];
        
                float x = sinf(0.1f) * 4.f;
                float y = cosf(0.1f) * 4.f;
                float z = 2.f;
        
                float eye[3];
                eye[0] = x;
                eye[1] = y;
                eye[2] = z;

                float at[] = {0.f, 0.f, 0.f};
                float up[] = {0.f, 0.f, 1.f};
                kdm_mat4_lookat(eye, at, up, view);
                GLint view_i = glGetUniformLocation(ed_ctx.pro, "view");
                glUniformMatrix4fv(view_i, 1, GL_FALSE, view);

                float proj[16];
                float ratio = (float)(win_desc.width) / (float)win_desc.height;
                float fov = KDM_TAU * 0.125f;
                kdm_mat4_perspective_projection(ratio, 0.1f, 100.f, fov, proj);
                GLint proj_i = glGetUniformLocation(ed_ctx.pro, "proj");
                glUniformMatrix4fv(proj_i, 1, GL_FALSE, proj);
                GL_ERR("Update Mats")

                glDrawArrays(GL_TRIANGLES, 0, 36);
                GL_ERR("Draw")
        }

        if(glPopDebugGroup) {
                glPopDebugGroup();
        }

        GL_ERR("End Frame");
}


void
think()
{
        kd_result ok = KD_RESULT_OK;

        /* check events */
        uint64_t events = 0;
        ok = kd_events_get(&events);
        assert(ok == KD_RESULT_OK && "Failed to get events");

        #ifndef NDEBUG
        /* clear gl errors */
        while(glGetError()){}
        #endif

        /* screen was resized */
        if(events & KD_EVENT_VIEWPORT_RESIZE) {
                struct kd_window_desc win_desc;
                win_desc.type_id = KD_STRUCT_WINDOW_DESC;
                kd_window_get(&win_desc);

                glViewport(0, 0, win_desc.width, win_desc.height);
        }

        /* a key was pushed */
        if(events & KD_EVENT_INPUT_KB) {
                struct kd_keyboard_desc kb_desc;
                ok = kd_input_get_keyboards(&kb_desc); 
                assert(ok == KD_RESULT_OK && "Failed to get kb desc");
                assert(kb_desc.kb_count && "No kb found");

                if(kb_desc.kb_state[0][KD_KB_ESC] & KD_KEY_UP_EVENT) {
                        ok = kd_log(KD_LOG_INFO, "Quit");
                        assert(ok == KD_RESULT_OK && "Failed to log");
                        kd_ctx_close();
                }
                else if(kb_desc.kb_state[0][KD_KB_ANY] & KD_KEY_UP_EVENT) {

                        int app_idx, app_cnt;
                        ok = kd_ctx_application_index_get(&app_idx, &app_cnt);
                        assert(ok == KD_RESULT_OK && "Failed to get index");

                        int next_idx = (app_idx + 1) % app_cnt;

                        char buf[512] = {0};
                        sprintf(
                                buf,
                                "Curr IDX(%d) of (%d), next IDX(%d)\n",
                                app_idx,
                                app_cnt,
                                next_idx);

                        ok = kd_log(KD_LOG_INFO, buf);
                        assert(ok == KD_RESULT_OK && "Failed to log");

                        ok = kd_ctx_application_index_set(next_idx);
                        assert(ok == KD_RESULT_OK && "Failed to set index");
                }
        }
        
        GL_ERR("New frame");

        if(glPushDebugGroup) {
                glPushDebugGroup(
                        GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_PUSH_GROUP,
                        -1,
                        "Render");
        }

        render();

        if(glPopDebugGroup) {
                glPopDebugGroup();
        }

        /* reset state */
        GL_ERR("End Frame");
}


/* ----------------------------------------------- Application Description -- */


KD_APP_NAME("Chunk Editor")
KD_APP_DESC("Serializes data for a chunk")
KD_APP_GRAPHICS_API("OpenGL")
KD_APP_STARTUP_FN(setup)
KD_APP_TICK_FN(think)
KD_APP_SHUTDOWN_FN(shutdown)
