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

var sphere = null;
var lightEnabled = false ; // for lights

var gAngle =0.0;
var gRedious = 50.0;
var x_axis_rotation = false, y_axis_rotation = false, z_axis_rotation = false;
var perspectiveProjectionMatrix;

var light_ambient = [ 0.0, 0.0, 0.0];
var light_diffuse = [ 1.0, 1.0, 1.0];
var light_specular = [ 1.0, 1.0, 1.0];
var light_position = [ 100.0, 100.0, 100.0, 1.0];

var material_ambient = [ [ 0.0215, 0.1745, 0.0215],[ 0.135, 0.2225, 0.1575],[ 0.05375, 0.05, 0.06625],[ 0.25, 0.20725, 0.20725],[ 0.1745, 0.01175, 0.01175],[ 0.1, 0.18725, 0.1745],[ 0.329412, 0.223529, 0.027451],[ 0.2125, 0.1275, 0.054],[ 0.25, 0.25, 0.25],[ 0.19125, 0.0735, 0.0225],[ 0.24725, 0.1995, 0.0745],[ 0.19225, 0.19225, 0.19225],[ 0.0, 0.0, 0.0],[ 0.0, 0.1, 0.06],[ 0.0, 0.0, 0.0],[ 0.0, 0.0, 0.0],[ 0.0, 0.0, 0.0] ,[ 0.0, 0.0, 0.0],[ 0.02, 0.02, 0.02],[ 0.0, 0.05, 0.05],[ 0.0, 0.05, 0.0],[ 0.05, 0.0, 0.0],[ 0.05, 0.05, 0.05],[ 0.05, 0.05, 0.0] ];
var material_diffuse = [ [ 0.07568, 0.61424, 0.07568],[ 0.54, 0.89, 0.63],[ 0.18275, 0.17, 0.22525],[ 1.0, 0.829, 0.829],[ 0.61424, 0.04136, 0.04136],[ 0.396, 0.74151, 0.69102],[ 0.780392, 0.568627, 0.113725],[ 0.714, 0.4284, 0.18144],[ 0.4, 0.4, 0.4],[ 0.7038, 0.27048, 0.0828],[ 0.75164, 0.60648, 0.22648],[ 0.50754, 0.50754, 0.50754],[ 0.01, 0.01, 0.01],[ 0.0, 0.50980392, 0.50980392],[ 0.1, 0.35, 0.1],[ 0.5, 0.0, 0.0],[ 0.55, 0.55, 0.55],[ 0.5, 0.5, 0.0],[ 0.01, 0.01, 0.01],[ 0.4, 0.5, 0.5],[ 0.4, 0.5, 0.4],[ 0.5, 0.4, 0.4],[ 0.5, 0.5, 0.5],[ 0.5, 0.5, 0.4] ];
var material_specular = [ [ 0.633, 0.727811, 0.633],[ 0.316228, 0.316228, 0.316228],[ 0.332741, 0.328634, 0.346435],[ 0.296648, 0.296648, 0.296648],[ 0.727811, 0.626959, 0.626959],[ 0.297254, 0.30829, 0.306678],[ 0.992157, 0.941176, 0.807843],[ 0.393548, 0.271906, 0.166721],[ 0.774597, 0.774597, 0.774597],[ 0.256777, 0.137622, 0.086014],[ 0.628281, 0.555802, 0.366065],[ 0.508273, 0.508273, 0.508273],[ 0.50, 0.50, 0.50],[ 0.50196078, 0.50196078, 0.50196078],[ 0.45, 0.55, 0.45],[ 0.7, 0.6, 0.6],[ 0.70, 0.70, 0.70],[ 0.60, 0.60, 0.50],[ 0.4, 0.4, 0.4],[ 0.04, 0.7, 0.7],[ 0.04, 0.7, 0.04],[ 0.7, 0.04, 0.04],[ 0.7, 0.7, 0.7],[ 0.7, 0.7, 0.04] ];
var material_shininess = [ 0.6* 128, 0.1* 128, 0.3* 128, 0.088* 128, 0.6* 128, 0.1* 128, 0.21794872* 128, 0.2* 128, 0.6* 128, 0.1* 128, 0.4* 128, 0.4* 128, 0.25* 128, 0.25* 128, 0.25* 128, 0.25* 128, 0.25* 128, 0.25* 128, 0.078125* 128, 0.078125* 128, 0.078125* 128, 0.078125* 128, 0.078125* 128, 0.078125* 128 ];


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
			"#version 300 es"+
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
                        "}";

	vertexShaderObject = compileShader(vertexShaderSource, gl.VERTEX_SHADER);

	var fragmentShaderSource = 
			"#version 300 es"+
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
    makeSphere(sphere,1.0,30,30);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.CULL_FACE);

	gl.clearColor(0.25, 0.25, 0.25, 1.0);

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

       
    }
    else
    {
        gl.uniform1i(light_enable_uniform, 0);
    }

    var row =0, col =0 , count = 0;
    for (var index = 0; index < 24; index++) {
	    var modelMatrix = mat4.create();
	    var viewMatrix = mat4.create();
		
		mat4.translate(modelMatrix, modelMatrix, [-6.0 + col*2.5, 3.5 - row*2.5, -10.0]);

		gl.uniformMatrix4fv(model_matrix_uniform, false, modelMatrix);
		gl.uniformMatrix4fv(view_matrix_uniform, false, viewMatrix);
		gl.uniformMatrix4fv(projection_matrix_uniform, false, perspectiveProjectionMatrix);
		gl.uniform3fv(Ka_uniform, material_ambient[index]);
        gl.uniform3fv(Kd_uniform, material_diffuse[index]);
        gl.uniform3fv(Ks_uniform, material_specular[index]);
        gl.uniform1f(material_shininess_uniform, material_shininess[index]);
		sphere.draw();

		if (col < 5) {
                col++;
            }
            else {
                col = 0;
                row++;
            }

	}
	gl.useProgram(null);
	update();
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
		case 88 :
			
			x_axis_rotation = !x_axis_rotation;
			y_axis_rotation = false;
			z_axis_rotation = false;
			console.log("x_axis_rotation " + x_axis_rotation);
			break;
		
		case 89 :
			
			x_axis_rotation = false;
			y_axis_rotation = !y_axis_rotation;
			z_axis_rotation = false;
			console.log("y_axis_rotation " + y_axis_rotation);
			break;

		case 90 :
			
			x_axis_rotation = false;
			y_axis_rotation = false;
			z_axis_rotation = !z_axis_rotation;
			console.log("z_axis_rotation " + z_axis_rotation);
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
        gAngle = gAngle + 0.02;
        if (gAngle >= 360.0) {
            gAngle = 0.0;
        }
        //console.log(gAngle)
        if (x_axis_rotation) {
            light_position[0] = 0.0;
            light_position[1] = (gRedious * Math.cos(gAngle));
            light_position[2] = (gRedious * Math.sin(gAngle));
        } else if (y_axis_rotation) {
            light_position[0] = parseFloat(gRedious * Math.cos(gAngle));
            light_position[1] = 0.0;
            light_position[2] = parseFloat(gRedious * Math.sin(gAngle));
        } else if (z_axis_rotation) {
            light_position[0] = parseFloat(gRedious * Math.cos(gAngle));
            light_position[1] = parseFloat(gRedious * Math.sin(gAngle));
            light_position[2] = 0.0;
        }
    }