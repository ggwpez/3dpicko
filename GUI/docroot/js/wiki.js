// This file can be included in index.js it is a seperate file to make development easier.

//loads front page#
$(function(){
  $("#wiki-content").load("./docs/wiki.html #introduction");
  var element = document.getElementById("Introduction");
  try {
    element.classList.remove("btn-link");
  } catch {}
  try {
      element.classList.add("btn-primary");
      
} catch {}
});

function loadWiki(id, site){
    $("#wiki-content").load(site);
    moveWiki(id);
    var elementt = document.getElementById("Nomenclature");
    elementt.scrollIntoView();
    
    resetButtons();
    var element = document.getElementById(id);
    element.classList.remove("btn-link");
    element.classList.add("btn-primary");
}

function moveWiki(position){
    var element = document.getElementById(position);
    element.scrollIntoView();
}


function resetButtons(){
    var element = document.getElementById("About");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("Nomenclature");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("User-Manual-First-Steps");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("User-Manual-Example-Picking-Process");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("User-Manual-Advanced-Settings");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("Configuration-Hardware");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("Configuration-Software");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");

    var element = document.getElementById("Administration-Hardware");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
    
    var element = document.getElementById("Administration-Software");
    element.classList.remove("btn-primary");
    element.classList.add("btn-link");
}

<<<<<<< GUI/docroot/js/wiki.js

function loadHelpString(headline, site){
    $("#help-headline").text(headline);
	$("#help-body").load(site);
}


function loadHelp(headline, site){
	$("#help-headline").load(headline);
    $("#help-body").load(site);
=======
function loadHelp(id){
	let path = './docs/wiki.html';
	/*
	switch (headline) {
		case "area":
			headline = "Area";
			break;
		case "aabb_ratio":
			headline = "AABB Side Ratio";
			break;
		case "bb_ratio":
			headline = "BB Side Ratio";
			break;		
		case "convexity":
			headline = "Convexity";
			break;		
		case "circularity":
			headline = "Circularity";
			break;		
		default:
	}
	*/
    //$("#help-headline").text(headline);
	$("#help-headline").load(path+" #"+id+"-h");
    $("#help-body").load(path+" #"+id);
>>>>>>> GUI/docroot/js/wiki.js
}
