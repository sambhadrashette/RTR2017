// global variables
var canvas=null;
var context=null;

function main() {
	canvas = document.getElementById('AMC')
	if(!canvas) {
		console.log("Obtaining canvas failed.\n");
	} else {
		console.log("Obtaining canvas succeeded.\n");
	}

	//print canvas height and width
	console.log('Canvas width : '+canvas.width +' And canvas height : ' + canvas.height);

	//get 2D context
	context = canvas.getContext('2d');

	if(!context) {
		console.log("Obtaining 2D context failed.\n");
	} else {
		console.log("Obtaining 2D context succeeded.\n");
	}

	context.fillStyle = '#000000'
	context.fillRect(0,0,canvas.width, canvas.height);

	//centre the text
	context.textAlign = 'centre';
	context.textBaseline = 'middle';

	//text
	var str = 'Hello World';

	context.font ='48px sans-serif';

	context.fillStyle = '#FFFFFF';

	context.fillText(str, canvas.width/2, canvas.height/2);


	window.addEventListener('keydown', keydown, false);
    window.addEventListener("click", click, false);
}

function keydown() {
	alert('A key is pressed.');
}

function click() {
	alert('mouse is clicked.');
}