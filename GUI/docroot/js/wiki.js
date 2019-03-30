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
    //moveWiki(id);

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

function loadHelp(id, headline){
	let path = './docs/wiki.html';
	console.log(path+" #"+id);
	if(headline) $("#help-headline").text(headline);  
	else $("#help-headline").load(path+" #"+id+"-h");
    $("#help-body").load(path+" #"+id);
}
