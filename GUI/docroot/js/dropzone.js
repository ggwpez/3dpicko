$(function() {
    var dropZone = document.getElementById('dropZone');

    dropZone.addEventListener('dragover', function(e) {
        e.stopPropagation();
        e.preventDefault();
        e.dataTransfer.dropEffect = 'copy';
    });
    dropZone.addEventListener('dragenter', function(e) {
        dropZone.addClass('dragover');
    });
    dropZone.addEventListener('dragleave', function(e) {
        dropZone.removeClass('dragover');
    });

    // drag and drop
    dropZone.addEventListener('drop', function(e) {
        e.stopPropagation();
        e.preventDefault();
        dropZone.removeClass('dragover');
        upload(e.dataTransfer.files[0]);
    });
    // classic file selection
    $('#classicUpload').bind('change', function(e) {
        // upload multiple files
        for(i=0; i<this.files.length; i++){
            upload(this.files[i]);
        }
    });
});

function upload(file){
    if(file){
        let rawData = new ArrayBuffer();    // TODO
        let reader = new FileReader();
        reader.readAsDataURL(file);
        reader.onload = function(e) {
            rawData = e.target.result;
            var base64result = rawData.substr(rawData.indexOf(',') + 1);
            api("uploadimage", {original_filename: file.name, file: base64result});
        };
    }
}