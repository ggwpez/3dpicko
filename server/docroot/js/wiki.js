/*
 * SPDX-FileCopyrightText: 2018-2020 Harald Gültig <hgueltig@gmail.com>
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 * SPDX-FileCopyrightText: 2018-2020 Patrick Mayer <patrick.mayer@posteo.de>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

// This file can be included in index.js it is a seperate file to make development easier.

//loads front page#
$(function(){
  $("#wiki-content").load("./docs/wiki.html #introduction");
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
    $('.wiki-link').removeClass("btn-primary");
    $('.wiki-link').addClass("btn-link")
}

function loadHelp(id, headline){
	let path = './docs/wiki.html';
	console.log(path+" #"+id);
	if(headline) $("#help-headline").text(headline);  
	else $("#help-headline").load(path+" #"+id+"-h");
    $("#help-body").load(path+" #"+id);
}
