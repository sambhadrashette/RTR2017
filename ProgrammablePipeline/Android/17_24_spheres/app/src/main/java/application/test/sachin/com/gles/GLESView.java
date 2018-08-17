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

    private float light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    private float light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    private float light_specular[] = { 1.0f, 1.0f, 1.0f ,1.0f };
    private float light_position[] = { 100.0f, 100.0f, 100.0f, 1.0f };

    private float material_ambient[][] = { { 0.0215f, 0.1745f, 0.0215f, 1.0f },{ 0.135f, 0.2225f, 0.1575f, 1.0f },{ 0.05375f, 0.05f, 0.06625f, 1.0f },{ 0.25f, 0.20725f, 0.20725f, 1.0f },{ 0.1745f, 0.01175f, 0.01175f, 1.0f },{ 0.1f, 0.18725f, 0.1745f, 1.0f },{ 0.329412f, 0.223529f, 0.027451f, 1.0f },{ 0.2125f, 0.1275f, 0.054f, 1.0f },{ 0.25f, 0.25f, 0.25f, 1.0f },{ 0.19125f, 0.0735f, 0.0225f, 1.0f },{ 0.24725f, 0.1995f, 0.0745f, 1.0f },{ 0.19225f, 0.19225f, 0.19225f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.1f, 0.06f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f, 1.0f } ,{ 0.0f, 0.0f, 0.0f, 1.0f },{ 0.02f, 0.02f, 0.02f, 1.0f },{ 0.0f, 0.05f, 0.05f, 1.0f },{ 0.0f, 0.05f, 0.0f, 1.0f },{ 0.05f, 0.0f, 0.0f, 1.0f },{ 0.05f, 0.05f, 0.05f, 1.0f },{ 0.05f, 0.05f, 0.0f, 1.0f } };
    private float material_diffuse[][] = { { 0.07568f, 0.61424f, 0.07568f, 1.0f },{ 0.54f, 0.89f, 0.63f, 1.0f },{ 0.18275f, 0.17f, 0.22525f, 1.0f },{ 1.0f, 0.829f, 0.829f, 1.0f },{ 0.61424f, 0.04136f, 0.04136f, 1.0f },{ 0.396f, 0.74151f, 0.69102f, 1.0f },{ 0.780392f, 0.568627f, 0.113725f, 1.0f },{ 0.714f, 0.4284f, 0.18144f, 1.0f },{ 0.4f, 0.4f, 0.4f, 1.0f },{ 0.7038f, 0.27048f, 0.0828f, 1.0f },{ 0.75164f, 0.60648f, 0.22648f, 1.0f },{ 0.50754f, 0.50754f, 0.50754f, 1.0f },{ 0.01f, 0.01f, 0.01f, 1.0f },{ 0.0f, 0.50980392f, 0.50980392f, 1.0f },{ 0.1f, 0.35f, 0.1f, 1.0f },{ 0.5f, 0.0f, 0.0f, 1.0f },{ 0.55f, 0.55f, 0.55f, 1.0f },{ 0.5f, 0.5f, 0.0f, 1.0f },{ 0.01f, 0.01f, 0.01f, 1.0f },{ 0.4f, 0.5f, 0.5f, 1.0f },{ 0.4f, 0.5f, 0.4f, 1.0f },{ 0.5f, 0.4f, 0.4f, 1.0f },{ 0.5f, 0.5f, 0.5f, 1.0f },{ 0.5f, 0.5f, 0.4f, 1.0f } };
    private float material_specular[][] = { { 0.633f, 0.727811f, 0.633f, 1.0f },{ 0.316228f, 0.316228f, 0.316228f, 1.0f },{ 0.332741f, 0.328634f, 0.346435f, 1.0f },{ 0.296648f, 0.296648f, 0.296648f, 1.0f },{ 0.727811f, 0.626959f, 0.626959f, 1.0f },{ 0.297254f, 0.30829f, 0.306678f, 1.0f },{ 0.992157f, 0.941176f, 0.807843f, 1.0f },{ 0.393548f, 0.271906f, 0.166721f, 1.0f },{ 0.774597f, 0.774597f, 0.774597f, 1.0f },{ 0.256777f, 0.137622f, 0.086014f, 1.0f },{ 0.628281f, 0.555802f, 0.366065f, 1.0f },{ 0.508273f, 0.508273f, 0.508273f, 1.0f },{ 0.50f, 0.50f, 0.50f, 1.0f },{ 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },{ 0.45f, 0.55f, 0.45f, 1.0f },{ 0.7f, 0.6f, 0.6f, 1.0f },{ 0.70f, 0.70f, 0.70f, 1.0f },{ 0.60f, 0.60f, 0.50f, 1.0f },{ 0.4f, 0.4f, 0.4f, 1.0f },{ 0.04f, 0.7f, 0.7f, 1.0f },{ 0.04f, 0.7f, 0.04f, 1.0f },{ 0.7f, 0.04f, 0.04f, 1.0f },{ 0.7f, 0.7f, 0.7f, 1.0f },{ 0.7f, 0.7f, 0.04f, 1.0f } };
    private float material_shininess[] = { 0.6f * 128, 0.1f * 128, 0.3f * 128, 0.088f * 128, 0.6f * 128, 0.1f * 128, 0.21794872f * 128, 0.2f * 128, 0.6f * 128, 0.1f * 128, 0.4f * 128, 0.4f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.25f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128, 0.078125f * 128 };


    private float gAngle = 0.0f;
    private float gRedious = 50.0f;
    private int singleTap = 0; // for animation
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

        singleTap++;
        if (singleTap > 3)
            singleTap = 0;
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
                        "#version 320 es"+
                        "\n"+
                        "in vec4 vPosition;"+
                        "in vec3 vNormal;"+
                        "uniform mat4 u_model_matrix;"+
                        "uniform mat4 u_view_matrix;"+
                        "uniform mat4 u_projection_matrix;"+
                        "uniform mediump int u_lighting_enabled;"+
                        "uniform vec4 u_light_position;"+
                        "out vec3 transformed_normals;"+
                        "out vec3 light_direction;"+
                        "out vec3 viewer_vector;"+
                        "void main(void)"+
                        "{"	+
                        "if(u_lighting_enabled == 1)"+
                        "{"+
                        "vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition;"+
                        "transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;"+
                        "light_direction = vec3(u_light_position) - eye_coordinates.xyz;"+
                        "viewer_vector = -eye_coordinates.xyz;"	+
                        "}"	+
                        "gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;"	+
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
                        "#version 320 es"+
                        "\n"+
                        "precision highp float;"+
                        "in vec3 transformed_normals;"+
                        "in vec3 light_direction;"+
                        "in vec3 viewer_vector;"+
                        "out vec4 FragColor;"	+
                        "uniform vec3 u_La;"+
                        "uniform vec3 u_Ld;"+
                        "uniform vec3 u_Ls;"+
                        "uniform vec3 u_Ka;"+
                        "uniform vec3 u_Kd;"+
                        "uniform vec3 u_Ks;"	+
                        "uniform float u_material_shininess;"+
                        "uniform int u_lighting_enabled;"+
                        "void main(void)"+
                        "{"	+
                        "vec3 phong_ads_color;"+
                        "if(u_lighting_enabled == 1)"+
                        "{"+
                        "vec3 normalized_transformed_normals = normalize(transformed_normals);"	+
                        "vec3 normalize_light_direction = normalize(light_direction);"	+
                        "vec3 normalized_viewer_vector = normalize(viewer_vector);"+
                        "vec3 ambient = u_La * u_Ka;"+
                        "float tn_dot_ld = max(dot(normalized_transformed_normals, normalize_light_direction), 0.0);"+
                        "vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;"+
                        "vec3 reflection_vector = reflect(-normalize_light_direction, normalized_transformed_normals);"	+
                        "vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);"+
                        "phong_ads_color = ambient + diffuse + specular;"+
                        "}"	+
                        "else"+
                        "{"	+
                        "phong_ads_color = vec3(1.0, 1.0, 1.0);"	+
                        "}"	+
                        "FragColor = vec4(phong_ads_color, 1.0);"+
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

        GLES32.glClearColor(0.25f, 0.25f, 0.25f, 0.25f);

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

            GLES32.glUniform3fv(La_uniform, 1, light_ambient, 0);
            GLES32.glUniform3fv(Ld_uniform, 1, light_diffuse, 0);
            GLES32.glUniform3fv(Ls_uniform, 1, light_specular, 0);
            GLES32.glUniform4fv(light_position_uniform, 1, light_position, 0);

            /*GLES32.glUniform3fv(Ka_uniform, 1, material_ambient, 0);
            GLES32.glUniform3fv(Kd_uniform, 1, material_diffuse, 0);
            GLES32.glUniform3fv(Ks_uniform, 1, material_specular, 0);
            GLES32.glUniform1f(material_shininess_uniform, material_shininess);*/
        } else {
            GLES32.glUniform1i(double_tap_uniform, 0);
        }

        //OpenGL-ES Drawing
        float modelMatrix[] = new float[16];
        float viewMatrix[] = new float[16];

        int row =0, col =0 , count = 0;
        for (int index = 0; index < 24; index++) {
            //square
            Matrix.setIdentityM(modelMatrix, 0);
            Matrix.setIdentityM(viewMatrix, 0);

            Matrix.translateM(modelMatrix, 0, -5.0f + col*2, 3.0f - row*2, -10.0f);

            GLES32.glUniformMatrix4fv(model_matrix_uniform, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(view_matrix_uniform, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(projection_matrix_uniform, 1, false, perspectiveProjectionMatrix, 0);

            GLES32.glBindVertexArray(vao_sphere[0]);
            GLES32.glUniform3fv(Ka_uniform, 1, material_ambient[index], 0);
            GLES32.glUniform3fv(Kd_uniform, 1, material_diffuse[index], 0);
            GLES32.glUniform3fv(Ks_uniform, 1, material_specular[index], 0);
            GLES32.glUniform1f(material_shininess_uniform, material_shininess[index]);

            GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
            GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

            GLES32.glBindVertexArray(0);

            if (col < 5) {
                col++;
            }
            else {
                col = 0;
                row++;
            }
        }

        GLES32.glUseProgram(0);
        update();
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

    void update() {
        gAngle = gAngle + 0.02f;
        if (gAngle >= 360.0f) {
            gAngle = 0.0f;
        }

        if (singleTap == 1) {
            light_position[0] = 0.0f;
            light_position[1] = (float) (gRedious * Math.cos(gAngle));
            light_position[2] = (float) (gRedious * Math.sin(gAngle));
        } else if (singleTap == 2) {
            light_position[0] = (float) (gRedious * Math.cos(gAngle));
            light_position[1] = 0.0f;
            light_position[2] = (float) (gRedious * Math.sin(gAngle));
        } else if (singleTap == 3) {
            light_position[0] = (float) (gRedious * Math.cos(gAngle));
            light_position[1] = (float) (gRedious * Math.sin(gAngle));
            light_position[2] = 0.0f;
        }
    }
}
