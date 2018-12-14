// excecuted at startup
(function loadImages(){
	var d = new Date();
    imageId = 2;
	imagesDiv = document.getElementById('images');  
	for(id=0;id<12;id++){
		html = '\
		<div class="card m-1 float-left" id="card'+id+'" style="max-width: 200px;">\
			<button type="button" class="close" data-toggle="modal" data-target="#deleteDialog" onclick="select(\'card'+id+'\')">&times;</button>\
			<div href="link.html" class="card-body pb-0" onclick="location.href=\'createpickjob.html?image='+encodeURI(imageId)+'\';" style="cursor: pointer;">\
				<h5 class="card-title">Datei '+(id+1)+'</h5>\
				<img class="card-img" src="img/colony'+imageId+'.jpg" alt="Bild der Agarplatte">\
				<p class="card-text">Nutzer: Kabisch<br>Datum: '+d.toLocaleDateString()+'</li></p>\
			</div>\
		</div>';
		imagesDiv.insertAdjacentHTML('beforeend',html);
	}
})();

var selectedCard;
function select(id) {
	selectedCard = document.getElementById(id)
}
function del(){
	selectedCard.style.display = "none";
}