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

    private int[] vao_cube = new int[1];

    private int[] vbo_position = new int[1];
    private int[] vbo_normal = new int[1];
    private int  modelViewMatrixUniform, projectionMatrixUniform;
    private int  ldUniform, kdUniform, lightPositionUniform;

    private int doubleTapUniform;

    private float gAngle_cube = 0.0f;

    private boolean singleTap; // for animation
    private boolean doubleTap; // for lights

    private float perspectiveProjectionMatrix[] = new float[16];


    public GLESView (Context context) {
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
        System.out.println("SMB GL_VERSION : "+ glVersion);
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("SMB GLSL_VERSION : "+ glslVersion);
        initialize(gl);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int eventAction = e.getAction();
        if(!gestureDetector.onTouchEvent(e)) {
            super.onTouchEvent(e);
        }
        return true;
    }

    @Override
    public void onSurfaceChanged (GL10 unused, int width, int height) {
        resize (width, height);
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
                        "uniform mat4 u_model_view_matrix;"+
                        "uniform mat4 u_projection_matrix;"+
                        "uniform mediump int  u_double_tap;"+
                        "uniform vec3 u_Ld;"+
                        "uniform vec3 u_Kd;"+
                        "uniform vec4 u_light_position;"+
                        "out vec3 diffuse_light;"+
                        "void main(void)" +
                        "{" +
                            "if(u_double_tap == 1)" +
                            "{" +
                            "vec4 eye_co_ordinates = u_model_view_matrix * vPosition;" +
                            "vec3 tnorm = normalize(mat3(u_model_view_matrix) * vNormal );" +
                            "vec3 s = normalize(vec3(u_light_position - eye_co_ordinates));" +
                            "diffuse_light = u_Ld * u_Kd * max(dot(s, tnorm), 0.0);" +
                            "}" +
                            "gl_Position = u_projection_matrix*u_model_view_matrix*vPosition;"+
                        "}"
                );

        GLES32.glShaderSource(vertexShaderObject, vertextShaderSourceCode);
        GLES32.glCompileShader(vertexShaderObject);
        int[] iShaderCompileStatus = new int[1];
        int[] iInfoLoglenght = new int[1];
        String szInfoLog = null;

        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        System.out.println("SMB Vertex shader compile status : "+ iShaderCompileStatus[0]);
        if(iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if(iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("SMB : Vertex shader compilation failure log : "+szInfoLog);
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
                        "in vec3 diffuse_light;" +
                        "out vec4 FragColor;" +
                        "uniform int u_double_tap;" +
                        "void main(void)" +
                        "{" +
                        "vec4 color;" +
                        "if(u_double_tap == 1)" +
                        "{" +
                        "color = vec4(diffuse_light, 1.0);" +
                        "}" +
                        "else" +
                        "{" +
                        "color = vec4(1.0, 1.0, 1.0, 1.0);" +
                        "}" +
                        "FragColor = color;" +
                        "}"
                );

        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
        GLES32.glCompileShader(fragmentShaderObject);
        iShaderCompileStatus = new int[1];
        iInfoLoglenght = new int[1];
        szInfoLog = null;

        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        System.out.println("SMB Fragment shader compile status : "+ iShaderCompileStatus[0]);
        if(iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if(iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("SMB : Fragment shader compilation failure log : "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //create shader program object
        shaderProgramObject = GLES32.glCreateProgram();

        //attach vertex shader object
        GLES32.glAttachShader(shaderProgramObject,vertexShaderObject);

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

        if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLoglenght, 0);
            if(iInfoLoglenght[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("SMB : Shader program linking failure log : "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        //Get uniform location
        modelViewMatrixUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_model_view_matrix");
        projectionMatrixUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
        doubleTapUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_double_tap");
        ldUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_Ld");
        kdUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_Kd");
        lightPositionUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");


        final float cubeVertices[] =
                {
                        // top surface
                        1.0f, 1.0f,-1.0f,
                        -1.0f, 1.0f,-1.0f,
                        -1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f,

                        // bottom surface
                        1.0f,-1.0f, 1.0f,
                        -1.0f,-1.0f, 1.0f,
                        -1.0f,-1.0f,-1.0f,
                        1.0f,-1.0f,-1.0f,

                        // front surface
                        1.0f, 1.0f, 1.0f,
                        -1.0f, 1.0f, 1.0f,
                        -1.0f,-1.0f, 1.0f,
                        1.0f,-1.0f, 1.0f,

                        // back surface
                        1.0f,-1.0f,-1.0f,
                        -1.0f,-1.0f,-1.0f,
                        -1.0f, 1.0f,-1.0f,
                        1.0f, 1.0f,-1.0f,

                        // left surface
                        -1.0f, 1.0f, 1.0f,
                        -1.0f, 1.0f,-1.0f,
                        -1.0f,-1.0f,-1.0f,
                        -1.0f,-1.0f, 1.0f,

                        // right surface
                        1.0f, 1.0f,-1.0f,
                        1.0f, 1.0f, 1.0f,
                        1.0f,-1.0f, 1.0f,
                        1.0f,-1.0f,-1.0f
                };



        final float cubeNormal[] =
                {
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,

                        0.0f, -1.0f, 0.0f,
                        0.0f, -1.0f, 0.0f,
                        0.0f, -1.0f, 0.0f,
                        0.0f, -1.0f, 0.0f,

                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f,

                        0.0f, 0.0f, -1.0f,
                        0.0f, 0.0f, -1.0f,
                        0.0f, 0.0f, -1.0f,
                        0.0f, 0.0f, -1.0f,

                        -1.0f, 0.0f, 0.0f,
                        -1.0f, 0.0f, 0.0f,
                        -1.0f, 0.0f, 0.0f,
                        -1.0f, 0.0f, 0.0f,

                        1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f
                };

        // Square VAO
        GLES32.glGenVertexArrays(1, vao_cube, 0);
        GLES32.glBindVertexArray(vao_cube[0]);

        GLES32.glGenBuffers(1, vbo_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

        ByteBuffer byteBufferCube = ByteBuffer.allocateDirect(cubeVertices.length * 4);
        byteBufferCube.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBufferCube = byteBufferCube.asFloatBuffer();
        verticesBufferCube.put(cubeVertices);
        verticesBufferCube.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                cubeVertices.length * 4,
                verticesBufferCube,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        //For normal
        GLES32.glGenBuffers(1, vbo_normal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal[0]);

        ByteBuffer byteNormalBuffers = ByteBuffer.allocateDirect(cubeNormal.length * 4);
        byteNormalBuffers.order(ByteOrder.nativeOrder());

        FloatBuffer normalBufferCube = byteNormalBuffers.asFloatBuffer();
        normalBufferCube.put(cubeNormal);
        normalBufferCube.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                cubeNormal.length * 4,
                normalBufferCube,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_NORMAL,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        GLES32.glBindVertexArray(0);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        GLES32.glEnable(GLES32.GL_CULL_FACE);

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }

    private void resize (int width, int height) {
        GLES32.glViewport(0, 0, width, height);

        if(width <= height ) {
            Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)height/(float)width, 0.1f, 100.0f);
        } else {
            Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width/(float)height, 0.1f, 100.0f);
        }
    }

    private void draw() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject);
        if(doubleTap)
        {
            GLES32.glUniform1i(doubleTapUniform, 1);

            // setting light properties
            GLES32.glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f); // diffuse intensity of light
            GLES32.glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f); // diffuse reflectivity of material
            float[] lightPosition = {0.0f, 0.0f, 2.0f, 1.0f};
            GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition,0); // light position
        }
        else
        {
            GLES32.glUniform1i(doubleTapUniform, 0);
        }

        //OpenGL-ES Drawing
        float modelViewMatrix[] = new float[16];
        float modelviewProjectionmatrix[] = new float[16];
        float rotationMatrix[] = new float[16];

        //square
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelviewProjectionmatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.translateM(modelViewMatrix,0, 0.0f, 0.0f, -5.0f);
        Matrix.rotateM(rotationMatrix, 0, gAngle_cube, 1.0f, 1.0f, 1.0f);

        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
        GLES32.glUniformMatrix4fv(modelViewMatrixUniform,1,false,modelViewMatrix,0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform,1,false,perspectiveProjectionMatrix,0);

        GLES32.glBindVertexArray(vao_cube[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        if(singleTap)
            update();

        requestRender();

    }

    private void uninitialize() {

        if (vao_cube[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao_cube, 0);
            vao_cube[0] = 0;
        }

        if(vbo_position[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_position, 0);
            vbo_position[0] = 0;
        }

        if(vbo_normal[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_normal, 0);
            vbo_normal[0] = 0;
        }

        if(shaderProgramObject != 0)
        {
            if(vertexShaderObject != 0)
            {
                // detach vertex shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                // delete vertex shader object
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }

            if(fragmentShaderObject != 0)
            {
                // detach fragment  shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                // delete fragment shader object
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }

        // delete shader program object
        if(shaderProgramObject != 0)
        {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }



    }

    private void update()
    {
        gAngle_cube = gAngle_cube + 1.0f;
        if (gAngle_cube >= 360.0f)
            gAngle_cube = 0.1f;
    }

}
