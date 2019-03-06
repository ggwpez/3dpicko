var upload_timeout_id;
$(function() {
    window.addEventListener("dragover",function(e){
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
    },false);
    window.addEventListener("drop",function(e){
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
        // TODO upload on drop outside of dropzone?
        // upload(e.dataTransfer.files[0]);
    },false);
    document.getElementById('overlay').addEventListener("dragover", function(e){
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
    }, false);
    document.getElementById('overlay').addEventListener("drop", function(e){
        e = e || event;
        e.stopPropagation();
        e.preventDefault();
    }, false);

    let dropZone = document.getElementById('dropZone');
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
        upload(e.dataTransfer.files[0]);
    });
    // classic file selection
    $('#classicUpload').bind('change', function(e) {
        upload(this.files[0]);
    });
});

function upload(file){
    dropZone.removeClass('dragover');
    $('#overlay').show();
    upload_timeout_id = setTimeout(function(){
        $('#overlay').hide();
        ShowAlert("Image Upload Timeout", "danger");
    }, 10000);
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