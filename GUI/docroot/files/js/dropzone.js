var dropZone = document.getElementById('dropZone');

// Optional.   Show the copy icon when dragging over.  Seems to only work for chrome.
dropZone.addEventListener('dragover', function(e) {
    e.stopPropagation();
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
});

// Get file data on drop
var rawData = new ArrayBuffer();   // TODO
dropZone.addEventListener('drop', function(e) {

    e.stopPropagation();
    e.preventDefault();
    var file = e.dataTransfer.files[0]; // Array of all files

    var reader = new FileReader();

    reader.onload = function(e) {
        // finished reading file data.
        rawData = e.target.result;
        var base64result = rawData.substr(rawData.indexOf(',') + 1);
        api("uploadimage", {filename: file.name, file: base64result});
    }

    reader.readAsDataURL(file); // start reading the file data.
});