// This file can be included in index.js it is a seperate file to make development easier.

//loads front page#
$(function(){
  $("#wiki-content").load("./docs/Introduction.html"); 
});

function loadWiki(site){
    $("#wiki-content").load(site);
}

function loadHelp(headline, site){
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
    $("#help-headline").text(headline);
	//$("#help-headline").load(headline);
    $("#help-body").load(site);
}