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

var vao_pyramid;
var vao_cube;
var mvpuniform;

var gAngle_pyramid = 0.0;
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
                        "in vec4 vColor;" +
                        "uniform mat4 u_mvp_matrix;" +
                        "out vec4 out_color;" +
                        "void main(void)" +
                        "{" +
                        "gl_Position = u_mvp_matrix * vPosition;" +
                         "out_color = vColor;" +
                        "}";

	vertexShaderObject = compileShader(vertexShaderSource, gl.VERTEX_SHADER);

	var fragmentShaderSource = 
						"#version 300 es" +
                        "\n" +
                        "precision highp float;" +
                        "in vec4 out_color;" +
                        "out vec4 FragColor;" +
                        "void main(void)" +
                        "{" +
                        "FragColor = out_color;" +
                        "}";

	fragmentShaderObject = compileShader(fragmentShaderSource, gl.FRAGMENT_SHADER);
	
	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, webGLMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, webGLMacros.VDG_ATTRIBUTE_COLOR, "vColor");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	getAllUniformLocations();

	var pyramidVertices = new Float32Array([
						0.0, 1.0, 0.0, //left
                        1.0, -1.0, 1.0,
                        1.0, -1.0, -1.0,

                        0.0, 1.0, 0.0, //front
                        1.0, -1.0, 1.0,
                        -1.0, -1.0, 1.0,

                        0.0, 1.0, 0.0, //right
                        -1.0, -1.0, 1.0,
                        -1.0, -1.0, -1.0,

                        0.0, 1.0, 0.0, //back
                        -1.0, -1.0, -1.0,
                        1.0, -1.0, -1.0
						]);

	var pyramidColours = new Float32Array([
						1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0,

                        1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0,

                        1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0,
                        0.0, 1.0, 0.0,

                        1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 0.0, 1.0
						]);

	var cubeVertices = new Float32Array([
						1.0, 1.0, 1.0, //front
                        1.0, -1.0, 1.0,
                        -1.0, -1.0, 1.0,
                        -1.0, 1.0, 1.0,

                        1.0, 1.0, -1.0, //back
                        1.0, -1.0, -1.0,
                        -1.0, -1.0, -1.0,
                        -1.0, 1.0, -1.0,

                        1.0, 1.0, 1.0, //right
                        1.0, 1.0, -1.0,
                        1.0, -1.0, -1.0,
                        1.0, -1.0, 1.0,

                        -1.0, 1.0, 1.0, //left
                        -1.0, 1.0, -1.0,
                        -1.0, -1.0, -1.0,
                        -1.0, -1.0, 1.0,

                        1.0, 1.0, 1.0, //top
                        -1.0, 1.0, 1.0,
                        -1.0, 1.0, -1.0,
                        1.0, 1.0, -1.0,

                        1.0, -1.0, 1.0, //bottom
                        -1.0, -1.0, 1.0,
                        -1.0, -1.0, -1.0,
                        1.0, -1.0, -1.0
						]);

	var cubeColours =  new Float32Array([
						1.0, 0.0, 0.0, //red
                        1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0,
                        1.0, 0.0, 0.0,

                        0.0, 1.0, 0.0, //green
                        0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,

                        0.0, 0.0, 1.0, //blue
                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0,
                        0.0, 0.0, 1.0,

                        1.0, 1.0, 0.0, //yellow
                        1.0, 1.0, 0.0,
                        1.0, 1.0, 0.0,
                        1.0, 1.0, 0.0,

                        1.0, 0.0, 1.0, //magenta
                        1.0, 0.0, 1.0,
                        1.0, 0.0, 1.0,
                        1.0, 0.0, 1.0,

                        0.0, 1.0, 1.0, //cyan
                        0.0, 1.0, 1.0,
                        0.0, 1.0, 1.0,
                        0.0, 1.0, 1.0
						]);

	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_VERTEX, pyramidVertices);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_COLOR, pyramidColours);
	gl.bindVertexArray(null);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_VERTEX, cubeVertices);
	bindBufferData(webGLMacros.VDG_ATTRIBUTE_COLOR, cubeColours);
	gl.bindVertexArray(null);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    //gl.enable(gl.CULL_FACE);	
	gl.clearColor(0.0,0.0,0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();

}

function getAllUniformLocations() {
	mvpuniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");
}

function draw() {
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.useProgram(shaderProgramObject)
	drawPyramid([-2.0, 0.0, -6.0]);
	drawCube([2.0, 0.0, -6.0]);
	gl.useProgram(null);

	update();
	requestAnimationFrame (draw, canvas);

}

function drawPyramid(position3f) {
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	mat4.translate(modelViewMatrix, modelViewMatrix, position3f);
	mat4.rotate(rotationMatrix, rotationMatrix, gAngle_pyramid, [0.0, 1.0, 0.0]);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix );
	gl.uniformMatrix4fv(mvpuniform, false, modelViewProjectionMatrix);
	gl.bindVertexArray(vao_pyramid);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 12);
	gl.bindVertexArray(null);
}

function drawCube(position3f) {
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	mat4.translate(modelViewMatrix, modelViewMatrix, position3f);
	mat4.rotate(rotationMatrix, rotationMatrix, gAngle_cube, [1.0, 1.0, 1.0]);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix );
	gl.uniformMatrix4fv(mvpuniform, false, modelViewProjectionMatrix);
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

	if(vao_pyramid) {
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo) {
		gl.deleteBuffer(vbo);
		vbo = null;
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
	gAngle_pyramid = gAngle_pyramid + rotationSpeed;
        if (gAngle_pyramid >= 360.0)
            gAngle_pyramid = 0.1;

        gAngle_cube = gAngle_cube + rotationSpeed;
        if (gAngle_cube >= 360.0)
            gAngle_cube = 0.1;
}