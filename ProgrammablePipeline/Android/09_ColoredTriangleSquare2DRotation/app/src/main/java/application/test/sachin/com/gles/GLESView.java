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

    private int[] vao_triangle = new int[1];
    private int[] vao_square = new int[1];
    private int[] vbo_position = new int[1];
    private int[] vbo_color = new int[1];
    private int mvpUniform;

    private float gAngle_triangle = 0.0f;
    private float gAngle_square = 0.0f;

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
        return true;
    }

    @Override
    public boolean onDoubleTap(MotionEvent motionEvent) {
        System.out.println("SMB : onDoubleTap ");
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
                        "in vec4 vColor;" +
                        "uniform mat4 u_mvp_matrix;" +
                        "out vec4 out_color;" +
                        "void main(void)" +
                        "{" +
                        "gl_Position = u_mvp_matrix * vPosition;" +
                         "out_color = vColor;" +
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
                unintialize();
                System.exit(0);
            }
        }


        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        final String fragmentShaderSourceCode = String.format
                (
                        "#version 320 es" +
                        "\n" +
                        "precision highp float;" +
                                "in vec4 out_color;" +
                        "out vec4 FragColor;" +
                        "void main(void)" +
                        "{" +
                        "FragColor = out_color;" +
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
                unintialize();
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.VDG_ATTRIBUTE_COLOR, "vColor");

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
                unintialize();
                System.exit(0);
            }
        }

        //Get uniform location
        mvpUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");

        //******************* Vertices **************************
        final float triangleVertices[] =
                {
                        0.0f, 1.0f, 0.0f,
                        -1.0f, -1.0f, 0.0f,
                        1.0f, -1.0f, 0.0f
                };

        //Square Vertices
        final float squareVertices[] = new float[]
                {
                        -1.0f, 1.0f, 0.0f,
                        -1.0f, -1.0f, 0.0f,
                        1.0f, -1.0f, 0.0f,
                        1.0f, 1.0f, 0.0f
                };

        //******************* Colors **************************

        final float triangleColors[] = new float[]
                {
                        1.0f,0.0f,0.0f, //Red
                        0.0f,1.0f,0.0f, //Green
                        0.0f,0.0f,1.0f //Blue
                };

        //Triangle VAO
        GLES32.glGenVertexArrays(1, vao_triangle, 0);
        GLES32.glBindVertexArray(vao_triangle[0]);

        GLES32.glGenBuffers(1, vbo_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(triangleVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                triangleVertices.length * 4,
                verticesBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        //For color
        GLES32.glGenBuffers(1, vbo_color, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

        ByteBuffer byteColorBuffer = ByteBuffer.allocateDirect(triangleColors.length * 4);
        byteColorBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer colorBuffer = byteColorBuffer.asFloatBuffer();
        colorBuffer.put(triangleColors);
        colorBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                triangleColors.length * 4,
                colorBuffer,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_COLOR,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_COLOR);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);

        // Square VAO
        GLES32.glGenVertexArrays(1, vao_square, 0);
        GLES32.glBindVertexArray(vao_square[0]);

        GLES32.glGenBuffers(1, vbo_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

        ByteBuffer byteBufferSquare = ByteBuffer.allocateDirect(squareVertices.length * 4);
        byteBufferSquare.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBufferSquare = byteBufferSquare.asFloatBuffer();
        verticesBufferSquare.put(squareVertices);
        verticesBufferSquare.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                squareVertices.length * 4,
                verticesBufferSquare,
                GLES32.GL_STATIC_DRAW);

        GLES32.glVertexAttribPointer(GLESMacros.VDG_ATTRIBUTE_VERTEX,
                3,
                GLES32.GL_FLOAT,
                false, 0, 0);

        GLES32.glEnableVertexAttribArray(GLESMacros.VDG_ATTRIBUTE_VERTEX);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


        //For color
        GLES32.glVertexAttrib3f(GLESMacros.VDG_ATTRIBUTE_COLOR, 0.392f, 0.584f, 0.929f);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        //GLES32.glEnable(GLES32.GL_CULL_FACE);

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

        //OpenGL-ES Drawing
        float modelviewMatrix[] = new float[16];
        float modelviewProjectionmatrix[] = new float[16];
        float rotationMatrix[] = new float[16];

        //Triangle
        Matrix.setIdentityM(modelviewMatrix, 0);
        Matrix.setIdentityM(modelviewProjectionmatrix, 0);
        Matrix.setIdentityM(rotationMatrix,0);

        Matrix.translateM(modelviewMatrix,0, -1.5f, 0.0f, -5.0f);
        Matrix.rotateM(rotationMatrix,0,gAngle_triangle,0.0f,1.0f,0.0f);

        Matrix.multiplyMM(modelviewMatrix,0,modelviewMatrix,0,rotationMatrix,0);
        Matrix.multiplyMM(modelviewProjectionmatrix, 0, perspectiveProjectionMatrix, 0, modelviewMatrix, 0);

        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelviewProjectionmatrix, 0);

        GLES32.glBindVertexArray(vao_triangle[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

        GLES32.glBindVertexArray(0);

        //square
        Matrix.setIdentityM(modelviewMatrix, 0);
        Matrix.setIdentityM(modelviewProjectionmatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.translateM(modelviewMatrix,0, 1.5f, 0.0f, -5.0f);
        Matrix.rotateM(rotationMatrix, 0, gAngle_square, 1.0f, 0.0f, 0.0f);

        Matrix.multiplyMM(modelviewMatrix, 0, modelviewMatrix, 0, rotationMatrix, 0);
        Matrix.multiplyMM(modelviewProjectionmatrix, 0, perspectiveProjectionMatrix, 0, modelviewMatrix, 0);


        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelviewProjectionmatrix, 0);

        GLES32.glBindVertexArray(vao_square[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

        update();

        requestRender();

    }

    private void unintialize() {
        if (vao_triangle[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
            vao_triangle[0] = 0;
        }

        if (vao_square[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0] = 0;
        }


        if(vbo_position[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_position, 0);
            vbo_position[0] = 0;
        }

        if(vbo_color[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo_color, 0);
            vbo_color[0] = 0;
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
        gAngle_triangle = gAngle_triangle + 1.0f;
        if (gAngle_triangle >= 360.0f)
            gAngle_triangle = 0.1f;

        gAngle_square = gAngle_square + 1.0f;
        if (gAngle_square >= 360.0f)
            gAngle_square = 0.1f;
    }

}
