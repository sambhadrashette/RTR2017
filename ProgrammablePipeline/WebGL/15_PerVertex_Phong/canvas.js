// global variables
var canvas=null;
var context=null;
var gl=null;
var bFullscreen = false;

var canvas_original_width;
var canvas_original_height;

const WebGLMacros = {
	VDG_ATTRIBUTE_VERTEX:0,
	VDG_ATTRIBUTE_COLOR:1,
	VDG_ATTRIBUTE_NORMAL:2,
	VDG_ATTRIBUTE_TEXTURE0:3
};

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;

var La_uniform;
var Ld_uniform;
var Ls_uniform;
var light_position_uniform;

var Ka_uniform;
var Kd_uniform;
var Ks_uniform;
var material_shininess_uniform;

var light_enable_uniform;

var rotateCube = false; // for animation
var lightEnabled = false ; // for lights

var perspectiveProjectionMatrix;

var light_ambient=[0.0,0.0,0.0];
var light_diffuse=[1.0,1.0,1.0];
var light_specular=[1.0,1.0,1.0];
var light_position=[100.0,100.0,100.0,1.0];

var material_ambient= [0.0,0.0,0.0];
var material_diffuse= [1.0,1.0,1.0];
var material_specular= [1.0,1.0,1.0];
var material_shininess= 50.0;

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
            "}";

	vertexShaderObject = compileShader(vertexShaderSource, gl.VERTEX_SHADER);

	var fragmentShaderSource = 
			"#version 300 es" +
            "\n" +
            "precision highp float;" +
            "in vec3 phong_ads_color;" +
            "out vec4 FragColor;" +
            "void main(void)" +
            "{" +
            "FragColor = vec4(phong_ads_color, 1.0);" +
            "}";

	fragmentShaderObject = compileShader(fragmentShaderSource, gl.FRAGMENT_SHADER);
	
	shaderProgramObject = gl.createProgram();

	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_NORMAL, "vNormal");

	gl.linkProgram(shaderProgramObject);

	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length >0) {
			console.log(error);
			uninitialize();
		}
	}

	getAllUniformLocations();

	sphere=new Mesh();
    makeSphere(sphere,2.0,30,30);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);

	gl.clearColor(0.0,0.0,0.0, 1.0);

	perspectiveProjectionMatrix = mat4.create();

}

function getAllUniformLocations() {

    model_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
    view_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
    projection_matrix_uniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");
    light_enable_uniform = gl.getUniformLocation(shaderProgramObject, "u_lighting_enabled");
    La_uniform = gl.getUniformLocation(shaderProgramObject, "u_La");
    Ld_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ld");
    Ls_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ls");
    light_position_uniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");

    Ka_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
    Kd_uniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
    Ks_uniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");
    material_shininess_uniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");

}

function draw() {
	gl.clear(gl.COLOR_BUFFER_BIT);
	gl.useProgram(shaderProgramObject)
	if(lightEnabled)
    {
        gl.uniform1i(light_enable_uniform, 1);

        gl.uniform3fv(La_uniform, light_ambient);
        gl.uniform3fv(Ld_uniform, light_diffuse);
        gl.uniform3fv(Ls_uniform, light_specular);
        gl.uniform4fv(light_position_uniform, light_position);

        gl.uniform3fv(Ka_uniform, material_ambient);
        gl.uniform3fv(Kd_uniform, material_diffuse);
        gl.uniform3fv(Ks_uniform, material_specular);
        gl.uniform1f(material_shininess_uniform, material_shininess);
    }
    else
    {
        gl.uniform1i(light_enable_uniform, 0);
    }

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
	
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -6.0]);

	gl.uniformMatrix4fv(model_matrix_uniform, false, modelMatrix);
	gl.uniformMatrix4fv(view_matrix_uniform, false, viewMatrix);
	gl.uniformMatrix4fv(projection_matrix_uniform, false, perspectiveProjectionMatrix);

	sphere.draw();

	gl.useProgram(null);
	requestAnimationFrame (draw, canvas);

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

	if(sphere)
    {
        sphere.deallocate();
        sphere=null;
    }

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

		case 76 :
			lightEnabled = !lightEnabled;
			break;
	}
}

function mouseDown() {
	//alert('mouse is clicked.');
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