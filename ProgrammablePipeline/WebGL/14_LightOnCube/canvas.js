// global variables
var canvas=null;
var context=null;
var gl=null;
var bFullscreen = false;

var canvas_original_width;
var canvas_original_height;

const webGLMacros = {
	VDG_ATTRIBUTE_VERTEX:0,
	VDG_ATTRIBUTE_COLOR:1,
	VDG_ATTRIBUTE_NORMAL:2,
	VDG_ATTRIBUTE_TEXTURE0:3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_cube;
var modelViewMatrixUniform, projectionMatrixUniform;
var ldUniform, kdUniform, lightPositionUniform;
var doubleTapUniform;

var rotateCube = false; // for animation
var lightEnabled = false ; // for lights

var gAngle_cube = 0.0;
var rotationSpeed = 0.02;

var perspectiveProjectionMatrix;


//to start animation

var requestAnimationFrame = 
	window.requestAnimationFrame ||
	window.webkitRequestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;


function main() {
	canvas = document.getElementById('AMC')
	if(!canvas) {
		console.log("Obtaining canvas failed.\n");
	} else {
		console.log("Obtaining canvas succeeded.\n");
	}

	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;

	//print canvas height and width
	console.log('Canvas width : '+canvas.width +' And canvas height : ' + canvas.height);

	
	if(!context) {
		console.log("Obtaining 2D context failed.\n");
	} else {
		console.log("Obtaining 2D context succeeded.\n");
	}


	//drawText('Hello World !!!');

	window.addEventListener('keydown', keydown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    init();

    resize();
    draw();
}

function compileShader(shader, shaderType) {
	var shaderObject =  gl.createShader(shaderType);
	gl.shaderSource(shaderObject, shader);
	gl.compileShader(shaderObject);
	if(gl.getShaderParameter(shaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(shaderObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}
	return shaderObject;
}

function init() {
	gl = canvas.getContext('webgl2');
	if(gl==null) {
		console.log("Failed to get rendering context..\n");
		return;
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	var vertexShaderSource = 
						"#version 300 es" +
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
                        "}";

	vertexShaderObject = compileShader(vertexShaderSource, gl.VERTEX_SHADER);

	var fragmentShaderSource = 
						"#version 300 es" +
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
                        "}";

	fragmentShaderObject = compileShader(fragmentShaderSource, gl.FRAGMENT_SHADER);
	
	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, webGLMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, webGLMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	getAllUniformLocations();

	var cubeVertices = new Float32Array([
						// top surface
                        1.0, 1.0,-1.0,
                        -1.0, 1.0,-1.0,
                        -1.0, 1.0, 1.0,
                        1.0, 1.0, 1.0,

                        // bottom surface
                        1.0,-1.0, 1.0,
                        -1.0,-1.0, 1.0,
                        -1.0,-1.0,-1.0,
                        1.0,-1.0,-1.0,

                        // front surface
                        1.0, 1.0, 1.0,
                        -1.0, 1.0, 1.0,
                        -1.0,-1.0, 1.0,
                        1.0,-1.0, 1.0,

                        // back surface
                        1.0,-1.0,-1.0,
                        -1.0,-1.0,-1.0,
                        -1.0, 1.0,-1.0,
                        1.0, 1.0,-1.0,

                        // left surface
                        -1.0, 1.0, 1.0,
                        -1.0, 1.0,-1.0,
                        -1.0,-1.0,-1.0,
                        -1.0,-1.0, 1.0,

                        // right surface
                        1.0, 1.0,-1.0,
                        1.0, 1.0, 1.0,
                        1.0,-1.0, 1.0,
                        1.0,-1.0,-1.0	
						]);

	var cubeNormals =  new Float32Array([
						0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,

                        0.0, -1.0, 0.0,
                        0.0, -1.0, 0.0,
                        0.0, -1.0, 0.0,
                        0.0, -1.0, 0.0,

                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0,

                        0.0, 0.0, -1.0,
                        0.0, 0.0, -1.0,
                        0.0, 0.0, -1.0,
                        0.0, 0.0, -1.0,

                        -1.0, 0.0, 0.0,
                        -1.0, 0.0, 0.0,
                        -1.0, 0.0, 0.0,
                        -1.0, 0.0, 0.0,

                        1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0
						]);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_VERTEX, cubeVertices);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_NORMAL, cubeNormals);
	gl.bindVertexArray(null);

	gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);
	gl.clearColor(0.0, 0.0, 1.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();

}

function getAllUniformLocations() {

	 	modelViewMatrixUniform =  gl.getUniformLocation(shaderProgramObject, "u_model_view_matrix");
        projectionMatrixUniform =  gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
        doubleTapUniform =  gl.getUniformLocation(shaderProgramObject, "u_double_tap");
        ldUniform =  gl.getUniformLocation(shaderProgramObject, "u_Ld");
        kdUniform =  gl.getUniformLocation(shaderProgramObject, "u_Kd");
        lightPositionUniform =  gl.getUniformLocation(shaderProgramObject, "u_light_position");
}

function draw() {
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.useProgram(shaderProgramObject)

    if(lightEnabled)
    {
        gl.uniform1i(doubleTapUniform, 1);

        // setting light properties
        gl.uniform3fv(ldUniform, [1.0, 1.0, 1.0]); // diffuse intensity of light
        gl.uniform3fv(kdUniform, [0.5, 0.5, 0.5]); // diffuse reflectivity of material
        var lightPosition = [0.0, 0.0, 2.0, 1.0];
        gl.uniform4fv(lightPositionUniform, lightPosition); // light position
    }
    else
    {
        gl.uniform1i(doubleTapUniform, 0);
    }

    drawCube([0.0, 0.0, -6.0]);
	gl.useProgram(null);

	if(rotateCube)
		update();

	requestAnimationFrame (draw, canvas);

}

function drawCube(position3f) {
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	mat4.translate(modelViewMatrix, modelViewMatrix, position3f);
	mat4.rotate(rotationMatrix, rotationMatrix, gAngle_cube, [1.0, 1.0, 1.0]);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix );
	gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
	gl.bindVertexArray(vao_cube);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
	gl.bindVertexArray(null);
}

function bindBufferData(webGLMacro, dataArray ) {
	var vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.bufferData(gl.ARRAY_BUFFER, dataArray, gl.STATIC_DRAW);
	gl.vertexAttribPointer (webGLMacro, 
							3, 
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(webGLMacro);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	return vbo;
}

function resize() {
	if(bFullscreen==true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	} else {
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}

	console.log("In resize	 : "+ canvas.width);
	gl.viewport(0, 0, canvas.width, canvas.height);

	if(canvas.width <= canvas.height) {
		mat4.perspective(perspectiveProjectionMatrix, 45.0, (parseFloat(canvas.height)/parseFloat(canvas.width)), 0.1, 100.0);
	} else {
		mat4.perspective(perspectiveProjectionMatrix, 45.0, (parseFloat(canvas.width)/parseFloat(canvas.height)), 0.1, 100.0);
	}
}

function uninitialize() {
	if(vao_cube) {
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
	}


	if(shaderProgramObject) {
		if(vertexShaderObject) {
			gl.detachShader(shaderProgramObject, vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject = null;
		}

		if(fragmentShaderObject) {
			gl.detachShader(shaderProgramObject, fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject = null;
		}
	}

	gl.deleteProgram(shaderProgramObject);
	shaderProgramObject = null;
}

function keydown(event) {
	switch(event.keyCode)
	{
		case 70 : 
			toggleFullScreen();
			break;

		case 82 :
			rotateCube = !rotateCube;
			console.log('Rotate');
			break;
		case 76 :
			lightEnabled = !lightEnabled;
			break;
	}
}

function mouseDown() {
	alert('mouse is clicked.');
}

function toggleFullScreen() {
	var fullscreen_element = 
		document.fullscreenElement ||
		document.webkitFullscreenElement ||
		document.mozFullScreenElement ||
		document.msFullscreenElement ||
		null;

	if(fullscreen_element == null) {
		if(canvas.requestFullscreen) 
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen) 
			canvas.mozRequestFullscreen();
		else if(canvas.webkitRequestFullscreen) 
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen) 
			canvas.msRequestFullscreen();
		        bFullscreen=true;
	} else {
		if(document.exitFullscreen) 
			document.exitFullscreen();
		else if(document.mozCancelFullScreen) 
			document.mozCancelFullScreen();
		else if(document.webkitExitFullscreen) 
			document.webkitExitFullscreen();
		else if(document.msExitFullscreen) 
			document.msExitFullscreen();
		        bFullscreen=false;

	}
}

function update() {
        gAngle_cube = gAngle_cube + rotationSpeed;
        if (gAngle_cube >= 360.0)
            gAngle_cube = 0.1;
}