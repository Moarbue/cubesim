// current move
var move = 0;
// flag for changing state
var btnflag = false;

// Sets the current move and flag
function setMoveJS(_move) {
    move = _move;
    btnflag = true;
}
// Returns the current move and if it has changed
function getMoveJS() {
    let ret = { 0: move, 1: btnflag };
    btnflag = false;
    return ret;
}


// current algorithm
var algorithm = "";
// flag for changing state
var algoflag = false;

// Sets the current algorithm and flag
function setAlgorithmJS(_algorithm) {
    algorithm = _algorithm;
    algoflag = true;
}
// Returns the current algorithm and if it has changed
function getAlgorithmJS() {
    let ret = { 0: algorithm, 1: algoflag };
    algoflag = false;

    return ret;
}


// current cube faces
var faces = "";
// flag for changing state
var faceflag = false;

// Sets the current faces and flag
function setFacesJS(_faces) {
    faces = _faces;
}
// Returns the current faces and if it has changed
function getFacesJS() {
    let ret = { 0: faces, 1: faceflag };
    faceflag = false;

    return ret;
}


// fov of the scene
var fov = 30.0;
// flag for changing state
var fovflag = false;

// Returns the current fov and if it has changed
function getFovJS() {
    let ret = { 0: fov, 1: fovflag };
    fovflag = false;

    return ret;
}


// old horizontal cursor position
var old_x = 0;
// old vertical cursor position
var old_y = 0;
// current horizontal cursor position
var cur_x = 0;
// current vertical cursor position
var cur_y = 0;
// flag for changing state
var cursorflag = false;

// Calculates the change between the last two cursor positions and returns them and if they changed
function getCursorPosJS() {
    let ret = { 0: cur_x, 1: cur_y, 2: cursorflag };
    cursorflag = false;

    return ret;
}



// *** Events and functions to handle them

// set move buttons event handlers
var movebtns = document.querySelectorAll(".movebtn");
var i = 0, length = movebtns.length;
for (i; i < length; i++) {
    if (document.addEventListener) {
        movebtns[i].addEventListener("mousedown", function(e) {
            btnOnClick(e, this);});
        movebtns[i].addEventListener('contextmenu', event => event.preventDefault());
    };
};


// Sets the current move depending on which mouse button and which button was pressed
function btnOnClick(e, btn) {
    let _move = btn.innerText;
    if (e.button == 1) _move += "2";
    if (e.button == 2) _move += "\'";
    setMoveJS(parseInt(btn.id[1]) + 6 * e.button);
}

// set textbox event handlers
var textBox = document.getElementById("algoin");
textBox.addEventListener("keyup", function (e) {

    // new algorithm on enter
    if (e.key == "Enter") {
        let str = textBox.value;
        str = str.toUpperCase();
        str = str.replace(/\s/g, '');
        setAlgorithmJS(str);
        textBox.value = "";
    }
    // delete character on delete
    if (e.key == "Backspace") {
        textBox.value = textBox.value.slice(0, -1);
    }
});

// canvas element
var canvas = document.querySelector("canvas");
var insidecanvas = false;

document.addEventListener("mouseover", function(e) {
    if (e.target == canvas) {
        insidecanvas = true;
        canvas.focus();
    }
    else                    insidecanvas = false;
})


var search = new min2phase.Search();
min2phase.initFull();

document.addEventListener("keydown", function(e) {
    if (insidecanvas) {
        let move = -13;
        let _movestr = "";
        switch(e.key) {
            case "u":
            case "U":
                e.preventDefault();
                move = 0;
                _movestr = e.key.toUpperCase();
                break;
            case "l":
            case "L":
                e.preventDefault();
                move = 1;
                _movestr = e.key.toUpperCase();
                break;
            case "f":
            case "F":
                e.preventDefault();
                move = 2;
                _movestr = e.key.toUpperCase();
                break;
            case "d":
            case "D":
                e.preventDefault();
                move = 3;
                _movestr = e.key.toUpperCase();
                break;
            case "r":
            case "R":
                e.preventDefault();
                move = 4;
                _movestr = e.key.toUpperCase();
                break;
            case "b":
            case "B":
                e.preventDefault();
                move = 5;
                _movestr = e.key.toUpperCase();
                break;
            case "s":
                scramble();
                break;
            case "S":
                solve();
                break;
        }

        if (e.shiftKey)   move += 6;
        if (e.ctrlKey)  move += 12;

        if (move >= 0) {
            setMoveJS(move);
            _movestr += e.ctrlKey ? "\'" : e.shiftKey ? "2" : "";
        }
    }
});

// Scrambles the cube based on min2phase solver
function scramble() {
    let scrambledcube = search.solution(min2phase.randomCube(), 21).toUpperCase().replace(/\s/g, '');
    setAlgorithmJS(scrambledcube);
}

// Solves the cube based on min2phase solver
function solve() {
    let sol = search.solution(getFacesJS()[0], 21).toUpperCase().replace(/\s/g, '');
    setAlgorithmJS(sol);
}

// Set mouseup and mouse down events
var leftpressed = false;
canvas.addEventListener("mousedown", function(e) {
    if (e.button == 0)
        leftpressed = true;
});
canvas.addEventListener("mouseup", function(e) {
    leftpressed = false;
});

// Set mouse move event
canvas.addEventListener("mousemove", function(e) {
    onMotion(e);
});

// Function that is executed when the cursor is moved
function onMotion(e) {
    let rect = canvas.getBoundingClientRect();
    x = e.clientX - rect.left;
    y = e.clientY - rect.top;

    if (leftpressed) {
        cur_x = old_x - x;
        cur_y = y - old_y;

        cursorflag = true;
    }

    old_x = x;
    old_y = y;
}


// Set mouse wheel event
canvas.addEventListener("wheel", function(e) {
    onScroll(e);
});

// function that is called when the mouse wheel is scrolled
function onScroll(e) {
    if (fov >= 1.0 && fov <= 90.0)
        fov += e.deltaY * 0.01;
    if (fov <= 1.0)
        fov = 1.0;
    if (fov >= 90.0)
        fov = 90.0;
    fovflag = true;
}