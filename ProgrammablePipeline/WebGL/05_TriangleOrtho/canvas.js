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

var vao;
var vbo;
var mvpuniform;

var orthographicProjectionMatrix;


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

	
	if(!gl) {
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

function init() {
	gl = canvas.getContext('webgl2');
	if(gl==null) {
		console.log("Failed to get rendering context..\n");
		return;
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	var vertexShaderSource = 
		"#version 300 es"+
		"\n"+
		"in vec4 vPosition;"+
		"uniform mat4 u_mvp_matrix;"+
		"void main(void)"+
		"{"+
		"gl_Position = u_mvp_matrix * vPosition;"+
		"}";

	vertexShaderObject =  gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSource);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	var fragmentShaderSource = 
		"#version 300 es"+
		"\n"+
		"precision highp float;"+
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"FragColor = vec4 (1.0,1.0,1.0,1.0);"+
		"}";

	fragmentShaderObject =  gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSource);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, webGLMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	mvpuniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	var triangleVertices = new Float32Array([
												0.0, 50.0, 0.0,
												-50.0, -50.0, 0.0,
												50.0, -50.0, 0.0
											]);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer (webGLMacros.VDG_ATTRIBUTE_VERTEX, 
							3, 
							gl.FLOAT,
							false, 0, 0);
	gl.enableVertexAttribArray(webGLMacros.VDG_ATTRIBUTE_VERTEX);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	gl.clearColor(0.0,0.0,1.0, 1.0);

	orthographicProjectionMatrix = mat4.create();

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
		mat4.ortho(orthographicProjectionMatrix, -100.0, 100.0, (-100 * (canvas.height/canvas.width)), (100.0 * (canvas.height/canvas.width)), -100.0, 100.0);
	} else {
		mat4.ortho(orthographicProjectionMatrix, -100.0, 100.0, (-100 * (canvas.width/canvas.height)), (100.0 * (canvas.width/canvas.height)), -100.0, 100.0);
	}
}

function draw() {
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();

	mat4.multiply(modelViewProjectionMatrix, orthographicProjectionMatrix, modelViewMatrix );
	gl.uniformMatrix4fv(mvpuniform, false, modelViewProjectionMatrix);
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.TRIANGLES, 0, 3);
	gl.bindVertexArray(null);
	gl.useProgram(null);

	requestAnimationFrame (draw, canvas);
}

function uninitialize() {
	if(vao) {
		gl.deleteVertexArray(vao);
		vao = null;
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