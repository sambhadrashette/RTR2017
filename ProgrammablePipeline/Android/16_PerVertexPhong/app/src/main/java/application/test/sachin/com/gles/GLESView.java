package application.test.sachin.com.gles;

import android.content.Context;
import android.opengl.GLES32;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.view.GestureDetector;
import android.view.MotionEvent;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Olaf on 07-01-2018.
 */

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, GestureDetector.OnGestureListener, GestureDetector.OnDoubleTapListener {

    private GestureDetector gestureDetector;
    private final Context context;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];

    private int model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;

    private int La_uniform;
    private int Ld_uniform;
    private int Ls_uniform;
    private int light_position_uniform;

    private int Ka_uniform;
    private int Kd_uniform;
    private int Ks_uniform;
    private int material_shininess_uniform;

    private int double_tap_uniform;

    private int numVertices;
    private int numElements;

    private float lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    private float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

    private float material_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    private float material_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float material_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float material_shininess = 50.0f;


    private float gAngle_cube = 0.0f;

    private boolean singleTap; // for animation
    private boolean doubleTap; // for lights

    private float perspectiveProjectionMatrix[] = new float[16];


    public GLESView(Context context) {
        super(context);
        this.context = context;
        this.setEGLContextClientVersion(3);
        setRenderer(this);
        this.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String glVersion = gl.glGetString(GL10.GL_VERSION);
        System.out.println("SMB GL_VERSION : " + glVersion);
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("SMB GLSL_VERSION : " + glslVersion);
        initialize(gl);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int eventAction = e.getAction();
        if (!gestureDetector.onTouchEvent(e)) {
            super.onTouchEvent(e);
        }
        return true;
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        draw();
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent motionEvent) {
        System.out.println("SMB : Single tap ");
        singleTap = !singleTap;
        return true;
    }

    @Override
    public boolean onDoubleTap(MotionEvent motionEvent) {
        System.out.println("SMB : onDoubleTap ");
        doubleTap = !doubleTap;
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent motionEvent) {
        return true;
    }

    @Override
    public boolean onDown(MotionEvent motionEvent) {
        return true;
    }

    @Override
    public void onShowPress(MotionEvent motionEvent) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
        return true;
    }

    @Override
    public boolean onScroll(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        System.out.println("SMB : scroll ");
        return true;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
        System.out.println("SMB : Long Press ");
    }

    @Override
    public boolean onFling(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        System.out.println("SMB : exiting app ");
        System.exit(0);
        return true;
    }

    private void initialize(GL10 gl) {
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertextShaderSourceCode = String.format
                (
                        "#version 320 es" +
                                "\n" +
                                "in vec4 vPosition;" +
                                "in vec3 vNormal;" +
                                "uniform mat4 u_model_matrix;" +
                                "uniform mat4 u_view_matrix;" +
                                "uniform mat4 u_projection_matrix;" +
                                "uniform int u_lighting_enabled;" +
                                "uniform vec3 u_La;" +
                                "uniform vec3 u_Ld;" +
                                "uniform vec3 u_Ls;" +
                                "uniform vec4 u_light_position;" +
                                "uniform vec3 u_Ka;" +
                                "uniform vec3 u_Kd;" +
                                "uniform vec3 u_Ks;" +
                                "uniform float u_material_shininess;" +
                                "out vec3 phong_ads_color;" +
                                "void main(void)" +
                                "{" +
                                "if(u_lighting_enabled == 1)" +
                                "{" +
                                "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;" +
                                "vec3 transformed_normals = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
                                "vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);" +
                                "float tn_dot_ld = max(dot(transformed_normals, light_direction), 0.0);" +
                                "vec3 ambient = u_La * u_Ka;" +
                                "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" +
                                "vec3 reflection_vector = reflect(-light_direction, transformed_normals);" +
                                "vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
                                "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" +
                                "phong_ads_color = ambient + diffuse + specular;" +
                                "}" +
                                "else" +
                                "{" +
                                "phong_ads_color = vec3(1.0, 1.0, 1.0);" +
                                "}" +
                                "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
                                "}"
                );

        GLES32.glShaderSource(vertexShaderObject, vertextShaderSourceCode);
        GLES32.glCompileShader(vertexShaderObject);
        int[] iShaderCompileStatus = new int[1];
        int[] iInfoLoglenght = new int[1];
        String szInfoLog = null;

        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        System.out.println("SMB Vertex shader compile status : " + iShaderCompileStatus[0]);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if (iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("SMB : Vertex shader compilation failure log : " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }


        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        final String fragmentShaderSourceCode = String.format
                (
                        "#version 320 es" +
                                "\n" +
                                "precision highp float;" +
                                "in vec3 phong_ads_color;" +
                                "out vec4 FragColor;" +
                                "void main(void)" +
                                "{" +
                                "FragColor = vec4(phong_ads_color, 1.0);" +
                                "}"
                );

        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
        GLES32.glCompileShader(fragmentShaderObject);
        iShaderCompileStatus = new int[1];
        iInfoLoglenght = new int[1];
        szInfoLog = null;

        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        System.out.println("SMB Fragment shader compile status : " + iShaderCompileStatus[0]);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if (iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("SMB : Fragment shader compilation failure log : " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //create shader program object
        shaderProgramObject = GLES32.glCreateProgram();

        //attach vertex shader object
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

        //attach fragment shader object
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        //pre-linking binding of shader program object with vertex shader attribute
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");

        //Link the two shaders together for shader program object
        GLES32.glLinkProgram(shaderProgramObject);

        int[] iShaderProgramLinkStatus = new int[1];

        //re-initialize
        iInfoLoglenght[0] = 0;
        szInfoLog = null;

        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);

        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if (iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("SMB : Shader program linking failure log : " + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //Get uniform location
        model_matrix_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        view_matrix_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projection_matrix_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        double_tap_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lighting_enabled");
        La_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_La");
        Ld_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ld");
        Ls_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ls");
        light_position_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");

        Ka_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ka");
        Kd_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Kd");
        Ks_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ks");
        material_shininess_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");

        //Creating sphere
        Sphere sphere = new Sphere();
        float sphere_vertices[] = new float[1146];
        float sphere_normals[] = new float[1146];
        float sphere_textures[] = new float[764];
        short sphere_elements[] = new short[2280];

        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // Sphere VAO
        GLES32.glGenVertexArrays(1, vao_sphere, 0);
        GLES32.glBindVertexArray(vao_sphere[0]);

        GLES32.glGenBuffers(1, vbo_sphere_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                sphere_vertices.length * 4,
                verticesBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        //For sphere normal
        GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);

        ByteBuffer byteNormalBuffers = ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteNormalBuffers.order(ByteOrder.nativeOrder());

        FloatBuffer normalBuffer = byteNormalBuffers.asFloatBuffer();
        normalBuffer.put(sphere_normals);
        normalBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                sphere_normals.length * 4,
                normalBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_NORMAL,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        // Sphere Elements

        GLES32.glGenBuffers(1, vbo_sphere_element, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

        ByteBuffer byteElementBuffers = ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteElementBuffers.order(ByteOrder.nativeOrder());

        ShortBuffer elementBuffer = byteElementBuffers.asShortBuffer();
        elementBuffer.put(sphere_elements);
        elementBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                sphere_elements.length * 2,
                elementBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        GLES32.glEnable(GLES32.GL_CULL_FACE);

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void resize(int width, int height) {
        GLES32.glViewport(0, 0, width, height);

        if (width <= height) {
            Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float) height / (float) width, 0.1f, 100.0f);
        } else {
            Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float) width / (float) height, 0.1f, 100.0f);
        }
    }

    private void draw() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject);
        if (doubleTap) {
            GLES32.glUniform1i(double_tap_uniform, 1);

            GLES32.glUniform3fv(La_uniform, 1, lightAmbient, 0);
            GLES32.glUniform3fv(Ld_uniform, 1, lightDiffuse, 0);
            GLES32.glUniform3fv(Ls_uniform, 1, lightSpecular, 0);
            GLES32.glUniform4fv(light_position_uniform, 1, lightPosition, 0);

            GLES32.glUniform3fv(Ka_uniform, 1, material_ambient, 0);
            GLES32.glUniform3fv(Kd_uniform, 1, material_diffuse, 0);
            GLES32.glUniform3fv(Ks_uniform, 1, material_specular, 0);
            GLES32.glUniform1f(material_shininess_uniform, material_shininess);
        } else {
            GLES32.glUniform1i(double_tap_uniform, 0);
        }

        //OpenGL-ES Drawing
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];

        //square
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -2.0f);

        GLES32.glUniformMatrix4fv(model_matrix_uniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(view_matrix_uniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(projection_matrix_uniform, 1, false, perspectiveProjectionMatrix, 0);

        GLES32.glBindVertexArray(vao_sphere[0]);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        requestRender();

    }

    private void uninitialize() {

        if (vao_sphere[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0] = 0;
        }

        if (vbo_sphere_position[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0] = 0;
        }

        if (vbo_sphere_normal[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0] = 0;
        }

        if (vbo_sphere_element[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0] = 0;
        }

        if (shaderProgramObject != 0) {
            if (vertexShaderObject != 0) {
                // detach vertex shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                // delete vertex shader object
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }

            if (fragmentShaderObject != 0) {
                // detach fragment  shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                // delete fragment shader object
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }

        // delete shader program object
        if (shaderProgramObject != 0) {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }

}
